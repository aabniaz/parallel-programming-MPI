//nonblocking
#include <iostream>
#include <mpi.h>
using namespace std;


int main() {
    MPI_Init(NULL, NULL);  

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size);  
    MPI_Request request_recv, request_send;  
    MPI_Status reqstat, sendstat;  
    int send, recv;  
    
    double start_time;
    if (rank == 0) {
        send = 0;  
        start_time = MPI_Wtime();  //запускаем время
    }

    for (int i = 0; i < size; i++) {
        int right = (i + 1) % size;  

        if (rank == i) {
            MPI_Isend(&send, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &request_send);  
        }
        else if (rank == right) {
            int left = (rank - 1 + size) % size; 
            MPI_Irecv(&recv, 1, MPI_INT, left, 0, MPI_COMM_WORLD, &request_recv);  
            MPI_Wait(&request_recv, &reqstat);  // ожидаем завершения приема
            send = recv + rank;  // обновляем значение
            cout << "Process " << rank << ": received " << recv << ", updated send to " << send << endl;
        }
    }

    if (rank == 0) {
        double end_time = MPI_Wtime();
        cout << "Total time: " << end_time - start_time << " seconds" << endl;
    }

    MPI_Finalize();  
    return 0;
}
