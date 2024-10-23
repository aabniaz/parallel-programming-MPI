#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
using namespace std;

double leibniz_pi(int start, int end) {
    double pi = 0.0;
    for (int n = start; n <end; ++n) {
        double term = pow(-1, n) / (2 * n + 1);
        pi += term;
    }
    pi *= 4;
    return pi;
}
int main() {
    MPI_Init(NULL, NULL); 
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  
    int iter = 1000000;  
    int iter_process = iter / size;  
    double start_time_p = MPI_Wtime();
    int start = rank * iter_process;
    int end = (rank + 1) * iter_process;
    double local_sum = leibniz_pi(start, end);
    if (rank == 0) {
        double pi = local_sum;  
        // получаем суммы от всех остальных процессов
        for (int i = 1; i < size - 1; ++i) {
            double recv_sum;
            MPI_Recv(&recv_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pi += recv_sum;  
        }

        double end_time_p = MPI_Wtime();  

        cout << setprecision(15);
        cout << "Parallel pi = " << pi << endl;
        cout << "Time (parallel): " << (end_time_p - start_time_p) << " seconds" << endl;

        double start_time_seq = MPI_Wtime();
        double approx_pi = leibniz_pi(0, iter);
        double end_time_seq = MPI_Wtime();  
        cout << "Sequential pi = " << approx_pi << endl;
        cout << "Time (sequential): " << (end_time_seq - start_time_seq) << " seconds" << endl;

        cout << "Diff between them: " << fabs(pi - approx_pi) << endl;
    } else {
        // остальные процессы отправляют свои  суммы нулевому
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();  
    return 0;
}
