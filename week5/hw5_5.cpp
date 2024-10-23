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

        // взаимная блокировка deadlock:
        //  1 -> 2, а затем ожидает данные от него
        // блокируется, так как пр 2 делает то же самое.
        MPI_Send(&send_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Recv(&recv_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    } else if (rank == 2) {  
        send_data = 200;  
        start_time = MPI_Wtime();  

        // 2 -> 1, а затем ожидает данные от него
        // оба ждут друг друга и не могут продолжить
        MPI_Send(&send_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&recv_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    MPI_Finalize();  
    return 0;
}
























// #include <mpi.h>
// #include <iostream>

// using namespace std;

// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);  

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
//     MPI_Comm_size(MPI_COMM_WORLD, &size);  

//     int send_data, recv_data;  
//     double start_time, end_time;  

//     if (rank == 1) {  
//         send_data = 100; 
//         start_time = MPI_Wtime();  

//         // взаимная блокировка deadlock:
//         //  1 -> 2, а затем ожидает данные от него
//         // блокируется, так как пр 2 делает то же самое.
//         MPI_Send(&send_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
//         MPI_Recv(&recv_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//         end_time = MPI_Wtime();  
//         cout << "Process 1 received " << recv_data << " from Process 2." << endl;
//         cout << "Time taken (deadlock): " << end_time - start_time << " seconds." << endl;

//     } else if (rank == 2) {  
//         send_data = 200;  
//         start_time = MPI_Wtime();  

//         // взаимная блокировка deadlock:
//         // 2 -> 1, а затем ожидает данные от него
//         // оба ждут друг друга и не могут продолжить
//         MPI_Send(&send_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
//         MPI_Recv(&recv_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//         end_time = MPI_Wtime();  
//         cout << "Process 2 received " << recv_data << " from Process 1." << endl;
//         cout << "Time taken (deadlock): " << end_time - start_time << " seconds." << endl;
//     }

//     MPI_Finalize();  
//     return 0;
// }
