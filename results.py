#!/bin/python

import plots
import sys
import pandas as pd
from matplotlib import pyplot as plt


def plot_column(data, column_names = ['bandwidth'], ylabel='bandwidth [B/s]', domain=None):
    rows = 4
    cols = 4
    (fig, ax) = plt.subplots(rows, cols)

    num_modes = 0
    modes =  plots.mode_names
    patterns =  plots.send_pattern_names
    ax_per_mode = {}

    for (xValues, yValues, mode, pattern, column) in plots.iter_results(data, modes=modes, patterns=patterns, columns=column_names):
        # next axis
        if not mode in ax_per_mode.keys():
            ax_per_mode[mode] = (num_modes // cols, num_modes % cols)
            num_modes = num_modes + 1

        if num_modes >= rows * cols:
            break
            
        title = str(mode)
        label = mode + ', ' + pattern
        (a0, a1) = ax_per_mode[mode]
        plots.plot(ax[a0, a1], xValues, yValues, domain, title=title, label=label)

    plt.show()

def plot_column_combined(data, column_names=['bandwidth'], domain=None):
    rows = 1
    cols = 1
    (fig, ax) = plt.subplots(rows, cols)

    modes =  plots.mode_names
    patterns =  plots.send_pattern_names

    for (xValues, yValues, mode, pattern, column) in plots.iter_results(data, modes=modes, patterns=patterns, columns=column_names):
        title = column
        label = mode + ', ' + pattern
        plots.plot(ax[0, 0], xValues, yValues, domain, title=title, label=label)


if __name__ == "__main__":
    selected_value = sys.argv[1] if (len(sys.argv) > 1) else 'bandwidth'
    combined = (len(sys.argv) > 2) and sys.argv[2] == 'combined'
    filename = sys.argv[3] if (len(sys.argv) > 3) else 'results.csv'
    
    data = pd.read_csv(filename)

    if selected_value == 'time':
        selected_value = 't_total'

    domain = None
    if selected_value == 'bandwidth':
        domain = ((min(data['partition_size']), max(data['partition_size'])), (0, 30 * 2**30))

    if combined:
        plot_column_combined(data, [selected_value], domain=domain)
    else:
        plot_column(data, [selected_value], domain=domain)
