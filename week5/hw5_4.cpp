#include <mpi.h>
#include <iostream>
#include <vector>
#include <numeric> 

using namespace std;

int main() {
    MPI_Init(NULL, NULL); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    if (rank == 0) {  
        vector<int> data1 = {1, 2, 3};          
        vector<int> data2 = {4, 5, 6, 7};       
        vector<int> data3 = {8, 9, 10, 11, 12}; 

        MPI_Send(data1.data(), data1.size(), MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(data2.data(), data2.size(), MPI_INT, 2, 0, MPI_COMM_WORLD);
        MPI_Send(data3.data(), data3.size(), MPI_INT, 3, 0, MPI_COMM_WORLD);

        // MPI_ANY_SOURCE и MPI_ANY_TAG для ресив от любого процесса
        for (int i = 1; i <= 3; ++i) {
            int result;  // чтоб хранить резы
            MPI_Status status; 
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // инф о процессе-отправителе и полученного реза
            cout << "Process 0 received sum from process " << status.MPI_SOURCE << " with tag " << status.MPI_TAG << ": " << result << endl;
        }
    } else if (rank >= 1 && rank <= 3) {  
        MPI_Status status;

        // размер
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        int count;  // чтоб хранить кол элем
        MPI_Get_count(&status, MPI_INT, &count);  // кол элем

        vector<int> recv_data(count);
        MPI_Recv(recv_data.data(), count, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int sum = accumulate(recv_data.begin(), recv_data.end(), 0);

        cout << "Process " << rank << " received a message of size " << count << " and calculated the sum: " << sum << endl;

        // рез обратно -> 0 
        MPI_Send(&sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();  
    return 0;
}
