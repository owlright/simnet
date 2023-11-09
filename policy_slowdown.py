import bisect
from analysis import *

output_name = "policy_sd.png"
percentile_lowerbound = 0.99

sheet = read_csv("simulations", "exp", "spineleaf", True)
_kept_rows = ["flowSize:vector", "fct:vector", "idealFct:vector"]
flows = get_vectors(
    sheet,
    names=_kept_rows,
    module="SpineLeafNetwork",
)
runs = get_runIDs(sheet, by="iterationvars")
assert isinstance(runs, dict)

# * align the flows and jobs duration, as we dont know which one lasts longer
_truncated_duration = get_min_endtime(sheet, "fct:vector", "jobRCT:vector")
_pick_row = flows.iloc[0]["vectime"]
_truncated_index = bisect.bisect_left(_pick_row, _truncated_duration)

df = get_flows_slowdown(flows, runs, _truncated_index)
df.sort_values(by=["load", "epsion"], inplace=True)

policies = sorted(list(set([extract_str(x, 'aggPolicy') for x in runs.keys()])))
epsions = sorted(list(set([extract_float(x, 'epsion') for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, 'load') for x in runs.keys()])))
fig, ax = plt.subplots(1, len(loads), figsize=(50 / 2.54, 10 / 2.54))
_pos = np.arange(1, 11) * (len(epsions) + 1)


for col_index, load in enumerate(loads):
    _tmp = df[(df["load"] == load)]
    flsz = _tmp.iloc[0, :]["flowsize"]
    flsz.sort()
    percentiles = np.arange(10, 110, 10)  # * 10%, 20%,...100%

    flsz_x = np.percentile(flsz, percentiles).tolist()
    # * 10 intervals
    flsz_x100 = [round(i * len(flsz)) for i in [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]]
    _prefix0_flsz_x100 = [0] + flsz_x100
    bps = []
    for step, policy in enumerate(policies):
        flsd: np.ndarray = _tmp[_tmp['policy'] == policy]["slowdown"].values[0]
        flsd_intv = []
        for l, r in itertools.pairwise(_prefix0_flsz_x100):
            perlb = round(l + (r - l) * percentile_lowerbound)
            flsd_intv.append(flsd[perlb:r].mean())
        bp = ax[col_index].plot(_pos, flsd_intv, color=COLORS[step], marker=MARKERS[step])
        bps.append(bp)
    # * xticks set only once each ax
    ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x])
    ax[col_index].legend([b[0] for b in bps], policies)
    ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
ax[0].set_ylabel("FCT slow down")

fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")
