#include <mpi.h>
#include <iostream>

int main() {
    MPI_Init(NULL, NULL);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int numrpocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numrpocs);

    printf("Hello from processor %d out of %d processors\n", rank, numrpocs);

    MPI_Finalize();

    return 0;
}