import sys
import os
import bisect
from scipy.interpolate import interp1d

sys.path.append(os.getcwd())
from analysis.common import *


def get_dist_10percentile(distpath: str):
    df = pd.read_csv(distpath, names=["flowsize", "prob"], sep=" ")
    df["prob"] = df["prob"].transform(lambda x: x * 0.01)
    x = df["flowsize"]
    y = df["prob"]
    interp_func = interp1d(x, y)
    x_interp = np.linspace(x.min(), x.max(), 200000)
    y_interp = interp_func(x_interp)

    for i in range(1, 11):
        intv_index = bisect.bisect_left(y_interp, i * 0.1)
        print(x_interp[intv_index])
