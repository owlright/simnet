from .util import *
import itertools as itool
from typing import List, Tuple, Dict, overload
import networkx as nx


def print_slowdown(sheet):

    def _get_slowdown(x):
        fct = x[(x['type'] == 'vector')
                & (x['name'] == 'fct:vector')]['vecvalue'].iloc[0]
        idealfct = x[(x['type'] == 'vector')
                     & (x['name'] == 'idealFct:vector')]['vecvalue'].iloc[0]
        return np.mean(fct / idealfct)

    # sheet = read_csv("simulations", "test", "RandomCreateApp")
    # iterRunID = get_runID_by_itervar('load')
    runIds = get_runID(sheet)
    loads = [i / 10 for i in range(1, 10)]
    print(loads)
    for load in loads:
        rep_runids = runIds[tuple([load])]
        accumlated_fct = 0
        for run in rep_runids.values():
            df = sheet[sheet['runID'] == run].groupby('module').apply(
                _get_slowdown)
            accumlated_fct += df.mean()
        print(accumlated_fct / len(rep_runids))


# def get_avg_vector(vec_name, config_dir, config_name, itvar_name="") -> dict:
#     """all vectors must have the same size"""
#     sheet = read_csv("simulations", config_dir, config_name)
#     df_ = sheet[sheet["type"] == "itervar"]
#     itvar_avgs = {}
#     if itvar_name != "":
#         itvar_values = (
#             df_[(df_["attrname"] == itvar_name)]["attrvalue"].drop_duplicates().tolist()
#         )
#         runIds = {}
#         for itvar in itvar_values:
#             repetitions = df_[df_["attrvalue"] == itvar]["runID"].tolist()
#             runIds[itvar] = repetitions
#         # print(runIds)
#         for itvar in itvar_values:
#             repetitionIds = runIds[itvar]
#             df_ = sheet[sheet["name"].str.contains(pat=vec_name, na=False, regex=True)]

#             def calc_mean(x):
#                 if (x["type"] == "vector") and (x["runID"] in repetitionIds):
#                     return np.mean(x["vecvalue"])
#                 else:
#                     return 0

#             itvar_avgs[itvar] = df_.apply(calc_mean, axis=1).sum(axis=0) / len(
#                 repetitionIds
#             )
#             # print(itvar, itvar_avgs[itvar])
#         return itvar_avgs
#     else:
#         raise Exception("Not ready")


def get_avg_by_itvars(
        sheet: pd.DataFrame,
        vec_name: str) -> Tuple[List[str], Dict[Tuple[str], float]]:
    """Find all vectors related to vec_name

    Accept any number of itvars
    Vectors must have the same size

    Returns
    -------
    vec_names: list
    avg_vectors: dict key is vec_values combination
    """
    itvar_names = get_itvar_names(sheet)
    _df = sheet[sheet["type"] == "itervar"]
    itvar_avgs = {}
    itvar_values = [
        _df[(_df["attrname"] == itvar_name
             )]["attrvalue"].drop_duplicates().tolist()
        for itvar_name in itvar_names
    ]
    # itvar_runIds = {} # itvar_comb: runId
    # * just need the vecotrs according to vecname
    _df_veconly = sheet[sheet["name"].str.contains(pat=vec_name,
                                                   na=False,
                                                   regex=True)]
    for itvar_comb in itool.product(*itvar_values):
        query_index = _df["attrname"] == ""
        for i, itvar_name in enumerate(itvar_names):
            query_index = (_df["attrname"] == itvar_name) & (
                _df["attrvalue"] == itvar_comb[i]) | query_index
        repetition_runIds = _df[query_index]["runID"].drop_duplicates(
        ).to_list()

        # itvar_runIds[itvar_comb] = repetition_runIds
        # print(runIds)

        def _calc_mean(x):
            if (x["type"] == "vector") and (x["runID"] in repetition_runIds):
                return np.mean(x["vecvalue"])
            else:
                return 0

        itvar_avgs[itvar_comb] = _df_veconly.apply(
            _calc_mean, axis=1).sum(axis=0) / len(repetition_runIds)
    return itvar_names, itvar_avgs
