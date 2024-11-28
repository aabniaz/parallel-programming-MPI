import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("parallel_solution.txt")

x = data[:, 0]
y = data[:, 1]
u = data[:, 2]

x_unique = np.unique(x)
y_unique = np.unique(y)

X, Y = np.meshgrid(x_unique, y_unique)
U = u.reshape(len(y_unique), len(x_unique))

plt.figure(figsize=(8, 6))
plt.contourf(X, Y, U, 20, cmap="viridis")  
plt.colorbar(label="u")                    
plt.title("parallel")
plt.xlabel("x")
plt.ylabel("y")
plt.grid(True, linestyle='--', alpha=0.5)
plt.show()
