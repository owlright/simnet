import bisect
from analysis import *
import argparse

parser = argparse.ArgumentParser(description="draw flow slowdown in each interval under various load")
parser.add_argument(
    "-c",
    "--config",
    dest="config",
    help="experiment configure name",
    type=str,
    metavar="config_name",
    default="fatTreePolicy"
)

parser.add_argument(
    "-l",
    "--legend",
    dest="legendname",
    help="which legends to plot",
    choices = ["epsion", "policy"],
    type=str,
    metavar="legend_name",
    default="epsion"
)

parser.add_argument(
    "-p",
    "--percentile",
    dest="percentile",
    type=float,
    metavar="percentile",
    default=0.95
)

args = parser.parse_args()
output_name = "jobrct_" + args.legendname + ".png"
percentile_lowerbound = args.percentile


print("-" * 10, "reading data", "-" * 10)
sheet = read_csv("simulations", "exp", args.config, True)
_kept_rows = ["jobRCT:vector"]
jobs = get_vectors(
    sheet,
    names=_kept_rows,
    module="FatTree",
)
runs = get_runIDs(sheet, by="iterationvars")
assert isinstance(runs, dict)
# * a little hack to transpose the columns
jobs = jobs.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
df = pd.DataFrame(columns=["load", "policy", "epsion", "jct"])

for itervar, repetition_ids in runs.items():
    policy, load, epsion = extract_iterationvar(itervar)
    print(load, policy, epsion)
    jobreps = jobs.loc[repetition_ids]
    jct = np.concatenate(jobreps["jobRCT:vector"].values)
    df.loc[len(df.index)] = [load, policy, epsion, jct]   # pyright: ignore reportGeneralTypeIssues

jobs = df
policies = sorted(list(set([extract_str(x, 'aggPolicy') for x in runs.keys()])))
epsions = sorted(list(set([extract_float(x, "epsion") for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))
fig, ax = plt.subplots()
_pos = np.arange(len(loads))
_bar_width = 0.2
bps = []
legends = []
if args.legendname == "epsion":
    legends = epsions
elif args.legendname == "policy":
    legends = policies

for step, legend in enumerate(legends):
    current_load = jobs[jobs[args.legendname] == legend]
    jct_mean = []
    for load in loads:
        print(legend, load)
        current_epsion = current_load[current_load["load"] == load]
        jct_mean.append(current_epsion['jct'].values[0].mean())

    bp = ax.bar(_pos + step*_bar_width, jct_mean, _bar_width, hatch=HATCHES[step],  color=COLORS[step], ec="black")
    bps.append(bp)

ax.set_xticks(_pos+_bar_width/2, loads)
ax.legend([b[0] for b in bps], [f'{args.legendname}={legend}' for legend in legends])
ax.set_xlabel(f"Load")
ax.set_ylabel("job mean round completion time")
# fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
# fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")