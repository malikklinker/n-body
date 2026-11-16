# N-body simulation

A 2D Newtonian gravity simulator in C++23. It supports multiple bodies using pairwise gravity (G = 1) and fixed step velocity Verlet integration.

## Build and run

Requires a C++23-capable GCC or Clang compiler and GNU Make.

```sh
make -j4
./build/debug/nbody initial_conditions.txt
```

Pass a configuration file. Its first line contains the iteration count and timestep; each remaining line defines a body as `mass x y px py` (momentum, not velocity). For example:

```text
1000 0.01
1.0 0.0 0.0 0.0 0.0
0.01 0.0 -1.0 0.01 0.0
```

Iterations, timestep, and masses must be positive, all numeric values finite, and body positions distinct. Blank lines and comments are not supported. To build and run in one command:

```sh
make run RUN_ARGS="initial_conditions.txt"
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

- Edit `initial_conditions.txt` or pass another configuration file to change the bodies or run settings. The included file is a two-body example.
- States are saved before each step; the state after the last step is not included.
- Gravity calculations scale as O(N²). There is no collision handling or gravitational softening; coincident positions cause division by zero, and close encounters may require smaller timesteps.
