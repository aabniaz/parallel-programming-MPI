import numpy as np
import matplotlib.pyplot as plt

def read_and_process_data(file_name):
    cleaned_data = []
    with open(file_name, "r") as infile:
        for line in infile:
            parts = line.strip().split()
            if len(parts) == 3:  
                try:
                    cleaned_data.append([float(p) for p in parts])
                except ValueError:
                    continue  

    return np.array(cleaned_data)

data = read_and_process_data("result_parallel.txt")
if data.size == 0:
    raise ValueError("result_parallel.txt empty or wrong data.")

x = data[:, 0]
y = data[:, 1]
u = data[:, 2]

unique_x = np.unique(x)
unique_y = np.unique(y)
X, Y = np.meshgrid(unique_x, unique_y)

U = np.full_like(X, np.nan, dtype=np.float64)
for xi, yi, ui in zip(x, y, u):
    i = np.where(unique_x == xi)[0][0]
    j = np.where(unique_y == yi)[0][0]
    U[j, i] = ui

U = np.nan_to_num(U, nan=np.nanmean(u))

plt.figure(figsize=(8, 6))
contour = plt.contourf(X, Y, U, cmap='coolwarm', levels=20)

plt.colorbar(contour)

plt.xlabel("x")
plt.ylabel("y")
plt.title("parallel laplace")

plt.show()
