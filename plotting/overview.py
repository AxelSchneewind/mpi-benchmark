#!/bin/python

import plots
import sys
import pandas as pd
from matplotlib import pyplot as plt

import argparse


#
def evaluate_list(list_str, all_values):
    include = []
    exclude = []
    for t in list_str.split(','):
        if t == 'all':
            include = all_values
        if not t.startswith('^'):
            include = include + [t]
        else:
            exclude = exclude + [t[1:]]

    result = []
    for m in include:
        if not m in exclude:
            result = result + [m]
    
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='plotter for benchmarks', description='', epilog='')
    parser.add_argument('-c', '--columns', default='bandwidth')
    parser.add_argument('-n', '--thread-counts', default='1')
    parser.add_argument('-m', '--modes', default='all')
    parser.add_argument('-p', '--patterns', default='all')
    parser.add_argument('-y', '--ydomain', default='')
    parser.add_argument('-x', '--xdomain', default='')
    parser.add_argument('-s', '--single-plot', action='store_true')
    parser.add_argument('-t', '--title', default='')
    parser.add_argument('-f', '--file', default='results.csv')
    parser.add_argument('-o', '--output-file')
    args = parser.parse_args()

    data = pd.read_csv(args.file)
    data.sort_values(by=['mode', 'thread_count', 'send_pattern'], inplace=True)

    # only use results where send and recv partition sizes are equal
    data = data.loc[data['partition_size'] == data['partition_size_recv']]

    # select column(s)
    columns = evaluate_list(args.columns, data.columns)

    ylabel = None
    if columns[0].startswith('t_'):
        ylabel = columns[0] + ' [s]'
    if 'bandwidth' == columns[0]:
        ylabel = columns[0] + ' [B/s]'

    # select modes here
    modes = evaluate_list(args.modes, plots.mode_names)

    # select patterns here
    patterns = evaluate_list(args.patterns, plots.send_pattern_names)

    # setup domain if given
    ydomain = None
    xdomain = None
    if args.ydomain != '':
        (ymin, ymax) = args.ydomain.split(',')
        ydomain = (float(ymin), float(ymax))
    if args.xdomain != '':
        (xmin, xmax) = args.xdomain.split(',')
        xdomain = (float(xmin), float(xmax))
    domain = (xdomain, ydomain)

    title = args.title
    if title == '':
        title = args.file

    # 
    thread_counts = args.thread_counts
    if thread_counts == None:
        thread_counts = [1]
    else:
        thread_counts = [ int(t) for t in thread_counts.split(',') ]

    # 
    if args.single_plot:
        plots.plot_column_combined(data, columns, title=title, modes=modes, patterns=patterns, thread_counts=thread_counts, ylabel=ylabel, domain=domain)
    else:
        plots.plot_column(data, columns, title=title, modes=modes, patterns=patterns, thread_counts=thread_counts, ylabel=ylabel, domain=domain)

    # output depending on output file argument
    if args.output_file != None:
        plt.savefig(args.output_file)
    else:
        plt.show()
