import numpy as np
import re
import itertools
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg') # ! if on windows's wsl, do not use plt.show() just savefig is much faster
# matplotlib.use('QtAgg') # ! this GUI is faster then others
from .util.parse_csv import *


_PACKET_HEADER_SIZE = .02 # 20B

def print_avg_slowdown(config_name=''):
    sheet = read_csv('simulations', 'exp', config_name)
    flows  = get_vectors(sheet, names=['flowSize:vector', 'fct:vector', 'idealFct:vector'], module='FatTree')
    flows = flows.set_index(['runID', 'name'], drop=True)['vecvalue'].unstack() # pivot the name column
    flows['slowdown'] = flows.apply(lambda x: x['fct:vector'] / x['idealFct:vector'], axis=1) # rowwise

    def _extract_policy_load(iterationvar: str):
        policy = re.search(r'\$aggPolicy="([^"]+)"', iterationvar)
        load = re.search(r'\$load=([\d.]+)', iterationvar)
        assert policy is not None
        assert load is not None
        return (policy.group(1), float(load.group(1)))

    df = pd.DataFrame(columns=['load', 'policy', 'flowsize', 'slowdown'])
    runs = get_runIDs(sheet, by='iterationvars')
    for policy, load in map(_extract_policy_load, runs):
        # print(load, policy)
        repetition_ids = runs[f'$load={load}, $aggPolicy="{policy}"'] # pyright: ignore reportGeneralTypeIssues
        extract_policy_load = flows.loc[repetition_ids] # * multiple repetition experiments
        flowsize = np.concatenate( extract_policy_load['flowSize:vector'].values )
        fsd = np.concatenate( extract_policy_load['slowdown'].values )
        df.loc[len(df.index)] = [load, policy, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues

    df.sort_values(by=['load', 'policy'], inplace=True)
    _tmp = df[(df.load==0.1) & (df.policy=='sptree')]
    _FLOWSIZE_COLUMN = 'flowsize(KB)'
    _FLOWSLOWDOWN = 'slowdown'
    sptree_fsd01 = pd.DataFrame({_FLOWSIZE_COLUMN: _tmp['flowsize'].values[0]*1e-3,
                                 _FLOWSLOWDOWN : _tmp.slowdown.values[0]
                                 }).sort_values(by=[_FLOWSIZE_COLUMN]).reset_index(drop=True)
    flsz = sptree_fsd01[_FLOWSIZE_COLUMN].values
    flsd = sptree_fsd01[_FLOWSLOWDOWN].values
    flsz_x100 = [ round(i * len(flsz)) for i in [.1, .2, .3, .4, .5, .6, .7, .8, .9, 1.] ]
    _prefix0_fisz_x100 = [0] + flsz_x100
    flsz_x99 = [ round(i + (j-i)*.99) for i, j in itertools.pairwise(_prefix0_fisz_x100) ]
    flsz_x95 = [ round(i + (j-i)*.95) for i, j in itertools.pairwise(_prefix0_fisz_x100) ]
    flsz_x50 = [ round(i + (j-i)*.50) for i, j in itertools.pairwise(_prefix0_fisz_x100) ]

    flsd95 = [ flsd[l:r].mean() for l, r in zip(flsz_x95, flsz_x100) ]
    flsd99 = [ flsd[l:r].mean() for l, r in zip(flsz_x99, flsz_x100) ]
    flsd50 = [ flsd[l:r].mean() for l, r in zip(flsz_x50, flsz_x100) ]

    percentiles = np.arange(10, 110, 10) # * 10%, 20%,...100%
    flsz_x = np.percentile(flsz, percentiles).tolist() # pyright: ignore reportGeneralTypeIssues
    fig, ax = plt.subplots()
    print(flsz_x)
    print(flsd50)
    ax.plot(flsd50, 'b-', lw=2,  label='mid-sptree')
    ax.plot(flsd95, 'b-', lw=2,  label='mid-sptree')
    ax.plot(flsd99, 'b-', lw=2,  label='mid-sptree')
    ax.set_xticks(np.arange(0, 11), [
    "0", "324", "400", "500", "600", "700", "1K", "7K", "46K", "120K", "10M"])
    # ax.set_xlabel('Flow size (Bytes)', fontsize=15)
    # ax.set_ylabel('FCT slow down', fontsize=15)
    # ax.legend(ncol=1, fontsize=10)
    plt.savefig('sl.png')


    # for fs_intv_left, fs_intv_right in itertools.pairwise(flsz_x):
    #     print(fs_intv_left, fs_intv_right)
