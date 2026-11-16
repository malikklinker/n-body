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

    # Use a percent scale so tiny numerical fluctuations look nearly flat.
    initial_energy = energies["energy"][0]
    energy_change = energies["energy"] - initial_energy

    if initial_energy != 0:
        energy_error = energy_change / abs(initial_energy) * 100
        ax_energy.set_ylabel("Energy change / |initial energy| (%)")
        error_units = "%"
    else:
        energy_error = energy_change
        ax_energy.set_ylabel("Energy change (absolute units; initial energy = 0)")
        error_units = "energy units"

    max_error = np.max(np.abs(energy_error))

    # Keep at least +/-1% visible; expand to avoid hiding larger errors.
    if initial_energy != 0:
        error_limit = max(1.0, max_error * 1.1)
        ax_energy.set_ylim(-error_limit, error_limit)

    ax_energy.plot(energies["time"], energy_error)
    ax_energy.axhline(0, color="gray", linewidth=0.8, linestyle="--")
    ax_energy.set_xlabel("Time")
    ax_energy.set_title(f"Energy conservation\nMax |error|: {max_error:.3g} {error_units}")
    ax_energy.ticklabel_format(axis="y", style="plain", useOffset=False)
    ax_energy.grid(alpha=0.25)

    print(f"Initial energy: {initial_energy:.12g}")
    print(f"Maximum absolute energy error: {max_error:.6g} {error_units}")

    fig.tight_layout()
    fig.savefig("image.pdf")
    plt.show()
