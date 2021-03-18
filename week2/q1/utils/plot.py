import matplotlib.pyplot as plt
import pandas as pd

if __name__ == "__main__":
    df = pd.read_csv("data/bm.txt", sep="\t", header=None)

    x = [1, 10, 100, 1000, 10000, 100000]
    plt.plot(x, df.values[:,0:6].mean(0))
    plt.fill_between(
        x, 
        df.values[:, 0:6].mean(0) - df.values[:, 0:6].std(0),
        df.values[:, 0:6].mean(0) + df.values[:, 0:6].std(0),
        alpha=0.2)
    plt.scatter(x, df.values[:,0:6].mean(0), c="C0")

    plt.grid()
    plt.title("merge sort benchmark (N=1000)")
    plt.xlabel("input size")
    plt.ylabel("time (ns)")
    plt.xscale("log")
    plt.yscale("log")
    plt.savefig("result/bm.png")
