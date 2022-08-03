import matplotlib.pyplot as plt
import numpy as np
import sympy


def main():  # pylint: disable=too-many-locals
    x = [-2, -4, 4]
    y = [1, 8, 4]
    s = [5, 4, 5]

    point_string = [r"$(x_1, y_1)$", r"$(x_2, y_2)$", r"$(x_3, y_3)$"]

    fig, axis = plt.subplots()
    axis.set_facecolor("darkgray")
    fig.set_facecolor("dimgray")

    angles = np.linspace(0, 2 * np.pi, 100)
    for i in range(3):
        axis.plot(
            s[i] * np.cos(angles) + x[i],
            s[i] * np.sin(angles) + y[i],
            color="blue",
            linewidth=2,
            zorder=0,
        )
        axis.scatter(x[i], y[i], color="b", marker="x", zorder=0)
        axis.text(x[i] + 0.5, y[i] + 0.5, point_string[i], fontsize=13, zorder=0)

    axis.axis("equal")
    axis.axis("off")

    x_u, y_u = sympy.symbols("x_u y_u")

    error = [0, 0, 0]

    cost = 0

    N = 3  # pylint: disable=invalid-name
    for i in range(N):
        error[i] = (x[i] - x_u) ** 2 + (y[i] - y_u) ** 2 - s[i] ** 2
        cost += (error[i] ** 2) / N

    cost_dx = sympy.diff(cost, x_u)
    cost_dy = sympy.diff(cost, y_u)

    basis = sympy.groebner([cost_dy, cost_dx], x_u, y_u)

    y_k = float(sympy.solve(basis[1], y_u)[0].evalf())
    x_k = float(sympy.solve(basis[0].subs(y_u, y_k))[0])

    axis.scatter(x_k, y_k, color="r", marker="x", s=100, linewidth=2, zorder=1)

    axis.text(
        1,
        0,
        r"$(x - x_0)^2 + (y - y_0)^2 = s^2$",
        fontsize=15,
        bbox={"facecolor": "darkgray", "edgecolor": "black"},
    )

    axis.text(
        1,
        -2,
        r"$\mathrm{MSE} \sum_{i \in [1, 3]}(s(x_i, y_i)^2 - s_i^2)^2$",
        fontsize=15,
        bbox={"facecolor": "darkgray", "edgecolor": "black"},
    )

    mse = float(cost.subs([(x_u, x_k), (y_u, y_k)]))
    rmse = np.sqrt(mse)
    print(rmse)

    plt.show()


if __name__ == "__main__":
    main()
