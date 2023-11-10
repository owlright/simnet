import bisect
from analysis import *

output_name = "policy_fsd.png"
percentile_lowerbound = 0.99

sheet = read_csv("simulations", "exp", "spineleaf", True)
_kept_rows = ["flowSize:vector", "fct:vector", "idealFct:vector", "jobRCT:vector"]
flows = get_vectors(
    sheet,
    names=_kept_rows,
    module="SpineLeafNetwork",
)
runs = get_runIDs(sheet, by="iterationvars")
assert isinstance(runs, dict)

truncate_vectime(flows, runs)

# * align the flows and jobs duration, as we dont know which one lasts longer
# _truncated_duration = get_min_endtime(sheet, "fct:vector", "jobRCT:vector")
# _pick_row = flows.iloc[0]["vectime"]
# _truncated_index = bisect.bisect_left(_pick_row, _truncated_duration)
# flows["vecvalue"] = flows.apply(lambda x: x["vecvalue"][:_truncated_index], axis=1)
df = get_flows_slowdown(flows, runs)
# df.sort_values(by=["policy", "load"], inplace=True)

policies = sorted(list(set([extract_str(x, 'aggPolicy') for x in runs.keys()])))
epsions = sorted(list(set([extract_float(x, 'epsion') for x in runs.keys()])))
loads = sorted(list(set([extract_float(x, 'load') for x in runs.keys()])))
fig, ax = plt.subplots(1, len(loads), figsize=(50 / 2.54, 10 / 2.54))
_pos = np.arange(1, 11) * (len(epsions) + 1)

dist = "./src/distribution/data/WebSearch_10percentile.csv"
distper = pd.read_csv(dist)

for col_index, load in enumerate(loads):
    current_load = df[(df["load"] == load)]
    bps = []
    for step, policy in enumerate(policies):
        current_policy = current_load[current_load['policy'] == policy]
        flsz = current_policy.iloc[0, :]["flowsize"]
        flsz.sort()
        x = []
        for fs in distper['flowsize']:
            x.append(bisect.bisect_left(flsz, fs))
        x = [0] + x
        x95 = []
        flsd: np.ndarray = current_policy["slowdown"].values[0]
        flsd_intv = []
        for l, r in itertools.pairwise(x):
            lb = round(l + (r - l) * percentile_lowerbound)
            x95.append((lb, r))
            if (len(flsd[lb:r]) == 0):
                print_error(f'{lb},{r}')
                exit()
            flsd_intv.append(flsd[lb:r].mean())
        bp = ax[col_index].plot(_pos, flsd_intv, color=COLORS[step], marker=MARKERS[step])
        bps.append(bp)
    # * xticks set only once each ax
    ax[col_index].set_xticks(_pos, distper['xtick'])
    ax[col_index].legend([b[0] for b in bps], policies)
    ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
ax[0].set_ylabel("FCT slow down")

fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")
