#!python3

import pandas as pd
import matplotlib as mpl
from matplotlib import pyplot as plt


mode_names = [ 'Send', 'SendPersistent', 'Isend', 'IsendTest', 'IsendThenTest', 'IsendTestall', 'CustomPsend', 'WinSingle', 'Win', 'Psend', 'PsendParrived', 'PsendProgress', 'PsendProgressThread', 'PsendThreaded']
send_pattern_names = [ 'linear', 'linear inverse', 'stride (128B)', 'stride (1KB)', 'stride (16KB)', 'random', 'random burst (128B at a time)', 'random burst (1KB at a time)', 'random burst (16KB at a time)' ]

# returns (xValues, yValues, pattern, mode, column) all partition_size -> column plots for the given modes and patterns
def iter_results(data, modes = None, patterns = None, columns = None, filter = None):
    if columns == None:
        return

    if filter != None:
        data = filter(data)
    
    for pattern in patterns:
        for mode in modes:
            for column in columns:
                plot_data = data[(data['send_pattern'] == pattern) & (data['mode'] == mode)] 
                if len(plot_data) == 0:
                    continue
                xValues = plot_data['partition_size']
                yValues = plot_data[column]
                yield (xValues, yValues, mode, pattern, column)

def plot(ax, x, y, domain=None, title=None, label=None, ylabel='Bandwidth [B/s]', xlabel='partition size [B]'):
    ax.plot(x, y, label=label)

    ax.grid()
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

    ax.legend()


# def plot(data, ax, mode, columns, domain=None, ylabel=None):
#     mode_data = data[data['mode'] == mode] 
#     mode_data.describe()
# 
#     for pattern in data['send_pattern'].unique():
#         # only plot run with one-to-one relation between partitions
#         plot_data = mode_data[(mode_data['send_pattern'] == pattern) & (mode_data['partition_size'] == mode_data['partition_size_recv'])]
#         for column in columns:
#             xValues = plot_data['partition_size']
#             yValues = plot_data[column]
# 
#             if len(yValues) > 0:
#                 ax.plot(xValues, yValues, label=mode + ', ' + pattern)
# 
#     if ylabel !=None:
#         ax.set_ylabel(ylabel)    
#     else:
#         ax.set_ylabel(columns[0])
#     
#     ax.grid()
#     ax.set_xlabel('partition_size [B]')
#     ax.set_xscale(mpl.scale.LogScale(ax, base=2))
# 
#     if domain == None:
#         x = (min(ax.get_xlim()[0], min(xValues)), max(ax.get_xlim()[1], max(xValues)))
#         y = (min(ax.get_ylim()[0], min(yValues)), max(ax.get_ylim()[1], max(yValues)))
#         domain = (x, y)
# 
# 
#     (xdomain, ydomain) = domain
#     ax.set_ylim(ydomain)
#     ax.set_xlim(xdomain)
# 
#     ax.legend()
# 



def plot_column(data, column_names = ['bandwidth'], modes=mode_names, patterns=send_pattern_names, ylabel='bandwidth [B/s]', title=None, domain=None):
    rows = 3
    cols = 4
    (fig, ax) = plt.subplots(rows, cols, sharex=True, sharey=True)

    num_modes = 0
    ax_per_mode = {}

    for (xValues, yValues, mode, pattern, column) in iter_results(data, modes=modes, patterns=patterns, columns=column_names):
        # next axis
        if not mode in ax_per_mode.keys():
            ax_per_mode[mode] = (num_modes // cols, num_modes % cols)
            num_modes = num_modes + 1

        # 
        if num_modes >= rows * cols:
            break
            
        title = str(mode)
        label = mode + ', ' + pattern
        (a0, a1) = ax_per_mode[mode]
        plot(ax[a0, a1], xValues, yValues, title=title, ylabel=ylabel, label=label, domain=domain)

        if title != None:
            ax[a0, a1].set_title(title)


def plot_column_combined(data, column_names=['bandwidth'], modes=mode_names, patterns=send_pattern_names, ylabel='bandwidth [B/s]', title='', domain=None):
    rows = 1
    cols = 1
    (fig, ax) = plt.subplots(rows, cols)

    for (xValues, yValues, mode, pattern, column) in iter_results(data, modes=modes, patterns=patterns, columns=column_names):
        label = mode + ', ' + pattern
        plot(ax, xValues, yValues, title=title, ylabel=ylabel, label=label, domain=domain)

