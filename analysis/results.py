from .util.parse_csv import *
import numpy as np
import re
import itertools
import matplotlib.pyplot as plt
import matplotlib

# ! if on windows's wsl, do not use plt.show() just savefig is much faster
matplotlib.use("Agg")
# matplotlib.use('QtAgg') # ! this GUI is faster then others


_PACKET_HEADER_SIZE = 0.02  # 20B
_COLORS = ["gray", "blue", "green", "red"]
_MARKERS = [".", "s", "*", "+"]
def humanize(number):
    if number < 1000:
        return str(round(number))
    elif number < 1e6:
        return f"{round(number / 1e3):.0f}K"
    elif number < 1e9:
        return f"{round(number / 1e6):.0f}M"


def plot_policies_slowdown(config_name=""):
    sheet = read_csv("simulations", "exp", config_name)
    flows = get_vectors(
        sheet,
        names=["flowSize:vector", "fct:vector", "idealFct:vector"],
        module="FatTree",
    )
    flows = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows["slowdown"] = flows.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    def _extract_policy_load(iterationvar: str):
        policy = re.search(r'\$aggPolicy="([^"]+)"', iterationvar)
        load = re.search(r"\$load=([\d.]+)", iterationvar)
        assert policy is not None
        assert load is not None
        return (policy.group(1), float(load.group(1)))

    df = pd.DataFrame(columns=["load", "policy", "flowsize", "slowdown"])
    runs = get_runIDs(sheet, by="iterationvars")
    policys = []
    loads = []
    for policy, load in map(_extract_policy_load, runs):
        print(load, policy)
        policys.append(policy)
        loads.append(load)
        repetition_ids = runs[f'$load={load}, $aggPolicy="{policy}"']  # pyright: ignore reportGeneralTypeIssues
        # * multiple repetition experiments
        extract_policy_load = flows.loc[repetition_ids]
        flowsize = np.concatenate(extract_policy_load["flowSize:vector"].values)
        fsd = np.concatenate(extract_policy_load["slowdown"].values)
        df.loc[len(df.index)] = [load, policy, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues
    df.sort_values(by=["load", "policy"], inplace=True)
    policys = list(set(policys))
    loads = list(set(loads))
    fig, ax = plt.subplots(1, len(loads), figsize=(50 / 2.54, 10 / 2.54))
    _pos = np.arange(1, 11) * (len(policys) + 1)
    for col_index, load in enumerate(loads):
        _tmp = df[(df["load"] == load)]
        flsz = _tmp.iloc[0, :]["flowsize"]
        flsz.sort()
        percentiles = np.arange(10, 110, 10)  # * 10%, 20%,...100%
        # pyright: ignore reportGeneralTypeIssues
        flsz_x = np.percentile(flsz, percentiles).tolist()
        # * 10 intervals
        flsz_x100 = [round(i * len(flsz)) for i in [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]]
        _prefix0_flsz_x100 = [0] + flsz_x100
        bps = []
        for step, policy in enumerate(policys):
            flsd = _tmp[_tmp["policy"] == policy]["slowdown"].values[0]
            flsd_intv = [flsd[l:r] for l, r in itertools.pairwise(_prefix0_flsz_x100)]
            bp = ax[col_index].boxplot(
                flsd_intv,
                False,
                "",
                widths=0.4,
                patch_artist=True,
                positions=_pos - step,
                boxprops=dict(facecolor=_COLORS[step]),
            )
            bps.append(bp)
        # * xticks set only once each ax
        ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x])
        ax[col_index].legend([x["boxes"][0] for x in bps], policys)
        ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
    ax[0].set_ylabel("FCT slow down")
    fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
    fig.subplots_adjust(wspace=0.1)
    fig.savefig("slb.png")

