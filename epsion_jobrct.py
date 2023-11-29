import bisect
from analysis import *

output_name = "epsion_jct.png"
percentile_lowerbound = 0.00
print("-" * 10, "reading data", "-" * 10)
sheet = read_csv("simulations", "exp", "fatTreeK10LoadbalancePermutation", True)
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
epsions = sorted(list(set([extract_float(x, "epsion") for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, "load") for x in runs.keys()])))
fig, ax = plt.subplots()
_pos = np.arange(len(loads))
_bar_width = 0.2
bps = []

for step, epsion in enumerate(epsions):
    current_load = jobs[jobs["epsion"] == epsion]
    jct_mean = []
    for load in loads:
        print(epsion, load)
        current_epsion = current_load[current_load["load"] == load]
        jct_mean.append(current_epsion['jct'].values[0].mean())

    bp = ax.bar(_pos + step*_bar_width, jct_mean, _bar_width, hatch=HATCHES[step])
    bps.append(bp)

ax.set_xticks(_pos, loads)
ax.legend([b[0] for b in bps], epsions)
ax.set_xlabel(f"load")
ax.set_ylabel("job mean round completion time")
fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")