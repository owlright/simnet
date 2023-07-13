from simnet.util import *
from simnet.results import get_avg_by_itvars
import pandas as pd

if __name__ == '__main__':
    tmp = get_avg_by_itvars("exp", "AggSptreeOnFatTree", 'job-.*-RoundFinishTime')
    print(tmp)