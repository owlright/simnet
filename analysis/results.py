from .util.parse_csv import *
import numpy as np
import re
import itertools
import matplotlib.pyplot as plt
import matplotlib
import bisect
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

def truncate_vectime(flows:pd.DataFrame, runs: dict):
    def _get_min_time(x:pd.DataFrame):
        flow_time = x.loc[x['name'] == 'fct:vector', 'vectime'].values[0]
        job_time =  x.loc[x['name'] == 'jobRCT:vector', 'vectime'].values[0]
        min_time = min(flow_time[-1], job_time[-1])
        return bisect.bisect_left(flow_time, min_time)
    truncated_index = flows.groupby('runID').apply(_get_min_time)
    original_index = flows.groupby('runID').apply(lambda x: len(x['vecvalue'].values[0]))
    flows['endIndex'] = flows['runID'].map(truncated_index)
    flows['originalIndex'] = flows['runID'].map(original_index)
    flows['vecvalue'] = flows.apply(lambda x: x['vecvalue'][:x['endIndex']], axis=1)
    flows.drop('endIndex', axis=1, inplace=True)

def get_flows_slowdown(flows: pd.DataFrame, runs: dict):
    # * a little hack to transpose the columns
    flows_vec = flows.set_index(["runID", "name"], drop=True)["vecvalue"].unstack()  # pivot the name column
    flows_vec["slowdown"] = flows_vec.apply(lambda x: x["fct:vector"] / x["idealFct:vector"], axis=1)  # rowwise

    df = pd.DataFrame(columns=["load", "policy", "epsion", "flowsize", "slowdown"])
    assert isinstance(runs, dict)
    for itervar, repetition_ids in runs.items():
        policy, load, epsion = extract_iterationvar(itervar)
        print(load, policy, epsion)
        flowreps = flows_vec.loc[repetition_ids]
        flowsize = np.concatenate(flowreps["flowSize:vector"].values)
        fsd = np.concatenate(flowreps["slowdown"].values)
        df.loc[len(df.index)] = [load, policy, epsion, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues
    return df
