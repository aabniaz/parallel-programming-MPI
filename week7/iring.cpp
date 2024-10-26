#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    MPI_Init(NULL, NULL);
    int rank, size, left, right;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Request req_recv, req_send;
    MPI_Status req_stat;
    int send_mes = 0;
    int recv;

    double start_time = MPI_Wtime();
    left = (rank - 1 + size) % size;
    right = (rank + 1) % size;

    // 0 proc starts the process
    if (rank == 0) {
        MPI_Isend(&send_mes, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req_send);
        MPI_Wait(&req_send, MPI_STATUS_IGNORE);
        cout << "process " << rank << " sent " << send_mes << " to process " << right << endl;
        MPI_Recv(&recv, 1, MPI_INT, left, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        cout << "process " << rank << " received " << recv << " from prcess " << left << endl;
        double end_time = MPI_Wtime();
        cout << "total time: " << end_time - start_time << " sec" << endl;
    } 
    else {
        // all other ranks first receive, then send_mes
        MPI_Recv(&recv, 1, MPI_INT, left, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        send_mes = recv + rank;
        MPI_Isend(&send_mes, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req_send);
        MPI_Wait(&req_send, MPI_STATUS_IGNORE);
        cout << "process " << rank << " sent " << send_mes << " to process " << right << endl;
    }

    MPI_Finalize();
    return 0;
}