from analysis.util import *
import pandas as pd
import cProfile
import pstats


pd.set_option('display.max_colwidth', 20)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 5000)
print_avg_slowdown('fatTree')
# cProfile.run("print_avg_slowdown('fatTree')", 'restats')
# p = pstats.Stats('restats')
# p.sort_stats(pstats.SortKey.CUMULATIVE).print_stats(10)
# p.sort_stats(pstats.SortKey.TIME).print_stats(10)