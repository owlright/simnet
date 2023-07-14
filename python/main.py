from simnet.util import *
from simnet.results import get_avg_by_itvars
import pandas as pd

if __name__ == '__main__':
    sheet = read_csv("simulations", "test", "uniformTraffic")
    print(get_runIDs(sheet))