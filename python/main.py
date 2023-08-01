from simnet.util import *
from simnet.results import get_avg_by_itvars
import pandas as pd

if __name__ == '__main__':
    sheet = read_csv("simulations", "exp", "fatTree")
    vec_name = r'job-\d+-RoundFinishTime'
    _df_veconly = sheet[sheet["name"].str.contains(pat=vec_name,
                                                   na=False,
                                                   regex=True)]
    print(_df_veconly)