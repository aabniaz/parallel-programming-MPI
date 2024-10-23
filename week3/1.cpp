#include <iostream>
#include <vector>
using namespace std;

// func to mult 2 matr
vector<vector<int>> mult_matrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int rowsA = A.size();
    int colsA = A[0].size();
    int colsB = B[0].size();    
    vector<vector<int>> result(rowsA, vector<int>(colsB, 0));
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            for (int k = 0; k < colsA; ++k) {
                result[i][j] += A[i][k] * B[k][j];}}
    }
    return result;
}

// func to print a matrix
void print_matr(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int element : row) {
            cout << element << " ";}
        cout << endl;}
}

int main() {
    int rowsA, colsA, rowsB, colsB;
    cout << "size of the matrix A: "; cin >> rowsA >> colsA;
    cout << "size of the matrix B: "; cin >> rowsB >> colsB;
    // checking possibility
    if (colsA != rowsB) {
        cout << "multiplication is not possible! (colsA != rowsB)" << endl;
        return -1;}
    vector<vector<int>> A(rowsA, vector<int>(colsA));
    cout << "enter elements of matrix A: " << endl;
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsA; ++j) {
            cin >> A[i][j];}
    }
    vector<vector<int>> B(rowsB, vector<int>(colsB));
    cout << "enter elements of matrix B: " << endl;
    for (int i = 0; i < rowsB; ++i) {
        for (int j = 0; j < colsB; ++j) {
            cin >> B[i][j];}
    }
    vector<vector<int>> result = mult_matrices(A, B);
    cout << "result:" << endl;
    print_matr(result);
    return 0;
}