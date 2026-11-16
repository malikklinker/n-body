"""Plot trajectories from the simulator's time,body_index,x,y CSV output."""

import argparse
import csv
import math
from pathlib import Path


def read_trajectories(path):
    trajectories = {}

    with path.open(newline="") as source:
        reader = csv.DictReader(source)
        required = {"time", "body_index", "x", "y"}
        if not required.issubset(reader.fieldnames or []):
            raise ValueError("CSV must contain time, body_index, x, and y columns")

        for row in reader:
            try:
                body_index = int(row["body_index"])
                time, x, y = (float(row[column]) for column in ("time", "x", "y"))
                if body_index < 0 or not all(math.isfinite(value) for value in (time, x, y)):
                    raise ValueError("expected a nonnegative body index and finite coordinates/time")
            except (ValueError, TypeError) as error:
                raise ValueError(f"Invalid CSV data on line {reader.line_num}: {error}") from error

            trajectories.setdefault(body_index, []).append((time, x, y))

    if not trajectories:
        raise ValueError("CSV contains no trajectory data")

    for samples in trajectories.values():
        samples.sort(key=lambda sample: sample[0])

    return trajectories


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", nargs="?", type=Path, default=Path("trajectory.csv"))
    parser.add_argument("-o", "--output", type=Path, default=Path("image.pdf"))
    parser.add_argument("--no-show", action="store_true", help="save without opening a plot window")
    args = parser.parse_args()

    if args.input.resolve() == args.output.resolve():
        parser.error("input and output paths must differ")

    try:
        trajectories = read_trajectories(args.input)
    except (OSError, ValueError, csv.Error) as error:
        parser.exit(1, f"Error reading {args.input}: {error}\n")

    try:
        import matplotlib

        if args.no_show:
            matplotlib.use("Agg")

        import matplotlib.pyplot as plt
    except ImportError:
        parser.exit(1, "Plotting requires matplotlib; install it in your Python environment.\n")

    fig, ax = plt.subplots()

    for body_index, samples in sorted(trajectories.items()):
        _, x, y = zip(*samples)
        ax.plot(x, y, linewidth=2, label=f"Body {body_index}")
        ax.plot(x[0], y[0], ".", color=ax.lines[-1].get_color())

    ax.set_aspect("equal", adjustable="box")
    ax.set_xlabel("x", fontsize="large")
    ax.set_ylabel("y", fontsize="large")
    ax.set_title("Body trajectories (dots mark initial positions)")
    ax.legend()
    fig.tight_layout()

    try:
        fig.savefig(args.output)
    except (OSError, ValueError) as error:
        plt.close(fig)
        parser.exit(1, f"Error saving {args.output}: {error}\n")

    print(f"Saved {args.output}")

    if not args.no_show:
        plt.show()

    plt.close(fig)


if __name__ == "__main__":
    main()
