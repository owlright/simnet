from analysis import *
import argparse
from matplotlib import rcParams

rcParams.update(
    {
        "font.family": "serif",
        "mathtext.fontset": "stix",
        "font.serif": ["SimSun"],
        "axes.unicode_minus": False,
    }
)
ticklabel_style = {
    "fontname": "Times New Roman",
}
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
    help="which legends to plot",
    choices=["epsion", "policy"],
    type=str,
    metavar="legend_name",
    default="epsion",
)

args = parser.parse_args()
output_name = "jobrct_" + args.legendname + ".pdf"


print("-" * 10, "reading data", "-" * 10)
sheet = read_csv(args.csv, use_cached=True)
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
    df.loc[len(df.index)] = [load, policy, epsion, jct]  # pyright: ignore reportGeneralTypeIssues

jobs = df
policies = sorted(list(set([extract_str(x, "aggPolicy") for x in runs.keys()])))
epsions = sorted(list(set([extract_float(x, "epsion") for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))

fig, ax = plt.subplots(figsize=(12 / 2.54, 7 / 2.54))
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
        current_epsion = current_load[current_load["load"] == load]
        jct_mean.append(current_epsion["jct"].values[0].mean())
        print(legend, load, current_epsion["jct"].values[0].mean())

    bp = ax.bar(_pos + step * _bar_width, jct_mean, _bar_width, hatch=HATCHES[step], color=COLORS[step], ec="black")
    bps.append(bp)


plt.xticks(fontname="Times New Roman")

ax.tick_params(direction="in")
ax.set_xticks(_pos + _bar_width / 2, loads)
if args.legendname == "epsion":
    ax.legend([b[0] for b in bps], [f"K={0 if legend == 0.0 else 10}" for legend in legends], frameon=False, prop={'family': "Times New Roman"}, ncol=2, columnspacing=0.8,loc='upper right',bbox_to_anchor=(1, 1.04))
    ax.set_ylim(ymin=0, ymax=0.085)
elif args.legendname == "policy":
    ax.legend([b[0] for b in bps], ["ATP", "ATSR"], frameon=False, prop={'family': "Times New Roman"}, ncol=2,columnspacing=0.8,loc='upper right',bbox_to_anchor=(1, 1.04))
    ax.set_ylim(ymin=0, ymax=0.085)

ax.set_xlabel("负载")
ax.set_ylabel("汇聚组平均每轮完成时间/s")
ax.set_axisbelow(True)
ax.yaxis.grid(color='gray', linestyle='dashed', alpha=0.5)
ax.xaxis.grid(color='gray', linestyle='dashed', alpha=0.5)

fig.subplots_adjust(left=0.12, bottom=0.15, right=0.99, top=0.99)
# fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name, dpi=600)
print(f"output {output_name}")
# import os
# paper_path = os.path.join(os.getcwd(), f"../Documents/dissertation/figures/chapter4/{output_name}")
# fig.savefig(paper_path, dpi=600)
