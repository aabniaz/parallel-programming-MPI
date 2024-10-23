#include <iostream>
using namespace std;

int main() {
    int rows_A, columns_A, rows_B, columns_B;
    cout << "size of the matrix A: "; cin >> rows_A >> columns_A;
    cout << "size of the matrix B: "; cin >> rows_B >> columns_B;
    // checking possibility
    if (columns_A != rows_B) {
        cout << "multiplication is not possible! (columns_A != rows_B)" << endl;
        return -1;}
    int A[rows_A][columns_A], B[rows_B][columns_B], result[rows_A][columns_B];
    // fill with zeros
    for (int i = 0; i < rows_A; ++i) {
        for (int j = 0; j < columns_B; ++j) {
            result[i][j] = 0;}
    }
    cout << "enter elements of matrix A:" << endl;
    for (int i = 0; i < rows_A; ++i) {
        for (int j = 0; j < columns_A; ++j) {
            cin >> A[i][j];}
    }
    cout << "enter elements of matrix B:" << endl;
    for (int i = 0; i < rows_B; ++i) {
        for (int j = 0; j < columns_B; ++j) {
            cin >> B[i][j];}
    }
    for (int i = 0; i < rows_A; ++i) {
        for (int j = 0; j < columns_B; ++j) {
            for (int k = 0; k < columns_A; ++k) {
                result[i][j] += A[i][k] * B[k][j];}}
    }
    cout << "result" << endl;
    for (int i = 0; i < rows_A; ++i) {
        for (int j = 0; j < columns_B; ++j) {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}