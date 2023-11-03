#!/bin/python
import pandas as pd

from matplotlib import pyplot as plt

mode_names = ['Send', 'Isend', 'IsendTest', 'Psend', 'Win']

def results(file1, file2):
	dtypes = {'mode':int, 'buffer_size':int, 'partition_size':int, 't_total' : float, 't_local' : float }
	r0 = pd.read_csv(file1, dtype=dtypes)
	r1 = pd.read_csv(file2, dtype=dtypes)

	result = r0.merge(r1, how='inner', on=["mode", "buffer_size", "partition_size", "send_pattern", "partition_size_recv"], suffixes=['', '_recv'], copy=False)
	return result

if __name__ == "__main__":
	data = results('R0.csv', 'R1.csv')
	data.to_csv('results.csv')
