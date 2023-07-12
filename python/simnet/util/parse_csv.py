import os
import numpy as np
import pandas as pd
from typing import Dict
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

def get_itvar_names(sheet: pd.DataFrame) -> list:
    '''Get all iterator variable names'''
    return sheet[sheet['type'] == 'itervar']['attrname'].drop_duplicates().to_list()

def get_itvar_values(sheet, itvar_name) -> list:
    return sheet[(sheet['type'] == 'itervar')
                 & (sheet['attrname'] == itvar_name)
                 ]['attrvalue'].drop_duplicates().tolist()

def get_runID(sheet: pd.DataFrame, itvar_name):
    ''' dict[itvar_name][itvar_value]: runid
    '''
    df = sheet[sheet['type'] == 'itervar']
    itvar_values = df[(sheet['attrname'] == itvar_name)]['attrvalue'].drop_duplicates().tolist()
    runIds = []
    for itvar in itvar_values:
        runid = df[df['attrvalue'] == itvar].iloc[0,0]
        runIds.append(itvar)

    # runs_ = sheet.groupby("runID")
    # all_runIds = list(runs_.groups.keys())
    # print("total", len(all_runIds), "run ids")
    print(itvar_names)
    # repeat_number = sheet[(sheet['type'] == 'config') & (sheet['attrname'] == 'repeat')]['attrvalue'].iloc[0]

    # iter_runid = dict()
    # for run in all_keys:
    #     key = list()
    #     for itername in iternames:
    #         key.append(sheet[(sheet['runID'] == run)
    #                          & (sheet['type'] == 'itervar')
    #                          & (sheet['attrname'] == itername)
    #                          ]['attrvalue'].astype(float).iloc[0]
    #                    )
    #     if len(key) == 0:
    #         key = ('NoItervarFound')
    #     else:
    #         key = tuple(key)
    #     if key not in iter_runid:
    #         iter_runid[key] = dict()
    #     replication = sheet[(sheet['runID'] == run)
    #                         & (sheet['type'] == 'runattr')
    #                         & (sheet['attrname'] == 'replication')
    #                         ]['attrvalue'].iloc[0].strip('#')
    #     iter_runid[key][int(replication)] = run
    # return iter_runid