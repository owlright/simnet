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


def get_sim_duration(sheet: pd.DataFrame, vec_name: str) -> float:
    fcts = sheet[(sheet.type == "vector") & (sheet.name == vec_name)]
    fctimes = np.array(fcts["vectime"].values.tolist())
    return fctimes.max()


def get_min_endtime(sheet: pd.DataFrame, *vecnames: str):
    durations = [get_sim_duration(sheet, vecname) for vecname in vecnames]
    return min(durations)


def extract_str(iterationvars: str, name: str) -> str:
    value = re.search(r"\$" + name + '="([^"]+)"', iterationvars)
    assert value is not None
    return value.group(1)


def extract_float(iterationvars: str, name: str) -> float:
    value = re.search(r"\$" + name + r"=([+-]?(\d*\.)?\d+)", iterationvars)
    assert value is not None
    return float(value.group(1))


def extract_iterationvar(iterationvar: str):
    _float_number_regex = r"[+-]?(\d*\.)?\d+"
    policy = extract_str(iterationvar, "aggPolicy")
    load = extract_float(iterationvar, "load")
    epsion = extract_float(iterationvar, "epsion")
    return policy, load, epsion


def get_flows_slowdown(flows: pd.DataFrame, runs: dict, truncated_index: int = -1):
    # * align the flows and jobs duration, as we dont know which one lasts longer
    # _truncated_duration = get_min_endtime(sheet, "fct:vector", "jobRCT:vector")
    # _pick_row = flows.iloc[0]["vectime"]
    # _truncated_index = bisect.bisect_left(_pick_row, _truncated_duration)
    # * truncate the vectors
    flows["vecvalue"] = flows.apply(lambda x: x["vecvalue"][:truncated_index], axis=1)

    # * a little hack to transpose the columns
    flows_vec = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows_vec["slowdown"] = flows_vec.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    df = pd.DataFrame(columns=["load", "policy", "epsion", "flowsize", "slowdown"])
    policys = []
    loads = []
    epsions = []
    assert isinstance(runs, dict)
    for itervar, repetition_ids in runs.items():
        policy, load, epsion = extract_iterationvar(itervar)
        print(load, policy, epsion)
        policys.append(policy)
        loads.append(load)
        epsions.append(epsion)
        # * multiple repetition experiments
        extract_policy_load = flows_vec.loc[repetition_ids]
        flowsize = np.concatenate(extract_policy_load["flowSize:vector"].values)
        fsd = np.concatenate(extract_policy_load["slowdown"].values)
        df.loc[len(df.index)] = [load, policy, epsion, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues
    return df
