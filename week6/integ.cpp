#include <iomanip>
#include <iostream>
#include <cmath>
#include <mpi.h>

using namespace std;

double f(double x) {
    return 2 * x * x - sqrt(x + 2);
}

double integ_simpson(double a, double h, int n_start, int n_end) {
    double sum = 0.0;
    double current_point = a + n_start * h;
    for (int i = n_start; i <= n_end; i++) {
        double temp = f(current_point);
        if (i % 2 != 0)
            sum += 4 * temp;  // odd ind
        else
            sum += 2 * temp;  // even ind
        current_point += h;
    }
    return sum;
}

int main() {
    MPI_Init(NULL, NULL); 
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  

    int n;
    double a, b;

    if (rank == 0) {
        cout << "Enter n (even): "; cin >> n;  //n = 30
        cout << "Enter range [a, b]: "; cin >> a >> b;

        if (n % 2 != 0) {
            cout << "n must be even" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Передаем a, b, и n всем процессам
        for (int i = 1; i < size; i++) {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&b, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Получаем a, b, и n от  0
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // parallel
    double h = (b - a) / n;  // Шаг сетки
    int iter_process = n / size;
    int start = rank * iter_process+1; 

    int end;
    if (rank == size - 1) {
        end = n - 1;  // Посл пр до посл точки (n-1)
    } else {
        end = start + iter_process - 1;  // Другие процессы свой диапазон
    }


    double start_time_p = MPI_Wtime();

    double local_sum = integ_simpson(a, h, start, end);

    // Собираем резы всех процессов на  0
    double integ = 0.0;
    if (rank == 0) {
        integ = f(a) + f(b);  
        integ += local_sum;
        for (int i = 1; i < size; i++) {
            double recv_sum;
            MPI_Recv(&recv_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            integ += recv_sum;
        }
        integ *= (h / 3.0);  
    } else {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    double end_time_p = MPI_Wtime();

    if (rank == 0) {
        cout << setprecision(25);
        cout << "Parallel numerical integration: " << integ << endl;
        cout << "Time (parallel): " << (end_time_p - start_time_p) << " seconds" << endl;

        // sequential
        double start_time_s = MPI_Wtime();
        double seq_sum = f(a) + f(b);
        seq_sum += integ_simpson(a, h, 1, n - 1);
        seq_sum *= (h / 3.0);
        double end_time_s = MPI_Wtime();

        cout << "Sequential numerical integration: " << seq_sum << endl;
        cout << "Time (sequential): " << (end_time_s - start_time_s) << " seconds" << endl;

        cout << "Diff between parallel and sequential: " << abs(integ - seq_sum) << endl;
        double exact = 38.2020410288673;
        cout << "exact: " << exact;
    }

    MPI_Finalize();  
    return 0;
}
