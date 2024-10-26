#include <iostream>
#include <cmath>
#include <mpi.h>
#include <iomanip>

using namespace std;

double phi_1(double x) {
    return 0.0;
}

double phi_2(double x) {
    return x;
}

// f(x, y) = x**2 * sqrt(1 + x * y)
double f(double x, double y) {
    return x * x * sqrt(1 + x * y);
}

// first let's integrate by dy
double simpson_y(double x, int N) {
    double a_y = phi_1(x);  // phi_1(x) = 0
    double b_y = phi_2(x);  // phi_2(x) = x
    double h_y = (b_y - a_y) / N;

    double sum = f(x, a_y) + f(x, b_y);
    for (int i = 1; i < N; ++i) {
        double y = a_y + i * h_y;
        if (i % 2 == 0) {
            sum += 2 * f(x, y);
        } else {
            sum += 4 * f(x, y);
        }
    }
    return (h_y / 3.0) * sum;
}

// then by dx
double simpson_x(int rank, int size, int N) {
    double a_x = 1.0;
    double b_x = 2.0;
    double h_x = (b_x - a_x) / N;

    int local_n_x = N / size;  //  инт для проц
    int remainder = N % size;  

    if (rank < remainder) {
        local_n_x += 1; 
    }

    double local_a_x = a_x + rank * local_n_x * h_x;
    double local_b_x = local_a_x + local_n_x * h_x;

    double sum = simpson_y(local_a_x, N) + simpson_y(local_b_x, N);
    for (int i = 1; i < local_n_x; ++i) {
        double x = local_a_x + i * h_x;
        if (i % 2 == 0) {
            sum += 2 * simpson_y(x, N);
        } else {
            sum += 4 * simpson_y(x, N);
        }
    }
    return (h_x / 3.0) * sum;
}

double exact_solution() {
    return (50 * sqrt(5) - 8 * sqrt(2) - 15) / 15.0;
}

int main() {
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1000000;  
    double start_time = MPI_Wtime();
    double local_integral = simpson_x(rank, size, N);

    if (rank == 0) {
        double par_integ = local_integral;

        // recv results from other processes
        for (int i = 1; i < size; ++i) {
            double recv_integ;
            MPI_Recv(&recv_integ, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            par_integ += recv_integ;
        }

        double end_time = MPI_Wtime();
        cout << "N: " << N << endl;
        cout << fixed << setprecision(10);
        cout << "parallel integral: " << par_integ << endl;
        cout << "total time: " << end_time - start_time << " sec" << endl;

        double exact_value = exact_solution();
        cout << "exact integral: " << exact_value << endl;
        cout << "error: " << fabs(par_integ - exact_value) << endl;

    } else {
        // other processes send their results to 0
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
