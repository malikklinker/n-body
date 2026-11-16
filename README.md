# N-body simulation

A 2D Newtonian gravity simulator in C++23. It supports multiple bodies using pairwise gravity (G = 1) and fixed step velocity Verlet integration.

## Build and run

Requires a C++23-capable GCC or Clang compiler and GNU Make.

```sh
make -j4
./build/debug/nbody 1000 0.01
```

Arguments are the number of iterations and timestep, both positive. To build and run in one command:

```sh
make run RUN_ARGS="1000 0.01"
```

Use `make release` for an optimized build, `make sanitize` for a sanitizer build, and `make clean` to remove build files. Executables are stored in `build/<mode>/`.

## Plotting

The simulator writes `trajectory.csv` (`time,body_index,x,y`) and `energies.csv` (`time,energy`) in the working directory, overwriting previous output.

With Python, NumPy, and Matplotlib installed:

```sh
python3 plotter.py
```

This reads both CSV files, plots trajectories and total energy side by side, saves `image.pdf`, and opens the plot. To run without a graphical display:

```sh
MPLBACKEND=Agg python3 plotter.py
```

## Notes

- Change the `bodies` initializer in `main.cpp` to set masses, positions, and momenta. The default is a two-body example.
- States are saved before each step; the state after the last step is not included.
- Gravity calculations scale as O(N²). There is no collision handling or gravitational softening; coincident positions cause division by zero, and close encounters may require smaller timesteps.
