from analysis.util import *
from analysis.results import _COLORS, _MARKERS, get_sim_duration
import pandas as pd
import bisect

output_name = "lb_sd.png"
percentile_lowerbound = 0.95

sheet = read_csv("simulations", "exp", "fatTree", True)
_kept_rows = ["flowSize:vector", "fct:vector", "idealFct:vector"]
flows = get_vectors(
    sheet,
    names=_kept_rows,
    module="FatTree",
)
# * align the flows and jobs duration
flow_duration = get_sim_duration(sheet, "fct:vector")
job_duration = get_sim_duration(sheet, "jobRCT:vector")
_truncated_duration = min(flow_duration, job_duration) # dont know which one lasts longer
_pick_row = flows.iloc[0]["vectime"]
_truncated_index = bisect.bisect_left(_pick_row, _truncated_duration)

# * truncate the vectors
flows["vecvalue"] = flows.apply(lambda x: x["vecvalue"][:_truncated_index], axis=1)

# * a little hack to transpose the columns
flows_vec = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
flows_vec["slowdown"] = flows_vec.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise


def _extract(iterationvar: str):
    _float_number_regex = r"[+-]?(\d*\.)?\d+"
    policy = re.search(r'\$aggPolicy="([^"]+)"', iterationvar)
    load = re.search(r"\$load=(" + _float_number_regex + ")", iterationvar)
    epsion = re.search(r"\$epsion=(" + _float_number_regex + ")", iterationvar)
    assert policy is not None
    assert load is not None
    assert epsion is not None
    return (policy.group(1), load.group(1), epsion.group(1))


df = pd.DataFrame(columns=["load", "policy", "epsion", "flowsize", "slowdown"])
runs = get_runIDs(sheet, by="iterationvars")
policys = []
loads = []
epsions = []
for policy, load, epsion in map(_extract, runs):
    print(load, policy, epsion)
    policys.append(policy)
    loads.append(load)
    epsions.append(epsion)
    key = f'$load={load}, $aggPolicy="{policy}", $epsion={epsion}'
    repetition_ids = runs[key]  # pyright: ignore reportGeneralTypeIssues
    # * multiple repetition experiments
    extract_policy_load = flows_vec.loc[repetition_ids]
    flowsize = np.concatenate(extract_policy_load["flowSize:vector"].values)
    fsd = np.concatenate(extract_policy_load["slowdown"].values)
    df.loc[len(df.index)] = [load, policy, epsion, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues

df.sort_values(by=["load", "epsion"], inplace=True)
epsions = sorted(list(set(epsions)))
loads = sorted(list(set(loads)))
fig, ax = plt.subplots(1, len(loads), figsize=(50 / 2.54, 10 / 2.54))
_pos = np.arange(1, 11) * (len(epsions) + 1)
if len(loads) > 1:
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
        for step, epsion in enumerate(epsions):
            flsd: np.ndarray = _tmp[_tmp["epsion"] == epsion]["slowdown"].values[0]
            flsd_intv = []
            for l, r in itertools.pairwise(_prefix0_flsz_x100):
                perlb = round(l + (r - l) * percentile_lowerbound)
                flsd_intv.append(flsd[perlb:r].mean())
            bp = ax[col_index].plot(_pos, flsd_intv, color=_COLORS[step], marker=_MARKERS[step])
            bps.append(bp)
        # * xticks set only once each ax
        ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x])
        ax[col_index].legend([b[0] for b in bps], epsions)
        ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
    ax[0].set_ylabel("FCT slow down")
else:
    _tmp = df[(df["load"] == loads[0])]
    flsz = _tmp.iloc[0, :]["flowsize"]
    flsz.sort()
    percentiles = np.arange(10, 110, 10)  # * 10%, 20%,...100%

    flsz_x = np.percentile(flsz, percentiles).tolist()
    # * 10 intervals
    flsz_x100 = [round(i * len(flsz)) for i in [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]]
    _prefix0_flsz_x100 = [0] + flsz_x100
    flsz_x = np.percentile(flsz, percentiles).tolist()
    # * 10 intervals
    flsz_x100 = [round(i * len(flsz)) for i in [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]]
    _prefix0_flsz_x100 = [0] + flsz_x100
    bps = []
    for step, epsion in enumerate(epsions):
        flsd: np.ndarray = _tmp[_tmp["epsion"] == epsion]["slowdown"].values[0]
        flsd_intv = []
        for l, r in itertools.pairwise(_prefix0_flsz_x100):
            perlb = round(l + (r - l) * percentile_lowerbound)
            flsd_intv.append(flsd[perlb:r].mean())
        bp = ax.plot(_pos, flsd_intv, color=_COLORS[step], marker=_MARKERS[step])
        bps.append(bp)
    # * xticks set only once each ax
    ax.set_xticks(_pos, [humanize(x) for x in flsz_x])
    ax.legend([b[0] for b in bps], epsions)
    ax.set_xlabel(f"Flow size (Bytes) when load={loads[0]}")

fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
fig.subplots_adjust(wspace=0.1)
fig.savefig(output_name)
print(f"output {output_name}")
