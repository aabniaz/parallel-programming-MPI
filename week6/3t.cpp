#include <iostream>
#include <mpi.h>
#include <cmath>

using namespace std;

int main() {
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    int limit = 10000;  
    int sum = 0;  

    // 0 вычисляет сумму всех чисел кратных 5
    if (rank == 0) {
        for (int i = 0; i <= limit; ++i) {
            if (i % 5 == 0) {
                sum += i;
            }
        }
        cout << "Process 0: sum of multiples of 5 up to " << limit << " is " << sum << endl;
    }

    MPI_Request req;
    MPI_Status status;
    int flag = 0;  // флаг для MPI_Test

    double start_time_nonblocking, end_time_nonblocking;
    double start_time_blocking, end_time_blocking;

    // nonblocking (MPI_Irecv и MPI_Test) для всех  кроме 0
    if (rank == 0) {
        //  0 отправляет сумму всем остальным 
        for (int i = 1; i < size; ++i) {
            MPI_Isend(&sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &status);
        }
    } else {
        // Остальные получают сумму с исплз неблк приема
        double start_time_nonblocking = MPI_Wtime();
        int received_sum;
        MPI_Irecv(&received_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);

        // Проверяем завершение приема с помощью MPI_Test и выполняем вычисления, пока данные не получены
        while (!flag) {
            double result = sqrt(12345.6789);  // Пример задачи, которую процесс может выполнять
            MPI_Test(&req, &flag, &status);  // Проверяем, завершен ли прием данных
        }

        double end_time_nonblocking = MPI_Wtime();
        cout << "Process " << rank << ": Non-blocking Irecv time: " << (end_time_nonblocking - start_time_nonblocking) << " sec" << endl;
        cout << "Process " << rank << ": Received sum (non-blocking) = " << received_sum << endl;
    }


    // blocking (MPI_Recv) для всех  кроме 0
    if (rank == 0) {
        //  0  отправляет сумму всем остальным процессам
        for (int i = 1; i < size; ++i) {
            MPI_Send(&sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Остальные  получают сумму с исплз блок приема
        start_time_blocking = MPI_Wtime();
        int received_sum_blocking;
        MPI_Recv(&received_sum_blocking, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        end_time_blocking = MPI_Wtime();

        cout << "Process " << rank << ": Blocking Recv time: " << (end_time_blocking - start_time_blocking) << " sec" << endl;
        cout << "Process " << rank << ": Received sum (blocking) = " << received_sum_blocking << endl;
    }

    MPI_Finalize();
    return 0;
}
