#!/bin/python

import plots
import sys
import pandas as pd
from matplotlib import pyplot as plt

import argparse




if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='plotter for benchmarks', description='', epilog='')
    parser.add_argument('-c', '--column', default='bandwidth')
    parser.add_argument('-m', '--modes', default='all')
    parser.add_argument('-s', '--single-plot', action='store_true')
    parser.add_argument('-f', '--file', default='results.csv')
    args = parser.parse_args()

    data = pd.read_csv(args.file)
    data = data.loc[data['partition_size'] == data['partition_size_recv']]

    selected_value = args.column
    ylabel = None
    if 'bandwidth' in selected_value:
        ylabel = selected_value + ' [B/s]'

    if selected_value.startswith('t_'):
        ylabel = selected_value + ' [s]'

    include = []
    exclude = []
    for t in args.modes.split(','):
        if t == 'all':
            include = plots.mode_names
        if not t.startswith('^'):
            include = include + [t]
        else:
            exclude = exclude + [t[1:]]

    modes = []
    for m in include:
        if not m in exclude:
            modes = modes + [m]

    if args.single_plot:
        plots.plot_column_combined(data, [selected_value], modes=modes, ylabel=ylabel)
    else:
        plots.plot_column(data, [selected_value], modes=modes, ylabel=ylabel)
