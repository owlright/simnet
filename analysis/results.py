import numpy as np
from .util.parse_csv import *


def print_avg_slowdown(config_name=''):
    sheet = read_csv('simulations', 'exp', config_name)
    runs = get_runIDs(sheet, by='iterationvars')

    flows  = get_vectors(sheet, names=['flowSize:vector', 'fct:vector', 'idealFct:vector'], module='FatTree')
    flows = flows.set_index(['runID', 'name'], drop=True)['vecvalue'].unstack() # pivot the name column
    flows['slowdown'] = flows.apply(lambda x: x['fct:vector'] / x['idealFct:vector'], axis=1) # rowwise

    policy_load = {'sptree': {}, 'edge': {}}

    for policy, slowdown in policy_load.items():
        for load in [0.1, 0.5, 0.9]:
            repetition_ids = runs[f'$load={load}, $aggPolicy="{policy}"'] # pyright: ignore reportGeneralTypeIssues
            _tmp = flows.loc[repetition_ids]['slowdown']
            slowdown[load] = np.mean(_tmp.map(lambda x: np.mean(x)))
            print(policy, load, slowdown[load])
    return policy_load
