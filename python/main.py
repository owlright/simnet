from simnet.util import *
from simnet.results import get_avg_by_itvars
import pandas as pd

if __name__ == '__main__':
    sheet = read_csv("simulations", "exp", "fatTree")
    vec_name = r'job-\d+-RoundFinishTime'
    result = get_avg_by_itvars(sheet, vec_name);
    print(result)