def boxplot_loadbalance_slowdown(config_name: str="", percentile_interval:int=95):
    assert 0 <= percentile_interval and percentile_interval <= 100
    percentile_lowerbound = percentile_interval / 100.0
    sheet = read_csv("simulations", "exp", config_name, True)
    flows = get_vectors(
        sheet,
        names=["flowSize:vector", "fct:vector", "idealFct:vector"],
        module="FatTree",
    )
    flows = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows["slowdown"] = flows.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    def _extract(iterationvar: str):
        _float_number_regex = r"[+-]?(\d*\.)?\d+"
        policy = re.search(r'\$aggPolicy="([^"]+)"', iterationvar)
        load = re.search(r"\$load=(" + _float_number_regex + ")", iterationvar)
        epsion = re.search(r"\$epsion=(" + _float_number_regex + ")", iterationvar)
        assert policy is not None
        assert load is not None
        assert epsion is not None
        return (policy.group(1), float(load.group(1)), float(epsion.group(1)))

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

        repetition_ids = runs[
            f'$load={load}, $aggPolicy="{policy}", $epsion={epsion}'
        ]  # pyright: ignore reportGeneralTypeIssues
        # * multiple repetition experiments
        extract_policy_load = flows.loc[repetition_ids]
        flowsize = np.concatenate(extract_policy_load["flowSize:vector"].values)
        fsd = np.concatenate(extract_policy_load["slowdown"].values)
        df.loc[len(df.index)] = [load, policy, epsion, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues

    df.sort_values(by=["load", "epsion"], inplace=True)
    epsions = sorted(list(set(epsions)))
    loads = sorted(list(set(loads)))
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
        for step, epsion in enumerate(epsions):
            flsd:np.ndarray = _tmp[_tmp["epsion"] == epsion]["slowdown"].values[0]
            flsd_intv = []
            for l, r in itertools.pairwise(_prefix0_flsz_x100):
                perlb = round(l + (r-l) * percentile_lowerbound)
                flsd_intv.append(flsd[perlb:r])
            bp = ax[col_index].boxplot(
                flsd_intv,
                False,
                "",
                widths=0.4,
                patch_artist=True,
                positions=_pos - step,
                boxprops=dict(facecolor=_COLORS[step]),
            )
            bps.append(bp)
        # * xticks set only once each ax
        ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x])
        ax[col_index].legend([b["boxes"][0] for b in bps], epsions)
        ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
    ax[0].set_ylabel("FCT slow down")
    fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
    fig.subplots_adjust(wspace=0.1)
    fig.savefig("bpls.png")


def plot_loadbalance_slowdown(config_name: str="", percentile_interval:int=0):
    assert 0 <= percentile_interval and percentile_interval <= 100
    percentile_lowerbound = percentile_interval / 100.0
    sheet = read_csv("simulations", "exp", config_name, True)
    flows = get_vectors(
        sheet,
        names=["flowSize:vector", "fct:vector", "idealFct:vector"],
        module="FatTree",
    )
    flows = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows["slowdown"] = flows.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    def _extract(iterationvar: str):
        _float_number_regex = r"[+-]?(\d*\.)?\d+"
        policy = re.search(r'\$aggPolicy="([^"]+)"', iterationvar)
        load = re.search(r"\$load=(" + _float_number_regex + ")", iterationvar)
        epsion = re.search(r"\$epsion=(" + _float_number_regex + ")", iterationvar)
        assert policy is not None
        assert load is not None
        assert epsion is not None
        return (policy.group(1), float(load.group(1)), float(epsion.group(1)))

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

        repetition_ids = runs[
            f'$load={load}, $aggPolicy="{policy}", $epsion={epsion}'
        ]  # pyright: ignore reportGeneralTypeIssues
        # * multiple repetition experiments
        extract_policy_load = flows.loc[repetition_ids]
        flowsize = np.concatenate(extract_policy_load["flowSize:vector"].values)
        fsd = np.concatenate(extract_policy_load["slowdown"].values)
        df.loc[len(df.index)] = [load, policy, epsion, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues

    df.sort_values(by=["load", "epsion"], inplace=True)
    epsions = sorted(list(set(epsions)))
    loads = sorted(list(set(loads)))
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
        for step, epsion in enumerate(epsions):
            flsd:np.ndarray = _tmp[_tmp["epsion"] == epsion]["slowdown"].values[0]
            flsd_intv = []
            for l, r in itertools.pairwise(_prefix0_flsz_x100):
                perlb = round(l + (r-l) * percentile_lowerbound)
                flsd_intv.append(flsd[perlb:r].mean())
            bp = ax[col_index].plot(_pos, flsd_intv, color = _COLORS[step], marker=_MARKERS[step])
            bps.append(bp)
        # * xticks set only once each ax
        ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x])
        ax[col_index].legend([b[0] for b in bps], epsions)
        ax[col_index].set_xlabel(f"Flow size (Bytes) when load={load}")
    ax[0].set_ylabel("FCT slow down")
    fig.subplots_adjust(left=0.05, bottom=0.15, right=0.95, top=0.95)
    fig.subplots_adjust(wspace=0.1)
    fig.savefig("pls.png")
