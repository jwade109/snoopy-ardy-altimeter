import sys
import os
import shutil
import pandas as pd
import matplotlib.pyplot as plt


EXPECTED_CSV_HEADER = "n,t,p"


def main():

    if len(sys.argv) < 3:
        print(f"usage: {sys.argv[0]} [logfile.txt] [split/] [--force]")
        return 1

    filename = sys.argv[1]
    outdir = sys.argv[2]
    force = len(sys.argv) > 3 and sys.argv[3] == "--force"

    if os.path.exists(outdir) and not force:
        print("Output directory already exists. Use --force to overwrite.")
        return 1
    elif os.path.exists(outdir):
        shutil.rmtree(outdir)

    os.mkdir(outdir)

    index = 0
    infile = open(filename)
    outfile = None

    for line in infile.read().splitlines():
        if line == EXPECTED_CSV_HEADER:
            outpath = os.path.join(outdir, f"chunk-{index}.csv")
            print(f"Writing to {outpath}")
            outfile = open(outpath, "w")
            index += 1
        outfile.write(f"{line}\n")

    for f in os.listdir(outdir):
        fn = os.path.join(outdir, f)
        df = pd.read_csv(fn)
        duration = (df.t.max() - df.t.min()) / 1000
        df["dt"] = df.t.diff()
        print(df.head())
        df.plot(x="t", y="dt", title=f"[{fn}] Delta Time ({duration:0.1f} s)")
        plt.grid()
        df.plot(x="t", y="p", title=f"[{fn}] Pressure ({duration:0.1f} s)")
        plt.grid()
    plt.show()


if __name__ == "__main__":
    main()
