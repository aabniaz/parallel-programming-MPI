#include <mpi.h>
#include <iostream>

using namespace std;

int main() {
    int rank, size;
    MPI_Init(NULL, NULL);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    int DATA_SIZE = 100;        // размер массива данных
    int data[DATA_SIZE];       // массив который будет отправлен
    int ack = 1;               // для подтверждения
    MPI_Status status;
    double start_time, end_time;

    if (rank == 0) { 
        // иниц массивы днными
        for (int i = 0; i < DATA_SIZE; ++i) {
            data[i] = i;
        }

        MPI_Send(data, DATA_SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
        cout << "process 0 sent message using MPI_Send to process 1\n";

        MPI_Ssend(data, DATA_SIZE, MPI_INT, 2, 0, MPI_COMM_WORLD);
        cout << "process 0 sent message using MPI_Ssend to process 2\n";

        // создаем буффер для буфф отправки
        int buffer_size;
        MPI_Pack_size(DATA_SIZE, MPI_INT, MPI_COMM_WORLD, &buffer_size);
        buffer_size += MPI_BSEND_OVERHEAD;
        char *buffer = new char[buffer_size];
        MPI_Buffer_attach(buffer, buffer_size);

        MPI_Bsend(data, DATA_SIZE, MPI_INT, 3, 0, MPI_COMM_WORLD);
        cout << "process 0 sent message using MPI_Bsend to process 3\n";

        MPI_Rsend(data, DATA_SIZE, MPI_INT, 4, 0, MPI_COMM_WORLD);
        cout << "process 0 sent message using MPI_Rsend to process 4\n";

        // получаем подтверждение от 4х проц
        for (int i = 1; i <= 4; ++i) {
            MPI_Recv(&ack, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            if (ack != 1) {
                cout << "process " << i << " failed to receive the data.\n";
            }
        }

        // освобождаем буффер
        MPI_Buffer_detach(&buffer, &buffer_size);
        delete[] buffer;
    } else if (rank >= 1 && rank <= 4) { 
        start_time = MPI_Wtime(); 
        MPI_Recv(data, DATA_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);  // получаем данные от 0
        end_time = MPI_Wtime();  
        cout << "process " << rank << " received message in " << end_time - start_time << " seconds.\n";
        MPI_Send(&ack, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); 
    return 0;
}
