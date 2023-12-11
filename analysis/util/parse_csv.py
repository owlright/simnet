import os
import numpy as np
import pandas as pd


def print_error(mesg: str):
    from colorama import Fore, Back, Style

    print(Fore.RED + mesg + Style.RESET_ALL)


def print_info(mesg: str):
    from colorama import Fore, Back, Style

    print(Fore.GREEN + mesg + Style.RESET_ALL)


_SCALAR_COLUMN_NAMES = ["value"]
_STATISTIC_COLUMN_NAMES = ["count", "sumweights", "mean", "stddev", "min", "max"]
_HISTOGRAM_COLUMN_NAMES = ["underflows", "overflows", "binedges", "binvalues"]
_VECTOR_COLUMN_NAMES = ["vectime", "vecvalue"]
_PARAMETER_COLUMN_NAMES = ["value"]


def _format_bytes(byte_size: float) -> tuple[float, str]:
    if byte_size < 1024.0:
        return byte_size, "B"
    else:
        for unit in ["B", "KB", "MB", "GB"]:
            byte_size /= 1024.0
            if byte_size < 1024.0:
                return byte_size, unit
    raise NotImplementedError(f"{_format_bytes.__name__} not implement units bigger than GB")


def read_csv(csv_path: str = "", use_cached=False) -> pd.DataFrame:
    if len(csv_path) == 0:
        raise ValueError("csv_path can't be empty")
    csv_name, csv_ext = os.path.splitext(os.path.basename(csv_path))
    if csv_ext != ".csv":
        raise ValueError("only accept .csv file.")
    csv_dir = os.path.dirname(csv_path)

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

    new_file = csv_path
    cached_file = os.path.join(csv_dir, csv_name + ".pkl")
    if os.path.exists(cached_file) and use_cached:
        csv_size, unit = _format_bytes(os.path.getsize(cached_file))
        print_info(f"Read cached file: {cached_file} size: {csv_size: .2f} {unit}")
        if unit == "GB":
            import ray

            ray.init(_temp_dir="/mnt/sdb/tmp", dashboard_host="0.0.0.0")
            import modin.pandas as pd
            import modin.config as modin_cfg

            modin_cfg.Engine.put("ray")
        else:
            import pandas as pd
        sheet = pd.read_pickle(cached_file)
        return sheet
    elif os.path.exists(new_file):
        csv_size, unit = _format_bytes(os.path.getsize(new_file))
        print_info(f"Read file: {new_file} size: {csv_size: .2f} {unit}")
        if unit == "GB":
            import ray

            ray.init(_temp_dir="/mnt/sdb/tmp", dashboard_host="0.0.0.0")
            import modin.pandas as pd
            import modin.config as modin_cfg

            modin_cfg.Engine.put("ray")
        else:
            import pandas as pd
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
    if by == "":  # * no iterationvars
        return sheet.iloc[0]["runID"]
    else:
        df = sheet[sheet["attrname"] == by]
        runs = df.groupby("attrvalue")["runID"].apply(list).to_dict()
        return runs
