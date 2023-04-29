import sys
import os
import shutil
import pandas as pd
import matplotlib.pyplot as plt
import seaborn


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

    count = 0
    infile = open(filename)
    outfile = None

    for line in infile.read().splitlines():
        if line == EXPECTED_CSV_HEADER:
            if outfile:
                outfile.close()
                outfile = None
            outpath = os.path.join(outdir, f"chunk-{count}.csv")
            print(f"Writing to {outpath}")
            outfile = open(outpath, "w")
            count += 1
        outfile.write(f"{line}\n")
    if outfile:
        outfile.close()

    for f in os.listdir(outdir):
        fn = os.path.join(outdir, f)
        df = pd.read_csv(fn)
        if df.empty:
            print(f"Empty: {fn}")
            continue
        df["dt"] = df.t.diff()
        df.t /= 1000
        seaborn.lineplot(data=df, x="t", y="p")
        seaborn.lineplot(data=df, x="t", y="dt")
    plt.grid()
    plt.show()


if __name__ == "__main__":
    main()
