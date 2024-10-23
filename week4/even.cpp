#include <stdio.h>
#include "mpi.h"
 
int main(int argc, char* argv[]) {
 
    int ProcNum, ProcRank;
 
    MPI_Init(&argc, &argv);
 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank % 2 == 0){
        printf("Hello from process %3d", ProcRank);
    }
    MPI_Finalize();
    return 0;
}