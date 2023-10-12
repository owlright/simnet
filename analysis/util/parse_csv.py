import os
import numpy as np
import pandas as pd
import re
import itertools as itool
import csv
from typing import Dict, List, Tuple, Literal


def print_error(mesg: str):
    from colorama import Fore, Back, Style
    print(Fore.RED + mesg + Style.RESET_ALL)

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
        if row["type"] == "itervar":
            row["attrvalue"] = row["attrvalue"].replace('"', '')
        return row

    path = os.path.join(sim_dirname, exp_name, "results", f'{config_name}.csv')
    try:
        sheet = pd.read_csv(path,
                        converters={
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
        sheet = sheet.transform(_transform, axis=1)
        sheet.rename(columns={"run": "runID"}, inplace=True)
        return sheet
    except FileNotFoundError:
        print_error(f'Error: {path} not Found! make export first!')
        exit()

def get_vector(sheet: pd.DataFrame, module=None, name=None):
    return sheet[
        (sheet.type == 'vector')
        & (True if module is None else (sheet.module == module))
        & (True if name is None else (sheet.name == name))
    ].copy()

def get_vector_by_regex(sheet: pd.DataFrame, module=None, name=None):
    module_names = []
    for modname in sheet.module:
        assert module is not None
        module_names.append(re.match(module, str(modname))!=None)
    return sheet[
        (sheet.type == 'vector')
        & (True if module is None else (module_names))
        & (True if name is None else (sheet.name == f'{name}'))
    ].copy()

def get_scalar(sheet, module=None, name=None):
    return sheet[
        (sheet.type == 'scalar')
        & (True if module is None else (sheet.module == module))
        & (True if name is None else (sheet.name == name))
    ].copy()

def get_scalar_regex(sheet: pd.DataFrame, module=None, name=None):
    moduleIndex = []
    for imodule in sheet.module:
        assert module is not None
        moduleIndex.append(re.match(module, str(imodule))!=None)

    return sheet[
        (sheet.type == 'scalar')
        & (True if module is None else (moduleIndex))
        & (True if name is None else (sheet.name == f'{name}'))
    ].copy()

# def get_itervar(sheet: pd.DataFrame, run, attrname: str):
#     values = sheet[
#         (sheet.type == 'itervar')
#         & (sheet.runID == run)
#         & (sheet.attrname == attrname)
#     ].iloc[0].attrvalue
#     return values

def get_itervar_names(sheet: pd.DataFrame) -> list:
    """Get all itvar names"""
    return sheet[sheet['type'] == 'itervar']['attrname'].drop_duplicates().to_list()

def get_itvar_values(sheet: pd.DataFrame, itvar_name: str) -> list:
    """Get all itvar values"""
    return sheet[(sheet['type'] == 'itervar')
                 & (sheet['attrname'] == itvar_name)]['attrvalue'].drop_duplicates().tolist()

def get_runIDs(sheet: pd.DataFrame) -> tuple[List[str], Dict[tuple[str,...], List[str]]]:
    """ get all combinations of itervars and their repetition ids
    Return dict {(itvar1, itvar2,...) : runID}
    """
    itvar_names = get_itervar_names(sheet)
    runIDs = {}
    itvars = []
    for itvar_name in itvar_names:
        itvars.append(get_itvar_values(sheet, itvar_name))

    for itvar_comb in itool.product(*itvars):
        query_index = sheet["attrname"] == ""
        for i, itvar_name in enumerate(itvar_names):
            query_index = (sheet["attrname"] == itvar_name) & (sheet["attrvalue"] == itvar_comb[i]) | query_index
        repetition_runIds = sheet[query_index]["runID"].drop_duplicates().to_list()
        runIDs[itvar_comb] = repetition_runIds
    return itvar_names, runIDs



