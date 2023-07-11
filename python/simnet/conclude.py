from .util import *

def print_slowdown():
    def _get_slowdown(x):
        fct = x[(x['type'] == 'vector') & (x['name']=='fct:vector')]['vecvalue'].iloc[0]
        idealfct = x[(x['type'] == 'vector') & (x['name']=='idealFct:vector')]['vecvalue'].iloc[0]
        return np.mean(fct/idealfct)
    sheet = read_csv("simulations", "test", "RandomCreateApp")
    # iterRunID = get_runID_by_itervar('load')
    runIds = get_runID(sheet)
    loads = [i/10 for i in range(1, 10)]
    print (loads)
    for load in loads:
        rep_runids = runIds[tuple([load])]
        accumlated_fct = 0;
        for run in rep_runids.values():
            df = sheet[sheet['runID'] == run].groupby('module').apply(_get_slowdown)
            accumlated_fct += df.mean()
        print(accumlated_fct/len(rep_runids))

def print_avg_vector(config_dir, config_name, vec_name, itervar_name='', itervar_value=None):
    ''' all vectors must have the same size
    '''
    sheet = read_csv("simulations", config_dir, config_name)
    pattern = vec_name + ':vector'
    vec = sheet[(sheet['type'] == 'vector')
                & sheet['name'].str.contains(pat=pattern, regex=True, na=False)
                ]
    if itervar_name != '':
        assert(itervar_value is not None)
        vec = vec[vec[itervar_name] == itervar_name]

    vec_avg = vec.apply(lambda x: np.mean(x['vecvalue']), axis=1)
    return vec_avg