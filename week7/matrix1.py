# also i just wanted to compare my mult matrices and result matr with 
# exact solution by using pythons library and give true/false

import numpy as np

def read_matr(filename):
    with open(filename, 'r') as file:
        matrix = [list(map(int, line.split())) for line in file]
    return np.array(matrix)

def save_matr(matrix, filename):
    with open(filename, 'w') as file:
        for row in matrix:
            file.write(' '.join(map(str, row)) + '\n')

def compare_matr(matrix1, matrix2):
    return np.array_equal(matrix1, matrix2)

matrix_A = read_matr('matrix_A_5.txt')
matrix_B = read_matr('matrix_B_5.txt')

result_matrix = np.dot(matrix_A, matrix_B)
save_matr(result_matrix, 'matrix_C_5_python.txt')
matrix_C = read_matr('matrix_C_5.txt')
is_equal = compare_matr(result_matrix, matrix_C)
print("Are the matrices equal? ", is_equal)