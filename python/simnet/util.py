import os
import numpy as np
import pandas as pd

_SCALAR_COLUMN_NAMES = ["value"]
_STATISTIC_COLUMN_NAMES = ["count", "sumweights", "mean", "stddev", "min", "max"]
_HISTOGRAM_COLUMN_NAMES = ["underflows", "overflows", "binedges", "binvalues"]
_VECTOR_COLUMN_NAMES = ["vectime", "vecvalue"]
_PARAMETER_COLUMN_NAMES = ["value"]

def _parse_int(s):
    return int(s) if s else np.nan

def _parse_float(s):
    return float(s) if s else np.nan

def _parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None

def read_csv(sim_dirname, exp_name, config_name) -> pd.DataFrame:

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
        return row

    path = os.path.join(sim_dirname, exp_name, "results", f'{config_name}.csv')
    df = pd.read_csv(path, converters = {
        'count': _parse_int,
        'min': _parse_float,
        'max': _parse_float,
        'mean': _parse_float,
        'stddev': _parse_float,
        'sumweights': _parse_float,
        'underflows': _parse_float,
        'overflows': _parse_float,
        'binedges': _parse_ndarray,
        'binvalues': _parse_ndarray,
        'vectime': _parse_ndarray,
        'vecvalue': _parse_ndarray
    },
    low_memory=False)
    df = df.transform(_transform, axis=1)
    df.rename(columns={"run": "runID"}, inplace=True)
    return df