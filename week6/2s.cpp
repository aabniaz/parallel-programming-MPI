#include <iostream>
#include <mpi.h>

using namespace std;

int main() {
    MPI_Init(NULL, NULL);  
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    int send, recv;  
    double start_time; 

    if (rank == 0) {
        send = 0;  
        start_time = MPI_Wtime();  
    }

    for (int i = 0; i < size; i++) {
        int right = (i + 1) % size;  

        if (rank == i) {
            MPI_Ssend(&send, 1, MPI_INT, right, 0, MPI_COMM_WORLD);
        } else if (rank == right) {
            int left = (rank - 1 + size) % size;  
            MPI_Request recv_request;
            MPI_Irecv(&recv, 1, MPI_INT, left, 0, MPI_COMM_WORLD, &recv_request);
            // ожидаем завершения приема данных
            MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
            // обновляем данные для отправки на следующем шаге
            send = recv + rank;
            cout << "Process " << rank << " received " << recv << " and updated sum to " << send << endl;
        }
    }

    if (rank == 0) {
        double end_time = MPI_Wtime();
        cout << "Total time: " << end_time - start_time << " seconds" << endl;
    }

    MPI_Finalize();  
    return 0;
}