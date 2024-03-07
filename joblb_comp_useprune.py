from analysis import *
import argparse
from matplotlib import rcParams
from pprint import pprint

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
    choices=["useprune", "K"],
    type=str,
    metavar="legend_name",
    default="epsion",
)

args = parser.parse_args()



print("-" * 10, "reading data", "-" * 10)
sheet = read_csv(args.csv, use_cached=True)
_kept_rows = ["jobRCT:vector"]
jobs = get_vectors(
    sheet,
    names=_kept_rows,
    module="FatTree",
)

runs = get_runIDs(sheet, by="iterationvars")
# pprint(runs)
assert isinstance(runs, dict)

# * a little hack to transpose the columns
jobs = jobs.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
df = pd.DataFrame(columns=["load", "optprune", "jct"])


for itervar, repetition_ids in runs.items():

    def _extract_iterationvar(iterationvar: str):
        load = extract_float(iterationvar, "load")
        useprune = extract_int(iterationvar, "optprune")
        return load, useprune

    load, useprune = _extract_iterationvar(itervar)
    print(load, useprune)
    jobreps = jobs.loc[repetition_ids]
    jct = np.concatenate(jobreps["jobRCT:vector"].values)
    df.loc[len(df.index)] = [load, useprune, jct]  # pyright: ignore reportGeneralTypeIssues

jobs = df
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))
useprunes = sorted(list(set([extract_int(x, "optprune") for x in runs.keys()])))


fig, ax = plt.subplots(figsize=(12 / 2.54, 7 / 2.54))
_pos = np.arange(len(loads))
_bar_width = 0.2
bps = []
# if args.legendname == "useprune":
#     legends = useprunes
# elif args.legendname == "K":
#     legends =
compare_parm = useprunes

for step, param in enumerate(compare_parm):
    current_load = jobs[jobs["optprune"] == param]
    jct_mean = []
    for load in loads:
        x = current_load[current_load["load"] == load]
        jct_mean.append(x["jct"].values[0].mean())
        print(param, load, x["jct"].values[0].mean())

    bp = ax.bar(_pos + step * _bar_width, jct_mean, _bar_width, hatch=HATCHES[step], color=COLORS[step], ec="black")
    bps.append(bp)


plt.xticks(fontname="Times New Roman")

ax.tick_params(direction="in")
ax.set_xticks(_pos + _bar_width / 2, loads)

ax.legend(
    [b[0] for b in bps],
    ["不使用改进策略","使用改进策略"],
    # [f"useprune={legend}" for legend in compare_parm],
    frameon=False,
    prop={"family": "SimSun"},
    ncol=2,
    columnspacing=0.8,
    loc="upper right",
    bbox_to_anchor=(1, 1.04),
)
ax.set_ylim(ymin=0, ymax=0.035)

ax.set_xlabel("负载")
ax.set_ylabel("汇聚组平均每轮完成时间/s")
ax.set_axisbelow(True)
ax.yaxis.grid(color="gray", linestyle="dashed", alpha=0.5)
ax.xaxis.grid(color="gray", linestyle="dashed", alpha=0.5)

fig.subplots_adjust(left=0.12, bottom=0.15, right=0.99, top=0.98)
# fig.subplots_adjust(wspace=0.1)

output_name = "jobrct_prune.pdf"
fig.savefig(output_name, dpi=600)
print(f"output {output_name}")
import os

paper_path = os.path.join(os.getcwd(), f"./{output_name}")
fig.savefig(paper_path, dpi=600)
