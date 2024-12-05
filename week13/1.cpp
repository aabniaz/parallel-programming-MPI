#include <mpi.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

// (Inlet)
void inlet_bound_cond(vector<vector<double>>& u, vector<vector<double>>& v, int block_rows, int block_cols,
                int i1, int j1, double dx, double dy, int N, int M) {
    for (int i = 1; i <= block_rows; ++i) {
        for (int j = 1; j <= block_cols; ++j) {
            double x = (i1 + i - 1) * dx;
            double y = (j1 + j - 1) * dy;

            // inlet u
            if (i1 + i - 1 == 0) u[i][j] = 1.0; // Левый край, скорость U = 1 (Inlet)
            
            // inlet v
            if (j1 + j - 1 == 0) v[i][j] = 0.0; // Нижняя граница, скорость V = 0
            if (j1 + j - 1 == M - 1) v[i][j] = 0.0; // Верхняя граница, скорость V = 0
        }
    }
}

//  (Outlet)
void outlet_bound_cond(vector<vector<double>>& u, vector<vector<double>>& v, int block_rows, int block_cols,
                int i1, int j1, double dx, double dy, int N, int M) {
    for (int i = 1; i <= block_rows; ++i) {
        for (int j = 1; j <= block_cols; ++j) {
            double x = (i1 + i - 1) * dx;
            double y = (j1 + j - 1) * dy;

            // outlet u 
            if (i1 + i - 1 == N - 1) u[i][j] = 0.0; // Правая граница, скорость U = 0 (Outlet)
            
            // outlet v
            if (j1 + j - 1 == 0) v[i][j] = 0.0; // Нижняя граница, скорость V = 0
            if (j1 + j - 1 == M - 1) v[i][j] = 0.0; // Верхняя граница, скорость V = 0
        }
    }
}

