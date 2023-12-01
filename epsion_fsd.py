import bisect
from analysis import *
import argparse

parser = argparse.ArgumentParser(description="draw flow slowdown with various load")
parser.add_argument(
    "-c",
    "--config",
    dest="config",
    help="experiment configure name",
    type=str,
    metavar="config_name",
    required=True,
)
parser.add_argument(
    "-o",
    "--output",
    dest="output_name",
    help="output file name",
    type=str,
    metavar="output_name",
    default="epsion_fsd.png",
)
args = parser.parse_args()
output_name = args.output_name
print("-" * 10, "reading data", "-" * 10)
sheet = read_csv("simulations", "exp", args.config, False)
_kept_rows = ["flowSize:vector", "fct:vector", "idealFct:vector", "jobRCT:vector"]
flows = get_vectors(
    sheet,
    names=_kept_rows,
    module="FatTree",
)
runs = get_runIDs(sheet, by="iterationvars")
assert isinstance(runs, dict)
truncate_vectime(flows, runs)

print("-" * 10, "calc slowdown", "-" * 10)
df = get_flows_slowdown(flows, runs)

epsions = sorted(list(set([extract_float(x, "epsion") for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))
fig, ax = plt.subplots()
_pos = np.arange(len(loads))
_bar_width = 0.2
bps = []

for step, epsion in enumerate(epsions):
    current_load = df[df["epsion"] == epsion]
    fsd_mean = []
    for load in loads:
        print(epsion, load)
        current_epsion = current_load[current_load["load"] == load]
        fsd_mean.append(current_epsion["slowdown"].values[0].mean())

    bp = ax.bar(_pos + step * _bar_width, fsd_mean, _bar_width, hatch=HATCHES[step])
    bps.append(bp)

ax.set_xticks(_pos, loads)
ax.legend([b[0] for b in bps], epsions)
ax.set_xlabel(f"load")
ax.set_ylabel("mean flow slowdown")
fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")