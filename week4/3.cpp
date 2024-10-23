#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            std::cerr << "you need to have exactly two processes." << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    const int NUM_PINGS = 5; // Number of pings to send
    int ping_count = 0;
    int message;

    if (rank == 0) {
        message = 1; 
        for (int i = 0; i < NUM_PINGS; ++i) {
            MPI_Send(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process " << rank<<"sent ping " << i + 1 << std::endl;

        }
    } else if (rank == 1) {
        for (int i = 0; i < NUM_PINGS; ++i) {
            MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive from Process 0
            std::cout << "Process 1 received ping " << i + 1 << std::endl;
          
        }
    }

    MPI_Finalize();
    return 0;
}
