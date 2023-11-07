import os
import numpy as np
import pandas as pd


def print_error(mesg: str):
    from colorama import Fore, Back, Style

    print(Fore.RED + mesg + Style.RESET_ALL)


_SCALAR_COLUMN_NAMES = ["value"]
_STATISTIC_COLUMN_NAMES = ["count", "sumweights", "mean", "stddev", "min", "max"]
_HISTOGRAM_COLUMN_NAMES = ["underflows", "overflows", "binedges", "binvalues"]
_VECTOR_COLUMN_NAMES = ["vectime", "vecvalue"]
_PARAMETER_COLUMN_NAMES = ["value"]


def read_csv(sim_dirname, exp_name, config_name, use_cached=False) -> pd.DataFrame:
    def _parse_int(s):
        return int(s) if s else np.nan

    def _parse_float(s):
        return float(s) if s else np.nan

    def _parse_ndarray(s):
        return np.fromstring(s, sep=" ") if s else None

    def _transform(row):
        if row["type"] == "scalar" and "value" in row:
            row["value"] = _parse_float(row["value"])
        if row["type"] == "vector":
            if "vectime" in row and row["vectime"] is None:
                row["vectime"] = np.array([])
            if "vecvalue" in row and row["vecvalue"] is None:
                row["vecvalue"] = np.array([])
        if row["type"] == "histogram":
            if "binedges" in row and row["binedges"] is None:
                row["binedges"] = np.array([])
            if "binvalues" in row and row["binvalues"] is None:
                row["binvalues"] = np.array([])
        if row["type"] == "itervar":
            row["attrvalue"] = row["attrvalue"].replace('"', "")
        return row

    file_prefix = os.path.join(sim_dirname, exp_name, "results", f"{config_name}")
    new_file = file_prefix + ".csv"
    cached_file = file_prefix + ".pkl"
    if os.path.exists(cached_file) and use_cached:
        sheet = pd.read_pickle(cached_file)
        return sheet

    elif os.path.exists(new_file):
        sheet = pd.read_csv(
            new_file,
            converters={
                "count": _parse_int,
                "min": _parse_float,
                "max": _parse_float,
                "mean": _parse_float,
                "stddev": _parse_float,
                "sumweights": _parse_float,
                "underflows": _parse_float,
                "overflows": _parse_float,
                "binedges": _parse_ndarray,
                "binvalues": _parse_ndarray,
                "vectime": _parse_ndarray,
                "vecvalue": _parse_ndarray,
            },
            low_memory=False,
        )
        sheet = sheet.transform(_transform, axis=1)
        sheet.rename(columns={"run": "runID"}, inplace=True)
        sheet.to_pickle(cached_file)
        return sheet
    else:
        print_error(f"Error: {new_file} not Found! make export first!")
        exit()


def get_vectors(sheet: pd.DataFrame, module=None, names=None):
    result = sheet[
        (sheet.type == "vector")
        & (True if module is None else (sheet.module == module))
        & (True if names is None else sheet["name"].isin(names))
    ].copy()
    return result

def get_runIDs(sheet: pd.DataFrame, by: str = "") -> dict[str, str] | str:
    """get all combinations of itervars and their repetition ids
    Return dict {(itvar1, itvar2,...) : runID}
    """
    if by == "":  # * no iterationvars
        return sheet.iloc[0]["runID"]
    else:
        df = sheet[sheet["attrname"] == by]
        runs = df.groupby("attrvalue")["runID"].apply(list).to_dict()
        return runs
