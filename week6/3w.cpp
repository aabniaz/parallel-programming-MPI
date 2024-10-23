#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    const int mes_size = 1000000;  
    int* mess = new int[mes_size];  
    MPI_Request req;

    // nonblocking (MPI_Isend и MPI_Wait) 
    double start_time = MPI_Wtime();  

    if (rank == 0) {
        // 0 отпр всем остальным
        for (int i = 1; i < size; ++i) {
            MPI_Isend(mess, mes_size, MPI_INT, i, 0, MPI_COMM_WORLD, &req);
        }
    } else {
        // остальные принимают сообщ
        MPI_Recv(mess, mes_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    double end_time = MPI_Wtime();  
    cout << "Non-blocking send to process " << rank << ": " << (end_time - start_time) << " sec" << std::endl;


    // blocking (MPI_Send) 
    double start_time_blocking = MPI_Wtime();  

    if (rank == 0) {
        // 0 отпр всем остальным
        for (int i = 1; i < size; ++i) {
            MPI_Send(mess, mes_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // остальные  принимают сообщение
        MPI_Recv(mess, mes_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    double end_time_blocking = MPI_Wtime();
    cout << "Blocking send to process " << rank << ": " << (end_time_blocking - start_time_blocking) << " sec" << endl;

    // освобождаем динамически выделенную память
    delete[] mess;
    MPI_Finalize();
    return 0;
}
