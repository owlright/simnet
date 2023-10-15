import numpy as np
import re
import itertools
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg') # ! if on windows's wsl, do not use plt.show() just savefig is much faster
# matplotlib.use('QtAgg') # ! this GUI is faster then others
from .util.parse_csv import *


_PACKET_HEADER_SIZE = .02 # 20B
def humanize(number):
    if number < 1000:
        return str(round(number))
    elif number < 1e6:
        return f"{round(number / 1e3):.0f}K"
    elif number < 1e9:
        return f"{round(number / 1e6):.0f}M"

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
    policys = []
    loads = []
    for policy, load in map(_extract_policy_load, runs):
        print(load, policy)
        policys.append(policy)
        loads.append(load)
        repetition_ids = runs[f'$load={load}, $aggPolicy="{policy}"'] # pyright: ignore reportGeneralTypeIssues
        extract_policy_load = flows.loc[repetition_ids] # * multiple repetition experiments
        flowsize = np.concatenate( extract_policy_load['flowSize:vector'].values )
        fsd = np.concatenate( extract_policy_load['slowdown'].values )
        df.loc[len(df.index)] = [load, policy, flowsize, fsd]  # pyright: ignore reportGeneralTypeIssues
    df.sort_values(by=['load', 'policy'], inplace=True)
    policys = list(set(policys))
    loads = list(set(loads))
    fig, ax = plt.subplots(1, len(loads), figsize=(50/2.54, 10/2.54))
    _pos = np.arange(1, 11) * (len(policys) +1)
    for col_index, load in enumerate(loads):
        _tmp = df[(df['load']==load)]
        flsz = _tmp.iloc[0, :]['flowsize']
        flsz.sort()
        percentiles = np.arange(10, 110, 10) # * 10%, 20%,...100%
        flsz_x = np.percentile(flsz, percentiles).tolist() # pyright: ignore reportGeneralTypeIssues
        flsz_x100 = [ round(i * len(flsz)) for i in [.1, .2, .3, .4, .5, .6, .7, .8, .9, 1.] ] # * 10 intervals
        _prefix0_flsz_x100 = [0] + flsz_x100
        colors = ['pink', 'lightblue', 'lightgreen']
        bps = []
        for step, policy in enumerate(policys):
            flsd = _tmp[_tmp['policy']==policy]['slowdown'].values[0]
            flsd_intv = [flsd[l:r] for l, r in itertools.pairwise(_prefix0_flsz_x100)]
            bp = ax[col_index].boxplot(flsd_intv, False, '', widths=.4, patch_artist=True, positions=_pos - step, boxprops=dict(facecolor=colors[step]))
            bps.append(bp)
        ax[col_index].set_xticks(_pos, [humanize(x) for x in flsz_x]) # * xticks set only once each ax
        ax[col_index].legend([x['boxes'][0] for x in bps], policys)
        ax[col_index].set_xlabel(f'Flow size (Bytes) when load={load}')
    ax[0].set_ylabel('FCT slow down')
    fig.subplots_adjust(left=0.05, bottom=.15, right=0.95, top=.95)
    fig.subplots_adjust(wspace=.1)
    fig.savefig('slb.png')