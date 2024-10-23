#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    int message = rank;
    int received_message;

    int next = (rank + 1) % numprocs;
    int prev = (rank - 1 + numprocs) % numprocs;

    if (rank == 0) {
        MPI_Send(&message, 1, MPI_INT, next, 0, 
                    MPI_COMM_WORLD);
        std::cout << "Process " << rank << ": Sent message " 
                  << message << " to process " << next << std::endl;

        MPI_Recv(&received_message, 1, MPI_INT, prev, 0, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << ": Received message " 
                  << received_message << " from process " << prev << std::endl;
    } else {
        // All other processor firstly will wait to getting message from previous processor
        MPI_Recv(&received_message, 1, MPI_INT, prev, 0, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << ": Received message " 
                  << received_message << " from process " << prev << std::endl;

        // And then they will send message to the next processor
        MPI_Send(&message, 1, MPI_INT, next, 0,
                    MPI_COMM_WORLD);
        std::cout << "Process " << rank << ": Sent message " << message
                  << " to process " << next << std::endl;
        // This order of receiving and sending is cruicial for the ring
        // and for the right order of sending/receiving
    }

    MPI_Finalize();
    return 0;
}