int main() {
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Размер сетки
    int N = 128, M = 128;
    double dx = 1.0 / (N - 1), dy = 1.0 / (M - 1);
    double epsilon = 1e-6; 

    int dims[2] = {0, 0};  
    MPI_Dims_create(size, 2, dims); 

    int periods[2] = {0, 0};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm); 

    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords); 

    int top, bottom, left, right;
    MPI_Cart_shift(cart_comm, 0, 1, &top, &bottom); 
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right); 

    int i1 = (N * coords[0]) / dims[0];
    int i2 = (N * (coords[0] + 1)) / dims[0];
    int j1 = (M * coords[1]) / dims[1];
    int j2 = (M * (coords[1] + 1)) / dims[1];

    int block_rows = i2 - i1; 
    int block_cols = j2 - j1; 

    vector<vector<double>> u(block_rows + 2, vector<double>(block_cols + 2, 0.0));
    vector<vector<double>> v(block_rows + 2, vector<double>(block_cols + 2, 0.0));

    // Применяем граничные условия
    if (i1 == 0) { // Вход (inlet)
        inlet_bound_cond(u, v, block_rows, block_cols, i1, j1, dx, dy, N, M);
    }
    if (i1 == N - 1) { // Выход (outlet)
        outlet_bound_cond(u, v, block_rows, block_cols, i1, j1, dx, dy, N, M);
    }

    double max_diff;
    do {
        max_diff = 0.0;

        // Обмен граничными строками между процессами по оси X
        if (bottom != MPI_PROC_NULL) {
            MPI_Send(u[block_rows].data(), block_cols + 2, MPI_DOUBLE, bottom, 0, cart_comm);
            MPI_Recv(u[block_rows + 1].data(), block_cols + 2, MPI_DOUBLE, bottom, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(v[block_rows].data(), block_cols + 2, MPI_DOUBLE, bottom, 0, cart_comm);
            MPI_Recv(v[block_rows + 1].data(), block_cols + 2, MPI_DOUBLE, bottom, 0, cart_comm, MPI_STATUS_IGNORE);
        }
        if (top != MPI_PROC_NULL) {
            MPI_Recv(u[0].data(), block_cols + 2, MPI_DOUBLE, top, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(u[1].data(), block_cols + 2, MPI_DOUBLE, top, 0, cart_comm);
            MPI_Recv(v[0].data(), block_cols + 2, MPI_DOUBLE, top, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(v[1].data(), block_cols + 2, MPI_DOUBLE, top, 0, cart_comm);
        }

        // Обмен граничными строками между процессами по оси Y
        vector<double> left_col_u(block_rows + 2), right_col_u(block_rows + 2);
        vector<double> left_col_v(block_rows + 2), right_col_v(block_rows + 2);
        vector<double> left_ghost_u(block_rows + 2), right_ghost_u(block_rows + 2);
        vector<double> left_ghost_v(block_rows + 2), right_ghost_v(block_rows + 2);

        for (int i = 0; i < block_rows + 2; ++i) {
            left_col_u[i] = u[i][1];
            right_col_u[i] = u[i][block_cols];
            left_col_v[i] = v[i][1];
            right_col_v[i] = v[i][block_cols];
        }

        if (right != MPI_PROC_NULL) {
            MPI_Send(right_col_u.data(), block_rows + 2, MPI_DOUBLE, right, 0, cart_comm);
            MPI_Recv(right_ghost_u.data(), block_rows + 2, MPI_DOUBLE, right, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(right_col_v.data(), block_rows + 2, MPI_DOUBLE, right, 0, cart_comm);
            MPI_Recv(right_ghost_v.data(), block_rows + 2, MPI_DOUBLE, right, 0, cart_comm, MPI_STATUS_IGNORE);
            for (int i = 0; i < block_rows + 2; ++i) {
                u[i][block_cols + 1] = right_ghost_u[i];
                v[i][block_cols + 1] = right_ghost_v[i];
            }
        }

        if (left != MPI_PROC_NULL) {
            MPI_Recv(left_ghost_u.data(), block_rows + 2, MPI_DOUBLE, left, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(left_col_u.data(), block_rows + 2, MPI_DOUBLE, left, 0, cart_comm);
            MPI_Recv(left_ghost_v.data(), block_rows + 2, MPI_DOUBLE, left, 0, cart_comm, MPI_STATUS_IGNORE);
            MPI_Send(left_col_v.data(), block_rows + 2, MPI_DOUBLE, left, 0, cart_comm);
            for (int i = 0; i < block_rows + 2; ++i) {
                u[i][0] = left_ghost_u[i];
                v[i][0] = left_ghost_v[i];
            }
        }

        // Основной расчет
        for (int i = 1; i <= block_rows; ++i) {
            for (int j = 1; j <= block_cols; ++j) {
                if ((i1 + i - 1 == 0 || i1 + i - 1 == N - 1) ||
                    (j1 + j - 1 == 0 || j1 + j - 1 == M - 1)) {
                    continue;
                }
                double old_u = u[i][j];
                double old_v = v[i][j];
                u[i][j] = 0.25 * (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]);
                v[i][j] = 0.25 * (v[i - 1][j] + v[i + 1][j] + v[i][j - 1] + v[i][j + 1]);
                max_diff = max(max_diff, fabs(u[i][j] - old_u));
                max_diff = max(max_diff, fabs(v[i][j] - old_v));
            }
        }

        double global_max_diff;
        MPI_Allreduce(&max_diff, &global_max_diff, 1, MPI_DOUBLE, MPI_MAX, cart_comm);
        max_diff = global_max_diff;

    } while (max_diff > epsilon);

    vector<double> local_data;
    for (int i = 1; i <= block_rows; ++i) {
        for (int j = 1; j <= block_cols; ++j) {
            double x = (i1 + i - 1) * dx;
            double y = (j1 + j - 1) * dy;
            local_data.push_back(x);
            local_data.push_back(y);
            local_data.push_back(u[i][j]);
            local_data.push_back(v[i][j]);
        }
    }

    int local_size = local_data.size();
    int total_size = local_size * size;
    vector<double> global_data(total_size);

    MPI_Gather(local_data.data(), local_size, MPI_DOUBLE, global_data.data(), local_size, MPI_DOUBLE, 0, cart_comm);

    if (rank == 0) {
        ofstream outfile("result_parallel.txt");
        for (size_t i = 0; i < global_data.size(); i += 4) {
            outfile << global_data[i] << " " << global_data[i + 1] << " " << global_data[i + 2] << " " << global_data[i + 3] << "\n";
        }
        outfile.close();
        cout << "Results saved in result_parallel.txt" << endl;
    }

    MPI_Finalize();
    return 0;
}