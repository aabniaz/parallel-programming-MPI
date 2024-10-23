#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
using namespace std;

unsigned long long factorial(int n) {
    unsigned long long factor = 1;
    for (int i = 1; i <= n; i++) {
        factor *= i;
    }
    return factor;
}

double exp_taylor_partial(double x, int start, int end) {
    double sum = 0.0;
    for (int n = start; n < end; n++) {
        sum += pow(x, n) / factorial(n);
    }
    return sum;
}

int main() {
    MPI_Init(NULL, NULL);  
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  

    double x;
    int Nterms = 30;

    if (rank == 0) {
        cout << "Enter the value of x for e^x: ";
        cin >> x;

        // Передаем x и Nterms всем процессам
        for (int i = 1; i < size; i++) {
            MPI_Send(&x, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&Nterms, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Получаем  x и Nterms от 0
        MPI_Recv(&x, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&Nterms, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // parallel
    int terms_process = Nterms / size;
    int start = rank * terms_process;
    int end = (rank == size - 1) ? Nterms : start + terms_process;

    double start_time_p = MPI_Wtime();

    double local_sum = exp_taylor_partial(x, start, end);

    double exp = 0.0;
    if (rank == 0) {
        exp = local_sum;
        // Получаем резы от всех остальных процессов
        for (int i = 1; i < size; i++) {
            double recv_sum;
            MPI_Recv(&recv_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            exp += recv_sum;
        }
    } else {
        // Отправляем рез нулевому
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    double end_time_p = MPI_Wtime(); 

    if (rank == 0) {
        cout << setprecision(15);
        cout << "Parallel exp = " << exp << endl;
        cout << "Time (parallel): " << (end_time_p - start_time_p) << " seconds" << endl;

        // sequential
        double start_time_s = MPI_Wtime();

        double seqtl_exp = exp_taylor_partial(x, 0, Nterms);

        double end_time_s = MPI_Wtime();  

        cout << "Sequential exp = " << seqtl_exp << endl;
        cout << "Time (sequential): " << (end_time_s - start_time_s) << " seconds" << endl;

        double diff = fabs(exp - seqtl_exp);
        cout << "Diff between them: " << diff << endl;
    }

    MPI_Finalize(); 
    return 0;
}
