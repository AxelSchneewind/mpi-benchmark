#!python3

import pandas as pd
import matplotlib as mpl
import numpy as np
from matplotlib import pyplot as plt


mode_names = [ 'Send', 'SendPersistent', 'Isend', 'IsendTest', 'IsendThenTest', 'IsendTestall', 'CustomPsend', 'WinSingle', 'Win', 'Psend', 'PsendParrived', 'PsendProgress', 'PsendProgressThread']
send_pattern_names = [ 'linear', 'linear inverse', 'stride (2B)', 'stride (128B)', 'stride (1KB)', 'stride (16KB)', 'random', 'random burst (128B)', 'random burst (1KB)', 'random burst (16KB)', 'neighborhood exchange' ]

marker_list = ['s', '^', '>', '<', 'v', 'x' ]
def get_marker(i):
    global marker_list
    return marker_list[i % len(marker_list)]

# returns (xValues, yValues, pattern, mode, column) all partition_size -> column plots for the given modes and patterns
def iter_results(data, modes = None, patterns = None, columns = None, thread_counts = None, filter = None, partitioning_ratios=[1]):
    if columns == None:
        return

    if filter != None:
        data = filter(data)

    data.sort_values(by=['partition_size', 'thread_count', 'send_pattern'], inplace=True)
    
    for pattern in patterns:
        for mode in modes:
            for column in columns:
                for ratio_string in partitioning_ratios:
                    r = int(ratio_string)
                    print('plotting ratio ', str(2**r) if r >= 0 else '1/'+str(2**(-r)), '-> 1')
                    for t in thread_counts:
                        plot_data = data[(data['send_pattern'] == pattern) & (data['mode'] == mode) & (data['thread_count'] == t)] 
                        if r < 0:  # m->1 partitions
                            plot_data = plot_data[plot_data['partition_size_recv'] == plot_data['partition_size'] * (2**-r)] 
                        elif r >= 0:  # 1->m partitions
                            plot_data = plot_data[plot_data['partition_size_recv'] * (2**r) == plot_data['partition_size']] 

                        if len(plot_data) == 0:
                            continue

                        xValues = plot_data['partition_size']
                        yValues = plot_data[column]
                        stdValues = None
                        if str('std_dev(' + column + ')') in plot_data:
                            stdValues = plot_data['std_dev(' +column + ')']
                        # elif column == 'bandwidth':
                        #     bw_with_std = np.array(plot_data['buffer_size']) / (np.array(plot_data['t_local']) + np.array(plot_data['std_dev(t_local)']))
                        #     stdValues = (bw_with_std - xValues)/2
                        yield (xValues, yValues, stdValues, mode, pattern, t, column, r)

def plot(ax, x, y, std, domain=None, title=None, label=None, ylabel='Bandwidth [B/s]', xlabel='partition size [B]', marker=None):
    # if std is not None and len(std) == len(x):
    #     ax.errorbar(x, y, std, None, label=label)
    # else:
    if marker is not None:
        ax.plot(x, y, marker=marker, label=label)
    else:
        ax.plot(x, y, label=label)

    if ylabel != None:
        ax.set_ylabel(ylabel)    
    if xlabel != None:
        ax.set_xlabel(xlabel)

    if title != None: 
        ax.set_title(title)

    if domain != None:
        (xdomain, ydomain) = domain
        if xdomain != None:
            l,u = xdomain
            ax.set_xlim(l,u)
        if ydomain != None:
            l,u = ydomain
            ax.set_ylim(l, u)

    ax.set_xscale(mpl.scale.LogScale(ax, base=2))

    ax.grid(visible=True)
    ax.legend()



def plot_column(data, column_names = ['bandwidth'], modes=mode_names, patterns=send_pattern_names, thread_counts=[1], ylabel='bandwidth [B/s]', title=None, domain=None, partitioning_ratios=[0]):
    # remove duplicates
    patterns = list(set(patterns))
    thread_counts = list(set(thread_counts))
    modes = list(set(modes))
    column_names = list(set(column_names))

    #
    multiple_patterns = (len(patterns) > 1)
    multiple_thread_counts = (len(thread_counts) > 1)
    multiple_partitionings = (len(partitioning_ratios) > 1)

    rows = 3
    cols = 4
    (fig, ax) = plt.subplots(rows, cols, sharex=True, sharey=True)

    num_modes = 0
    ax_per_mode = {}

    for (xValues, yValues, stdValues, mode, pattern, thread_count, column, r) in iter_results(data, modes=modes, patterns=patterns, thread_counts=thread_counts, columns=column_names, partitioning_ratios=partitioning_ratios):
        # next axis
        if not mode in ax_per_mode.keys():
            ax_per_mode[mode] = (num_modes // cols, num_modes % cols)
            num_modes = num_modes + 1

        # 
        if num_modes >= rows * cols:
            break
            
        title = str(mode)
        label = mode
        if multiple_patterns:
            label = label + ', ' + pattern
        if multiple_thread_counts:
            label = label + ', ' + str(thread_count) + ' threads'
        if multiple_partitionings:
            label = label + ', ' + str(2**r) + ' -> 1'

        (a0, a1) = ax_per_mode[mode]
        plot(ax[a0, a1], xValues, yValues, stdValues, title=title, ylabel=ylabel, label=label, domain=domain)

        if title != None:
            ax[a0, a1].set_title(title)


def plot_column_combined(data, column_names=['bandwidth'], modes=mode_names, patterns=send_pattern_names, thread_counts=[1], ylabel='bandwidth [B/s]', title='', domain=None, partitioning_ratios=[0]):
    # remove duplicates
    patterns = list(set(patterns))
    thread_counts = list(set(thread_counts))
    modes = list(set(modes))
    column_names = list(set(column_names))

    thread_counts.sort()

    #
    multiple_patterns = (len(patterns) > 1)
    multiple_thread_counts = (len(thread_counts) > 1)
    multiple_ratios = (len(partitioning_ratios) > 1)

    rows = 1
    cols = 1
    index = 0
    (fig, ax) = plt.subplots(rows, cols)

    for (xValues, yValues, stdValues, mode, pattern, thread_count, column, r) in iter_results(data, modes=modes, patterns=patterns, thread_counts=thread_counts, columns=column_names, partitioning_ratios=partitioning_ratios):
        label = mode
        if multiple_patterns:
            label = label + ', ' + pattern
        if multiple_thread_counts:
            label = label + ', ' + str(thread_count) + ' threads'
        if multiple_ratios:
            label = label + ', ' + str(2**r) + ' -> 1'

        plot(ax, xValues, yValues, stdValues, marker=get_marker(index), title=title, ylabel=ylabel, label=label, domain=domain)
        index = index + 1

