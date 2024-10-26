#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_N 50

void block_matrix_mult(int A[MAX_N][MAX_N], int B[MAX_N][MAX_N], int C[MAX_N][MAX_N], int local_rows, int N) {
    for (int i = 0; i < local_rows; i++) { 
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int rank, size, N;
    int sendcounts[MAX_N], displs[MAX_N]; 
    int local_rows, remainder;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    N = MAX_N;

    // num rows that each process will handle
    remainder = N % size;
    for (int i = 0; i < size; i++) {
        sendcounts[i] =  N * N / size; 
        if (i < remainder) {
            sendcounts[i] += N;  // +row rem
        }
        displs[i] = (i == 0) ? 0 : displs[i - 1] + sendcounts[i - 1];
    }

    // local_rows for curr proc
    local_rows = sendcounts[rank] / N;

    int A[MAX_N][MAX_N] = {0};
    int B[MAX_N][MAX_N] = {0};
    int C[MAX_N][MAX_N] = {0};  
    int local_A[MAX_N][MAX_N] = {0};  
    int local_C[MAX_N][MAX_N] = {0};  

    if (rank == 0) {
        // we fill by rand int and write A and B to the txt
        FILE* fileA = fopen("matrix_A_5.txt", "w");
        FILE* fileB = fopen("matrix_B_5.txt", "w");
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10; // fill A 
                B[i][j] = rand() % 10; // fill B
                fprintf(fileA, "%d ", A[i][j]);
                fprintf(fileB, "%d ", B[i][j]);
            }
            fprintf(fileA, "\n");
            fprintf(fileB, "\n");
        }
        fclose(fileA);
        fclose(fileB);
    }
    
    // sending blocks of matrix A -> to all processes (used v, if the matr size is not divisible by the num of proc)
    MPI_Scatterv(A, sendcounts, displs, MPI_INT, local_A, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    // bcast the entire matrix B -> to all processes
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    // * the local blocks
    block_matrix_mult(local_A, B, local_C, local_rows, N);
    // gather the results -> back to matrix C
    MPI_Gatherv(local_C, local_rows * N, MPI_INT, C, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    // then saving the result in a file on rank 0
    if (rank == 0) {
        FILE* fileC = fopen("matrix_C_5.txt", "w");
        if (fileC == NULL) {
            printf("Error opening file for writing\n");
            return -1;
        }
        //  result matrix
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(fileC, "%d ", C[i][j]);
            }
            fprintf(fileC, "\n");
        }
        fclose(fileC);
        printf("Matrix C saved to matrix_C.txt\n");
    }
    
    MPI_Finalize();
    return 0;
}
