import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("result_parallel.txt")
x = data[:, 0]
y = data[:, 1]
u = data[:, 2]
v = data[:, 3]

x_unique = np.unique(x)
y_unique = np.unique(y)
X, Y = np.meshgrid(x_unique, y_unique)

U = u.reshape(len(y_unique), len(x_unique))
V = v.reshape(len(y_unique), len(x_unique))

fig, axs = plt.subplots(1, 2, figsize=(12, 6))

im1 = axs[0].pcolormesh(X, Y, U, shading='auto', cmap='viridis')
axs[0].set_title("Velocity U")
axs[0].set_xlabel("x")
axs[0].set_ylabel("y")
fig.colorbar(im1, ax=axs[0])

im2 = axs[1].pcolormesh(X, Y, V, shading='auto', cmap='viridis')
axs[1].set_title("Velocity V")
axs[1].set_xlabel("x")
axs[1].set_ylabel("y")
fig.colorbar(im2, ax=axs[1])

plt.tight_layout()
plt.show()
