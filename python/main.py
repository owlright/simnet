from simnet.util import *
import pandas as pd

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

if __name__ == '__main__':
    sheet = read_csv("simulations", "unicast", "spineleaf")
    iterRunID = get_runID_by_itervar('load')
    for run in iterRunID:
        df = sheet[(sheet.runID == run)].groupby('module').apply(get_slowdown)
        print(df.mean())


