#include <mpi.h>
#include <iostream>

using namespace std;

int main() {
    MPI_Init(NULL, NULL);  

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    int send_data, recv_data;  
    double start_time, end_time;  

    if (rank == 1) { 
        send_data = 100;  
        start_time = MPI_Wtime();  

        // неблокир
        MPI_Request send_request, recv_request;
        MPI_Isend(&send_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &send_request);  // неблокир сэнд 2
        MPI_Irecv(&recv_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &recv_request);  // неблокир ресив 2

        // ждем завершания отправки и получения
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);  
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);  

        end_time = MPI_Wtime(); 
        cout << "Process 1 received " << recv_data << " from Process 2." << endl;
        cout << "Time taken (no deadlock): " << end_time - start_time << " seconds." << endl;

    } else if (rank == 2) {
        send_data = 200;  
        start_time = MPI_Wtime();  

        // неблокир
        MPI_Request send_request, recv_request;
        MPI_Isend(&send_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &send_request);  // неблокир сэнд 1
        MPI_Irecv(&recv_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &recv_request);  // неблокир ресив 1

        // ждем завершания отправки и получения
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);  
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);

        end_time = MPI_Wtime();  
        cout << "Process 2 received " << recv_data << " from Process 1." << endl;
        cout << "Time taken (no deadlock): " << end_time - start_time << " seconds." << endl;
    }

    MPI_Finalize(); 
    return 0;
}
