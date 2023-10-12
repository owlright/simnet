import numpy as np
from .parse_csv import *

def plot_loads(config_name=''):
    sheet = read_csv('simulations', 'exp', config_name)
    _, runIDs = get_runIDs(sheet)

    job_rcts = get_vector(sheet, name='jobRCT:vector', module='FatTree')
    main_itvar = 'sptree'
    for load in [0.1, 0.5, 0.9]:
        repetition_ids = runIDs[(main_itvar, str(load))]
        result = job_rcts[job_rcts.runID.isin(repetition_ids)]
        print(result)
        break

def print_slowdown(sheet):
    def _get_slowdown(x):
        try:
            fct = x[(x['type'] == 'vector')
                    & (x['name'] == 'fct:vector')]['vecvalue'].iloc[0]
            idealfct = x[(x['type'] == 'vector')
                        & (x['name'] == 'idealFct:vector')]['vecvalue'].iloc[0]
            return np.mean(fct / idealfct)
        except:
            return 0


    # sheet = read_csv("simulations", "test", "RandomCreateApp")
    # iterRunID = get_runID_by_itervar('load')
    itvarnames, runIds = get_runIDs(sheet)

    # loads = [i / 10 for i in range(1, 10)]
    # print(loads)
    for load, rep_runids in runIds.items():
        accumlated_fct = 0
        for run in rep_runids:
            df = sheet[sheet['runID'] == run].groupby('module').apply(_get_slowdown)
            accumlated_fct += df.mean()

        print(load[0], accumlated_fct / len(rep_runids))
