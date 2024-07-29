#!/bin/python

import pandas as pd
import sys

from matplotlib import pyplot as plt

def results(file1, file2):
    dtypes = {'mode' : str, 'send_pattern' : str, 'buffer_size':int, 'partition_size':int, 't_total' : float, 't_local' : float, 'std_dev(t_total)' : float, 'std_dev(t_local)' : float }
    r0 = pd.read_csv(file1, dtype=dtypes)
    r1 = pd.read_csv(file2, dtype=dtypes)
    
    result = r0.merge(r1, how='inner', on=["mode", "buffer_size", "partition_size", "send_pattern", "partition_size_recv"], suffixes=['', '_recv'], copy=False)
    return result

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print('usage: merge_results.py rank0.csv rank1.csv results.csv')
        exit(1)

    data = results(str(sys.argv[1]), str(sys.argv[2]))
    data.to_csv(str(sys.argv[3]))
