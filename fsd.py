import bisect
from analysis import *
import argparse

parser = argparse.ArgumentParser(description="draw flow slowdown in each interval under various load")
parser.add_argument(
    "-c",
    "--csv",
    dest="csv",
    help="experiment configure name",
    type=str,
    metavar="csv_path",
    required=True,
)

parser.add_argument(
    "-l",
    "--legend",
    dest="legendname",
    help="legends compared to plot",
    choices=["epsion", "policy"],
    type=str,
    default="epsion",
)
parser.add_argument(
    "-p",
    "--percentile",
    dest="percentile",
    type=float,
    default=0.95,
)
parser.add_argument(
    "-t",
    "--thresholdsmall",
    dest="threshold",
    type=float,
    help="threshold(MB) to determine small flows",
    default=10,
)
args = parser.parse_args()
output_name = "fsd_" + args.legendname + ".png"
percentile_lowerbound = args.percentile
threshold = args.threshold

print("-" * 10, "reading data", "-" * 10)
sheet = read_csv(args.csv, True)
_kept_rows = ["flowSize:vector", "fct:vector", "idealFct:vector", "jobRCT:vector"]
flows = get_vectors(
    sheet,
    names=_kept_rows,
    module="FatTree",
)
runs = get_runIDs(sheet, by="iterationvars")
assert isinstance(runs, dict)


print("-" * 10, "align flow and job finish time", "-" * 10)
truncate_vectime(flows, runs)


print("-" * 10, "calc slowdown", "-" * 10)
df = get_flows_slowdown(flows, runs)


policies = sorted(list(set([extract_str(x, "aggPolicy") for x in runs.keys()])))
epsions = sorted(list(set([extract_float(x, "epsion") for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))

legends = []
if args.legendname == "epsion":
    legends = epsions
elif args.legendname == "policy":
    legends = policies

plt.rcParams["font.family"] = "Serif"
fig, ax = plt.subplots(1, len(loads), figsize=(50 / 2.54, 10 / 2.54))
_pos = np.arange(2)
_bar_width = 0.2


for col_index, load in enumerate(loads):
    current_load = df[(df["load"] == load)]
    bps = []

    for step, legend in enumerate(legends):
        print(load, legend)
        current_data = current_load[current_load[args.legendname] == legend]
        flsz = current_data.iloc[0, :]["flowsize"]
        flsz.sort()
        x = [0, bisect.bisect_left(flsz, args.threshold * 1e6) , len(flsz)]
        x95 = []
        flsd: np.ndarray = current_data["slowdown"].values[0]
        flsd_intv = []
        flsd_intv_data = []
        flct = []
        for l, r in itertools.pairwise(x):
            lb = round(l + (r - l) * percentile_lowerbound)
            x95.append((lb, r))
            if len(flsd[lb:r]) == 0:
                print_error(f"inval too small: {lb},{r}")
                exit()
            data = flsd[lb:r]
            flsd_intv.append(data.mean())
            flsd_intv_data.append(data)
            flct.append(len(data))
        print(flct)
        # bp = ax[col_index].plot(_pos, flsd_intv, color=COLORS[step], marker=MARKERS[step])

        bp = ax[col_index].bar(_pos + step * _bar_width, flsd_intv, _bar_width, color=COLORS[step], ec="black")
        bps.append(bp)
        # bp = ax[col_index].boxplot(
        #     flsd_intv_data,
        #     False,
        #     "",
        #     widths=0.4,
        #     patch_artist=True,
        #     positions=_pos + step,
        #     boxprops=dict(facecolor=COLORS[step]),
        # )

    # * xticks set only once each ax
    ax[col_index].set_xticks(_pos+_bar_width/2, [f"flowsize <= {args.threshold} MB", f"flowsize > {args.threshold} MB"])
    # ax[col_index].legend([b["boxes"][0] for b in bps], epsions)
    ax[col_index].legend([b[0] for b in bps], [f"{args.legendname}={legend}" for legend in legends])
    ax[col_index].set_xlabel(f"Load={load}")
ax[0].set_ylabel("FCT slow down")
fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")
