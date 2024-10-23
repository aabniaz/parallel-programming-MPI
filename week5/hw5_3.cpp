#include <mpi.h>
#include <iostream>
#include <vector>
#include <iomanip>  

using namespace std;

int main() {
    MPI_Init(NULL, NULL);  

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  

    const int rows = 5; 
    const int cols = 5; 

    double vec[cols] = {1, 2, 3, 4, 5};

    double matrix[rows][cols] = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };

    double row_result;        
    double final_result[rows];

    if (rank == 0) {  
        double start_time = MPI_Wtime(); 

        // отпр строки матрицы другим процесс
        for (int i = 1; i < min(rows, num_procs); ++i) {
            MPI_Send(matrix[i], cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        // * 1 роу (процесс 0 сам обрабатывает первую строку матрицы)
        row_result = 0;
        for (int j = 0; j < cols; ++j) {
            row_result += matrix[0][j] * vec[j];
        }
        final_result[0] = row_result;  // сохр рез * первой строки

        // рез от других проц
        for (int i = 1; i < min(rows, num_procs); ++i) {
            MPI_Recv(&final_result[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        double end_time = MPI_Wtime();  
        cout << fixed << setprecision(6);  
        cout << "parallel multiplication result: ";
        for (int i = 0; i < rows; ++i) {
            cout << final_result[i] << " ";
        }
        cout << endl;
        cout << "time for parallel multiplication: " << end_time - start_time << " seconds." << endl;

        // для сравнения
        start_time = MPI_Wtime();
        cout << "comparison result: ";
        for (int i = 0; i < rows; ++i) {
            double sum = 0;
            for (int j = 0; j < cols; ++j) {
                sum += matrix[i][j] * vec[j];
            }
            cout << sum << " ";
        }
        cout << endl;
        end_time = MPI_Wtime();  
        cout << "time for comparison multiplication: " << end_time - start_time << " seconds." << endl;

    } else if (rank < rows) {  // ост проц, которые обрабатывают строки матрицы
        // получаем строки матрицы от 0
        double recv_row[cols];
        MPI_Recv(recv_row, cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        row_result = 0;
        for (int j = 0; j < cols; ++j) {
            row_result += recv_row[j] * vec[j];
        }

        // обратно процессу 0
        MPI_Send(&row_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();  
    return 0;
}
