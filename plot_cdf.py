import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import itertools as itt

dist = "./src/distribution/data/FbHdp_distribution.txt"
df = pd.read_csv(dist, header=None, names=["flowsize", "percentile"], sep=" ")
flowsizes:list[int] = df["flowsize"].values.tolist()
percent_cdf:list[float] = (df["percentile"].values / 100).tolist() # type: ignore
bytes_cdf:list[float] = [0]

index = 1
total_bytes = 0
for left, right in itt.pairwise(flowsizes):
    # print(left, right, percent_cdf[index])
    total_bytes += ((left + right) / 2) * percent_cdf[index]
    index += 1

index = 1
accumlated_bytes = 0
for left, right in itt.pairwise(flowsizes):
    accumlated_bytes += ((left + right) / 2) * percent_cdf[index]
    bytes_cdf.append(accumlated_bytes/total_bytes)
    index += 1

plt.rcParams["font.family"] = "Serif"
fig, ax = plt.subplots(figsize=(15 / 2.54, 6 / 2.54))
fig.subplots_adjust(left=0.10, bottom=0.2, right=0.99, top=0.99)
ax.plot(flowsizes, percent_cdf, marker='o', color="black", markersize=5, label="Flow Size")
ax.plot(flowsizes, bytes_cdf, marker = 'x', color="blue", label="Total Bytes")
ax.set_xscale('log')
ax.set_xlabel('Flow Size(Bytes)')
ax.set_ylabel('CDF')

ax.tick_params(top=True, right=True, direction="in")
legend = ax.legend(frameon=True)
legend.get_frame().set_edgecolor("black")
legend.get_frame().set_linewidth(1)
legend.get_frame().set_boxstyle("square", pad=0)
plt.savefig("dist.pdf",dpi=600)
