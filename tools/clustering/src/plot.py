#!/usr/bin/env python3
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 CHOPSTIX Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#

import os
import numpy as np
import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["font.family"] = "monospace"
matplotlib.rcParams["axes.grid"] = True
matplotlib.rcParams["axes.labelsize"] = 8
matplotlib.rcParams["axes.labelweight"] = "bold"
matplotlib.rcParams["axes.titlesize"] = 11
matplotlib.rcParams["axes.titleweight"] = "heavy"
matplotlib.rcParams["savefig.dpi"] = 600
matplotlib.rcParams["xtick.labelsize"] = 6
matplotlib.rcParams["ytick.labelsize"] = 6
matplotlib.rcParams["xtick.minor.visible"] = True
matplotlib.rcParams["ytick.minor.visible"] = True
# matplotlib.rcParams['grid.alpha'] = 0.1
matplotlib.rcParams["grid.color"] = "grey"
matplotlib.rcParams["grid.linestyle"] = "dotted"
matplotlib.rcParams["grid.linewidth"] = 1


from matplotlib import pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
from pathlib import Path
from src.misc import chop_print


def instr_ipc_cluster_plot(
    plotpath,
    x,
    y,
    labels,
    centroids,
    instr_coverage,
    inv_coverage,
    custom_range=None,
    benchmark_name="unk",
    function_name="unk",
    method="unk",
):
    if plotpath is None:
        return

    Path(os.path.dirname(plotpath)).mkdir(parents=True, exist_ok=True)

    colors = list(matplotlib.colors.CSS4_COLORS.keys())
    colors.remove("red")

    if custom_range is not None:
        to_delete = []
        for idx, (xval, yval, label) in enumerate(zip(x, y, labels)):
            if xval > custom_range[0][1]:
                to_delete.append(idx)
                continue
            if yval > custom_range[1][1]:
                to_delete.append(idx)

        x = np.delete(x, to_delete)
        y = np.delete(y, to_delete)
        labels = np.delete(labels, to_delete)
        centroids = dict(centroids)
        for cent in centroids:
            centroids[cent] = centroids[cent] - len(
                [elem for elem in to_delete if elem < centroids[cent]]
            )

    vectorizer = np.vectorize(lambda x: colors[x % len(colors)])

    fig, ax = plt.subplots()
    title = "Benchmark:" + benchmark_name
    title += "\nFunction:" + function_name

    ax.scatter(x, y, c=vectorizer(labels), s=1)

    if len(centroids.values()) > 0:
        ax.scatter(
            [x[idx] for idx in centroids.values()],
            [y[idx] for idx in centroids.values()],
            c="red",
            s=10,
            marker="*",
        )

    ax.set_xlabel("Instructions")
    ax.set_ylabel("IPC")

    # create new axes on the right and on the top of the current axes
    divider = make_axes_locatable(ax)
    # below height and pad are in inches
    ax_histx = divider.append_axes("top", 1.2, pad=0.1, sharex=ax)
    ax_histy = divider.append_axes("right", 1.2, pad=0.1, sharey=ax)

    text = "Method: %s\n" % method
    text += "Clusters: %d\n" % len(centroids.keys())
    text += "\nCoverage:\n"
    text += "- %%Invocations: %2.2f%%\n" % (inv_coverage * 100)
    text += "- %%Instructions: %2.2f%%\n" % (instr_coverage * 100)

    ax.annotate(text, xy=(1.02, 1.20), xycoords="axes fraction", fontsize=7)

    # make some labels invisible
    ax_histx.xaxis.set_tick_params(labelbottom=False)
    ax_histy.yaxis.set_tick_params(labelleft=False)

    # now determine nice limits by hand:
    bins = np.arange(0, max(x) * 1.02, max(x) / 100.0)
    ax_histx.hist(x, bins=bins, density=True, stacked=True)
    ax_histx.set_title(title)

    bins = np.arange(0, max(y) * 1.02, max(y) / 100.0)
    ax_histy.hist(y, bins=bins, orientation="horizontal", density=True, stacked=True)

    # the xaxis of ax_histx and yaxis of ax_histy are shared with ax,
    # thus there is no need to manually adjust the xlim and ylim of these
    # axis.

    ax_histx.set_ylabel("Instr. Frequency")
    ax_histx.set_yticks([])
    ax_histy.set_xlabel("IPC Frequency")
    ax_histy.set_xticks([])

    fname = plotpath + ".%s.%s.%s.instr_vs_ipc_cluster.jpg" % (
        benchmark_name,
        function_name,
        method,
    )
    fname = fname.replace(" ", "_")
    plt.savefig(fname, dpi=600)
    chop_print("Plot saved in '%s'" % fname)
