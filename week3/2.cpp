// #include <iostream>
// #include <vector>
// using namespace std;

// // func to mult a matrix by a vector
// vector<int> mult_matr_vect(const vector<vector<int>>& matrix, const vector<int>& vec) {
//     int rows = matrix.size();
//     int cols = matrix[0].size();
//     vector<int> result(rows, 0);
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             result[i] += matrix[i][j] * vec[j];}
//     }
//     return result;
// }

// // func to print a vector
// void print_vect(const vector<int>& vec) {
//     for (int element : vec) {
//         cout << element << " ";}
//     cout << endl;
// }

// int main() {
//     int rows, cols;
//     cout << "size of the matrix: "; cin >> rows >> cols;
//     int vecSize;
//     cout << "size of the vector: "; cin >> vecSize;
//     // checking possibility
//     if (cols != vecSize) {
//         cout << "multiplication is not possible! (cols != vecSize)" << endl;
//         return -1;}
//     vector<vector<int>> matrix(rows, vector<int>(cols));
//     cout << "enter elements of matrix: " << endl;
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             cin >> matrix[i][j];}
//         }
//     vector<int> vec(vecSize);
//     cout << "enter elements of vector:" << endl;
//     for (int i = 0; i < vecSize; ++i) {
//         cin >> vec[i];
//     }
//     vector<int> result = mult_matr_vect(matrix, vec);
//     cout << "result: " << endl;
//     print_vect(result);
//     return 0;
// }




#include <iostream>
#include <vector>
using namespace std;

// func to mult a matrix by a vector
vector<int> mult_matrix_vector(const vector<vector<int>>& A, const vector<int>& vec) {
    int rowsA = A.size();
    int colsA = A[0].size();
    vector<int> result(rowsA, 0);    
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsA; ++j) {
            result[i] += A[i][j] * vec[j];}
    }
    return result;
}

// func to print a vector
void print_vector(const vector<int>& vec) {
    for (int element : vec) {
        cout << element << endl; 
    }
}

int main() {
    int rowsA, colsA, vecSize;
    cout << "size of the matrix: "; cin >> rowsA >> colsA;
    cout << "size of the vector: "; cin >> vecSize;
     if (colsA != vecSize) {
        cout << "multiplication is not possible! (cols != vecSize)" << endl;
        return -1;
    }
    vector<vector<int>> A(rowsA, vector<int>(colsA));
    cout << "enter elements of matrix: " << endl;
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsA; ++j) {
            cin >> A[i][j];}
    }
    vector<int> vec(vecSize);
    cout << "enter elements of vector:" << endl;
    for (int i = 0; i < vecSize; ++i) {
        cin >> vec[i];
    }

    vector<int> result = mult_matrix_vector(A, vec);
    cout << "result: " << endl;
    print_vector(result);
    return 0;
}
