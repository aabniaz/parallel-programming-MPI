#include <mpi.h>
#include <iostream>
using namespace std; 

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            cout << "you need to have exactly two processes." << endl;}
        MPI_Finalize();
        return 1;}

    int NUM_PINGS = 10;
    int i = 0;
    int next_process = (rank + 1) % 2; 

    while (i < NUM_PINGS) {
        if (rank == i % 2) { 
            i++; 
            cout << "Process " << rank << " sent ping " << i << " to Process " << next_process << endl;
            MPI_Send(&i, 1, MPI_INT, next_process, 0, MPI_COMM_WORLD);
            // cout << "Process " << rank << " sent ping " << i << " to Process " << next_process << endl;
        } else {
            MPI_Recv(&i, 1, MPI_INT, next_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "Process " << rank << " received pong " << i << " from Process " << next_process << endl;

        }
    }

    MPI_Finalize();
    return 0;
}