#!/bin/python

import pandas as pd
import sys

import matplotlib as mpl
from matplotlib import pyplot as plt


mode_names = ['Send', 'Isend', 'IsendTest', 'IsendThenTest', 'IsendTestall', 'Psend', 'PsendParrived', 'Psend progress', 'Psend progress threaded', 'PsendThreaded', 'custom Psend', 'WinSingle', 'Win']
send_pattern_names = [ "linear", "linear inverse", "stride (128B)", "stride (1KB)", "random", "random burst (128B)" "random burst (1KB)" ]

def plot(data, ax, mode, columns, domain=None, ylabel=None):
    mode_data = data[data['mode'] == mode] 
    mode_data.describe()

    l = mode_names[mode]
    for pattern in data['send_pattern'].unique():
        # only plot run with one-to-one relation between partitions
        plot_data = mode_data[(mode_data['send_pattern'] == pattern) & (mode_data['partition_size'] == mode_data['partition_size_recv'])]
        for column in columns:
            xValues = plot_data['partition_size']
            yValues = plot_data[column]

            print('Mode: ', mode_names[mode])
            print('SendPattern: ', pattern)
            print(yValues.describe())
            print()

            ax.plot(xValues, yValues, label=l)
            l = ''

    if ylabel !=None:
        ax.set_ylabel(ylabel)    
    else:
        ax.set_ylabel(columns[0])
    
    ax.grid()
    ax.set_xlabel('partition_size [B]')
    ax.set_xscale(mpl.scale.LogScale(ax, base=2))

    if domain == None:
        x = (min(ax.get_xlim()[0], min(xValues)), max(ax.get_xlim()[1], max(xValues)))
        y = (min(ax.get_ylim()[0], min(yValues)), max(ax.get_ylim()[1], max(yValues)))
        domain = (x, y)


    (xdomain, ydomain) = domain
    ax.set_ylim(ydomain)
    ax.set_xlim(xdomain)

    ax.legend()


def plot_bandwiths(data):
    column_names = [ 'bandwidth' ]
    ylabel = 'bandwidth [B/s]'
    ydomain = (0, 30 * 2**30)
    xdomain = (min(data['partition_size']), max(data['partition_size']))
    domain = (xdomain, ydomain)

    ref_value = data[data['mode'] == 0].iloc[0][column_names[0]]
    if ref_value == None:
        ref_value = 0

    (fig, ax) = plt.subplots(4, 3)
    fig.suptitle('{0} (send/recv: {1:.3e})'.format('t_total', ref_value))

    plot(data, ax[0,0],  0, column_names, domain, ylabel=ylabel)
    plot(data, ax[0,1],  1, column_names, domain, ylabel=ylabel)
    plot(data, ax[0,2],  2, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,0],  3, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,1],  4, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,2],  5, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,0],  6, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,1],  7, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,2],  8, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,0],  9, column_names, domain, ylabel=ylabel)
    #plot(data, ax[3,1], 10, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,1], 11, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,2], 12, column_names, domain, ylabel=ylabel)
    plt.show()

def plot_bandwiths_combined(data):
    column_names = [ 'bandwidth' ]
    domain = None

    ref_value = data[data['mode'] == 0].iloc[0][column_names[0]]
    if ref_value == None:
        ref_value = 0

    (fig, ax) = plt.subplots(1, 1)
    fig.suptitle('{0} (send/recv: {1:.3e})'.format('bandwidth [B/s]', ref_value))

    for i in range(len(mode_names)):
        plot(data, ax, 0, column_names, domain)

    plt.show()

def plot_times(data):
    column_names = [ 't_total' ]
    ylabel = 'total time [s]'
    ydomain = None#(0, 30 * 2**30)
    xdomain = (min(data['partition_size']), max(data['partition_size']))
    domain = (xdomain, ydomain)

    ref_value = data[data['mode'] == 0].iloc[0][column_names[0]]
    if ref_value == None:
        ref_value = 0

    (fig, ax) = plt.subplots(4, 3)
    fig.suptitle('{0} (send/recv: {1:.3e})'.format('t_total', ref_value))

    plot(data, ax[0,0],  0, column_names, domain, ylabel=ylabel)
    plot(data, ax[0,1],  1, column_names, domain, ylabel=ylabel)
    plot(data, ax[0,2],  2, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,0],  3, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,1],  4, column_names, domain, ylabel=ylabel)
    plot(data, ax[1,2],  5, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,0],  6, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,1],  7, column_names, domain, ylabel=ylabel)
    plot(data, ax[2,2],  8, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,0],  9, column_names, domain, ylabel=ylabel)
    # plot(data, ax[3,1], 10, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,1], 11, column_names, domain, ylabel=ylabel)
    plot(data, ax[3,2], 12, column_names, domain, ylabel=ylabel)
    plt.show()


def plot_times_combined(data):
    column_names = [ 't_total']
    domain = None

    ref_value = data[data['mode'] == 0].iloc[0][column_names[0]]
    if ref_value == None:
        ref_value = 0

    (fig, ax) = plt.subplots(1, 1)
    fig.suptitle('{0} (send/recv: {1:.3e})'.format('t_total', ref_value))

    for i in range(len(mode_names)):
        plot(data, ax, i, column_names, domain)

    plt.show()



if __name__ == "__main__":
    selected_value = sys.argv[1] if (len(sys.argv) > 1) else 'bandwidth'
    combined = (len(sys.argv) > 2) and sys.argv[2] == 'combined'
    filename = sys.argv[3] if (len(sys.argv) > 3) else 'results.csv'
    
    # TODO: set converters
    data = pd.read_csv(filename)

    match (selected_value, combined):
        case ('bandwidth', True):
            plot_bandwiths_combined(data)
        case ('bandwidth', False):
            plot_bandwiths(data)
        case ('time', True):
            plot_times(data)
        case ('time', False):
            plot_times_combined(data)
