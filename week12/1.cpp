#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

const double XLength = 2.0, YLength = 1.0;  
const int XPoints = 60, YPoints = 30;      
const double deltaX = XLength / (XPoints - 1), deltaY = YLength / (YPoints - 1);
const double epsilon = 1e-6;              
const int maxIterations = 10000;           

double getBoundaryValue(int x, int y) {
    if (y == 0) return 0.0;                         // u(x, 0)
    if (y == YPoints - 1) return 0.5 * x * x + 0.5 * x; // u(x, YLength)
    if (x == 0) return 0.0;                         // u(0, y)
    if (x == XPoints - 1) return 2.0 * y * y + y;   // u(XLength, y)
    return 0.0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    int gridDims[2] = {0, 0};
    MPI_Dims_create(numProcs, 2, gridDims);
    int periodic[2] = {0, 0};
    MPI_Comm gridComm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, gridDims, periodic, 0, &gridComm);

    int coords[2], upNeighbor, downNeighbor, leftNeighbor, rightNeighbor;
    MPI_Cart_coords(gridComm, rank, 2, coords);
    MPI_Cart_shift(gridComm, 0, 1, &upNeighbor, &downNeighbor);
    MPI_Cart_shift(gridComm, 1, 1, &leftNeighbor, &rightNeighbor);

    int localX = XPoints / gridDims[0];
    int localY = YPoints / gridDims[1];
    int offsetX = coords[0] * localX;
    int offsetY = coords[1] * localY;

    vector<vector<double>> prevGrid(localX + 2, vector<double>(localY + 2, 0.0));
    vector<vector<double>> currGrid(localX + 2, vector<double>(localY + 2, 0.0));
    vector<vector<double>> sequentialResult(XPoints, vector<double>(YPoints, 0.0));

    for (int i = 0; i < XPoints; ++i) {
        for (int j = 0; j < YPoints; ++j) {
            if (i == 0 || i == XPoints - 1 || j == 0 || j == YPoints - 1) {
                sequentialResult[i][j] = getBoundaryValue(i, j);
            }
        }
    }

    if (rank == 0) {
        // seq for 0
        vector<vector<double>> u_temp = sequentialResult;
        double local_error;
        int iteration = 0;

        do {
            local_error = 0.0;
            for (int i = 1; i < XPoints - 1; ++i) {
                for (int j = 1; j < YPoints - 1; ++j) {
                    sequentialResult[i][j] = 0.25 * (u_temp[i - 1][j] + u_temp[i + 1][j] + u_temp[i][j - 1] + u_temp[i][j + 1]);
                    local_error = max(local_error, abs(sequentialResult[i][j] - u_temp[i][j]));
                }
            }
            u_temp = sequentialResult;
            ++iteration;
        } while (local_error > epsilon && iteration < maxIterations);

        ofstream seqFile("seq_result.txt");
        for (int i = 0; i < XPoints; ++i) {
            for (int j = 0; j < YPoints; ++j) {
                seqFile << i * deltaX << " " << j * deltaY << " " << sequentialResult[i][j] << "\n";
            }
        }
        seqFile.close();
    }

    double globalError = 1.0;
    int iteration = 0;

    for (int i = 0; i < localX + 2; ++i) {
        for (int j = 0; j < localY + 2; ++j) {
            int globalX = offsetX + i - 1;
            int globalY = offsetY + j - 1;
            if (globalX == 0 || globalX == XPoints - 1 || globalY == 0 || globalY == YPoints - 1) {
                prevGrid[i][j] = getBoundaryValue(globalX, globalY);
                currGrid[i][j] = getBoundaryValue(globalX, globalY);
            }
        }
    }

    while (globalError > epsilon && iteration < maxIterations) {
        for (int i = 1; i <= localX; ++i) {
            for (int j = 1; j <= localY; ++j) {
                currGrid[i][j] = 0.25 * (prevGrid[i - 1][j] + prevGrid[i + 1][j] + prevGrid[i][j - 1] + prevGrid[i][j + 1]);
            }
        }

        MPI_Status status;

        MPI_Sendrecv(&currGrid[1][1], localY, MPI_DOUBLE, upNeighbor, 0,
                     &currGrid[0][1], localY, MPI_DOUBLE, upNeighbor, 0, gridComm, &status);
        MPI_Sendrecv(&currGrid[localX][1], localY, MPI_DOUBLE, downNeighbor, 1,
                     &currGrid[localX + 1][1], localY, MPI_DOUBLE, downNeighbor, 1, gridComm, &status);

        vector<double> sendLeft(localX), recvLeft(localX);
        vector<double> sendRight(localX), recvRight(localX);

        for (int i = 1; i <= localX; ++i) {
            sendLeft[i - 1] = currGrid[i][1];
            sendRight[i - 1] = currGrid[i][localY];
        }

        MPI_Sendrecv(sendLeft.data(), localX, MPI_DOUBLE, leftNeighbor, 2,
                     recvRight.data(), localX, MPI_DOUBLE, rightNeighbor, 2, gridComm, &status);
        MPI_Sendrecv(sendRight.data(), localX, MPI_DOUBLE, rightNeighbor, 3,
                     recvLeft.data(), localX, MPI_DOUBLE, leftNeighbor, 3, gridComm, &status);

        for (int i = 1; i <= localX; ++i) {
            currGrid[i][0] = recvLeft[i - 1];
            currGrid[i][localY + 1] = recvRight[i - 1];
        }

        double localError = 0.0;
        for (int i = 1; i <= localX; ++i) {
            for (int j = 1; j <= localY; ++j) {
                localError = max(localError, fabs(currGrid[i][j] - prevGrid[i][j]));
            }
        }

        MPI_Allreduce(&localError, &globalError, 1, MPI_DOUBLE, MPI_MAX, gridComm);
        prevGrid.swap(currGrid);
        ++iteration;
    }

    if (rank == 0) {
        ofstream parFile("parallel_result.txt");
        for (int i = 0; i < XPoints; ++i) {
            for (int j = 0; j < YPoints; ++j) {
                parFile << i * deltaX << " " << j * deltaY << " " << sequentialResult[i][j] << "\n";
            }
        }
        parFile.close();
    }

    MPI_Finalize();
    return 0;
}
