import bisect
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

def _get_flows_slowdown(flows: pd.DataFrame, runs: dict):
    # * a little hack to transpose the columns
    flows_vec = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows_vec["slowdown"] = flows_vec.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    df = pd.DataFrame(columns=["numGroups", "numECTrees" , "flowsize", "slowdown"])
    assert isinstance(runs, dict)
    for itervar, repetition_ids in runs.items():
        nG = extract_int(itervar, "numGroups")
        K = extract_int(itervar, "numECTrees")
        print(nG, K)
        flowreps = flows_vec.loc[repetition_ids]
        flowsize = np.concatenate(flowreps["flowSize:vector"].values)
        fsd = np.concatenate(flowreps["slowdown"].values)
        df.loc[len(df.index)] = [nG, K, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues
    return df

output_name = "loadbalanceKfsd.pdf"
percentile_lowerbound = .95

print("-" * 10, "reading data", "-" * 10)
sheet = read_csv("./results/LoadBalanceK/LoadBalanceK.csv", True)
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
df = _get_flows_slowdown(flows, runs)

loads = sorted(list(set([extract_int(x, "numGroups") for x in runs.keys()])))
Ks = [1, 5, 10]
legends = Ks.copy()

plt.rcParams["font.family"] = "Serif"
fig, ax = plt.subplots(len(loads), 1, figsize=(14 / 2.54, 21 / 2.54))
_pos = np.arange(1, 11) * (len(Ks) + 1)
_bar_width = 0.8

print("-" * 10, "flows count in each interval", "-" * 10)
dist = "./src/distribution/data/FbHdp_10percentile.csv"
distper = pd.read_csv(dist)
print(distper["xtick"].values)

row_index = 0
for load in loads:
    current_load = df[(df["numGroups"] == load)]
    bps = []
    for step, legend in enumerate(legends):
        print(load, legend)
        current_data = current_load[current_load["numECTrees"] == legend]
        flsz = current_data.iloc[0, :]["flowsize"]
        flsz.sort()
        x = []
        for fs in distper["flowsize"]:
            x.append(bisect.bisect_left(flsz, fs))
        x = [0] + x
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
        bp = ax[row_index].bar(
            _pos + step * _bar_width, flsd_intv, _bar_width, hatch=HATCHES[step], color=COLORS[step], ec="black"
        )
        bps.append(bp)

    # * xticks set only once each ax
    ax[row_index].set_xticks(_pos + _bar_width / 2, distper["xtick"])
    # ax[col_index].legend([b["boxes"][0] for b in bps], epsions)

    ax[row_index].legend(
        [b[0] for b in bps],
        [f"K={legend}" for legend in legends],
        frameon=False,
        prop={"family": "Times New Roman"},
    )

    ax[row_index].set_xlabel(f"流长度（负载{load}）")
    ax[row_index].set_ylabel("95百分位流slowdown")
    ax[row_index].set_axisbelow(True)
    ax[row_index].yaxis.grid(color="gray", linestyle="dashed", alpha=0.4)
    ax[row_index].xaxis.grid(color="gray", linestyle="dashed", alpha=0.4)
    row_index += 1

# ax[0].set_ylabel("95百分位流slowdown")
fig.subplots_adjust(left=0.1, bottom=0.05, right=0.99, top=0.99)
# fig.subplots_adjust(hspace=0.3)
fig.savefig(output_name, dpi=600)
print(f"output {output_name}")
import os

paper_path = os.path.join(os.getcwd(), f"./{output_name}")
fig.savefig(paper_path, dpi=600)
