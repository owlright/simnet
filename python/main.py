from simnet.util import *
import pandas as pd
from pprint import pprint

def get_avg_fct(fctdf: pd.DataFrame):
    fct = np.vstack(fctdf["vecvalue"].to_numpy())
    return fct.mean()

def get_runID_by_itervar(itervar_name) -> dict:
    df = sheet[(sheet.type == 'itervar') & (sheet.attrname == itervar_name)].sort_values(by=['attrvalue'])
    return df['runID'].tolist()

def get_slowdown(x):
    fct = x[(x['type'] == 'vector') & (x['name']=='fct:vector')]['vecvalue'].iloc[0]
    idealfct = x[(x['type'] == 'vector') & (x['name']=='idealFct:vector')]['vecvalue'].iloc[0]
    return np.mean(fct/idealfct)

def get_runID(sheet) -> dict:
    runs_ = sheet.groupby("runID")
    all_keys = list(runs_.groups.keys())
    print("total", len(all_keys), "run numbers")
    iternames =  sheet[(sheet['type'] == 'itervar')]['attrname'].unique(); # ! incase multiple itervalues
    # repeat_number = sheet[(sheet['type'] == 'config') & (sheet['attrname'] == 'repeat')]['attrvalue'].iloc[0]
    iter_runid = dict()
    for run in all_keys:
        key = list()
        for itername in iternames:
            key.append(sheet[(sheet['runID'] == run)
                             & (sheet['type'] == 'itervar')
                             & (sheet['attrname'] == itername)
                             ]['attrvalue'].astype(float).iloc[0]
                       )
        key = tuple(key)
        if key not in iter_runid:
            iter_runid[key] = dict()
        replication = sheet[(sheet['runID'] == run)
                            & (sheet['type'] == 'runattr')
                            & (sheet['attrname'] == 'replication')
                            ]['attrvalue'].iloc[0].strip('#')
        iter_runid[key][int(replication)] = run
    return iter_runid

if __name__ == '__main__':
    sheet = read_csv("simulations", "test", "RandomCreateApp")
    # iterRunID = get_runID_by_itervar('load')
    runIds = get_runID(sheet)
    loads = [i/10 for i in range(1, 10)]
    print (loads)
    for load in loads:
        rep_runids = runIds[tuple([load])]
        accumlated_fct = 0;
        for run in rep_runids.values():
            df = sheet[sheet['runID'] == run].groupby('module').apply(get_slowdown)
            accumlated_fct += df.mean()
        print(accumlated_fct/len(rep_runids))



