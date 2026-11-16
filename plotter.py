import matplotlib.pyplot as plt
import numpy as np


if __name__ == "__main__":
    trajectories = np.genfromtxt("trajectory.csv", delimiter=",", names=True, ndmin=1)
    energies = np.genfromtxt("energies.csv", delimiter=",", names=True, ndmin=1)

    fig, (ax_orbits, ax_energy) = plt.subplots(1, 2, figsize=(12, 5))

    # Each body's rows form one trajectory.
    for body_index in np.unique(trajectories["body_index"]):
        body = trajectories[trajectories["body_index"] == body_index]
        ax_orbits.plot(body["x"], body["y"], label=f"Body {int(body_index)}")

    ax_orbits.set_aspect("equal", adjustable="box")
    ax_orbits.set_xlabel("x")
    ax_orbits.set_ylabel("y")
    ax_orbits.set_title("Trajectories")
    ax_orbits.legend()

    # Energy is saved separately, once per timestep.
    ax_energy.plot(energies["time"], energies["energy"])
    ax_energy.set_xlabel("Time")
    ax_energy.set_ylabel("Total energy")
    ax_energy.set_title("Energy conservation")

    fig.tight_layout()
    fig.savefig("image.pdf")
    plt.show()
