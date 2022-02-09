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
import csv
import argparse
from os.path import join
from os import scandir
import json
import numpy as np
import matplotlib.pyplot as plt

from src.trace import Trace
from src.clustering import dbscan, ClusteringInformation
from src.perfmetrics import (
    PerformanceMetrics,
    Benchmark,
    load_invocations_from_file,
    Microbenchmark,
    Function,
    weight_of_microbenchmark,
)


def load_function_data(results_path, benchmark):
    with open(join(results_path, "functions.txt")) as file:
        lines = file.readlines()[1:]

        for line in lines:
            _, name, _, _, weight, _, _ = line.split("\t")

            weight = float(weight[:-1]) / 100

            cluster_info = ClusteringInformation.from_file(
                join(results_path, name + "_cluster.json")
            )
            invocations = load_invocations_from_file(
                join(results_path, name, "benchmark.csv")
            )

            function = Function(cluster_info, invocations, weight)

            benchmark.add_function(name, function)


def bar_plot(ax, data, colors=None, total_width=0.8, single_width=1, legend=True):
    """Draws a bar plot with multiple bars per data point.

    Parameters
    ----------
    ax : matplotlib.pyplot.axis
        The axis we want to draw our plot on.

    data: dictionary
        A dictionary containing the data we want to plot. Keys are the names of the
        data, the items is a list of the values.

        Example:
        data = {
            "x":[1,2,3],
            "y":[1,2,3],
            "z":[1,2,3],
        }

    colors : array-like, optional
        A list of colors which are used for the bars. If None, the colors
        will be the standard matplotlib color cyle. (default: None)

    total_width : float, optional, default: 0.8
        The width of a bar group. 0.8 means that 80% of the x-axis is covered
        by bars and 20% will be spaces between the bars.

    single_width: float, optional, default: 1
        The relative width of a single bar within a group. 1 means the bars
        will touch eachother within a group, values less than 1 will make
        these bars thinner.

    legend: bool, optional, default: True
        If this is set to true, a legend will be added to the axis.
    """

    # Check if colors where provided, otherwhise use the default color cycle
    if colors is None:
        colors = plt.rcParams["axes.prop_cycle"].by_key()["color"]

    # Number of bars per group
    n_bars = len(data)

    # The width of a single bar
    bar_width = total_width / n_bars

    # List containing handles for the drawn bars, used for the legend
    bars = []

    # Iterate over all data
    for i, (name, values) in enumerate(data.items()):
        # The offset in x direction of that bar
        x_offset = (i - n_bars / 2) * bar_width + bar_width / 2

        # Draw a bar for every value of that type
        for x, y in enumerate(values):
            bar = ax.bar(
                x + x_offset,
                y,
                width=bar_width * single_width,
                color=colors[i % len(colors)],
            )

        # Add a handle to the last drawn bar, which we'll need for the legend
        bars.append(bar[0])

    # Draw legend if we need
    if legend:
        ax.legend(bars, data.keys(), loc="lower right")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Report the average CPI of a set of microbenchmarks (properly weighted)"
    )

    parser.add_argument("json")

    args = parser.parse_args()

    benchmark_ipcs = []
    micro_ipcs = []
    excluding_ipcs = []
    labels = []

    with open(args.json, "r") as file:
        json_obj = json.load(file)
        for benchmark_entry in json_obj["benchmarks"]:
            labels.append(benchmark_entry["name"])
            benchmark = Benchmark()
            load_function_data(benchmark_entry["results_path"], benchmark)

            benchmark.normalize_sample_percentages()

            microbenchmarks = []
            for function in benchmark.functions:
                function_results_path = join(benchmark_entry["results_path"], function)
                for file in scandir(function_results_path):
                    if not file.name.startswith("benchmark") and file.name.endswith(
                        "_global.csv"
                    ):
                        with open(file.path) as f:
                            reader = csv.DictReader(f)
                            index = 0
                            metrics = None
                            for row in reader:
                                assert metrics == None
                                metrics = PerformanceMetrics(
                                    int(row[" Time Elapsed (us)"]),
                                    int(row[" Retired Instructions"]),
                                    int(row["Cycles"]),
                                    int(row[" Retired Memory Instructions"]),
                                    int(row[" Data Cache Misses"]),
                                )
                        invocation = int(
                            file.name[len(function) + 1 : file.name.find("#")]
                        )
                        microbenchmarks.append(
                            Microbenchmark(function, invocation, metrics)
                        )

            with open(
                join(benchmark_entry["results_path"], "benchmark_global.csv")
            ) as file:
                reader = csv.DictReader(file)
                index = 0
                metrics = None
                for row in reader:
                    assert metrics == None
                    metrics = PerformanceMetrics(
                        int(row[" Time Elapsed (us)"]),
                        int(row[" Retired Instructions"]),
                        int(row["Cycles"]),
                        int(row[" Retired Memory Instructions"]),
                        int(row[" Data Cache Misses"]),
                    )
                    benchmark_ipc = metrics.ipc

            benchmark_ipcs.append(benchmark_ipc / benchmark.get_metrics().ipc)

            print("-------------- %s ----------------" % benchmark_entry["name"])

            microbenchmark_ipc = 0.0
            for microbenchmark in microbenchmarks:
                weight = weight_of_microbenchmark(benchmark, microbenchmark)
                microbenchmark_ipc += weight * microbenchmark.metrics.ipc
                print(
                    "=> Microbenchmark (function %s, invocation %d)"
                    % (microbenchmark.function_id, microbenchmark.invocation_id)
                )
                print("Microbenchmark weight: %f" % weight)
                print("Microbenchmark IPC: %f" % microbenchmark.metrics.ipc)
                print(
                    "Microbenchmark weighted IPC: %f"
                    % (weight * microbenchmark.metrics.ipc)
                )

            print("Micro IPC avg: %f" % microbenchmark_ipc)
            print("Bench IPC avg: %f" % benchmark.get_metrics().ipc)

            micro_ipcs.append(microbenchmark_ipc / benchmark.get_metrics().ipc)

            # excluding_ipcs.append(benchmark.get_metrics().ipc)
            excluding_ipcs.append(1)

    chart_data = {
        "Benchmark": benchmark_ipcs,
        "Benchmark (excl. others)": excluding_ipcs,
        "Microbenchmarks (weighted)": micro_ipcs,
    }

    fig, ax = plt.subplots()
    fig.set_size_inches(8.27, 4)
    ax.set_xticks(range(len(labels)))
    ax.set_xticklabels(labels)
    ax.set_xlabel("Benchmark")
    ax.set_ylabel("Normalized IPC")
    ax.set_title("Summary of benchmark & microbenchmark IPCs")
    ax.grid(axis="y")
    bar_plot(ax, chart_data, total_width=0.8, single_width=0.9)
    plt.show()

    """
    width = 0.3

    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.set_xticks(range(len(labels)))
    ax.set_xticklabels(labels)
    ax.set_xlabel('Benchmark')
    ax.set_ylabel('IPC')
    ax.set_title('Summary of benchmark IPC and weighted microbenchmark IPC')

    ser1 = ax.bar(np.arange(len(benchmark_ipcs)) - width, benchmark_ipcs, width=width)
    ser2 = ax.bar(np.arange(len(micro_ipcs)), micro_ipcs, width=width)
    ser3 = ax.bar(np.arange(len(excluding_ipcs)) + width, excluding_ipcs, width=width)
    ax.legend( (ser1[0], ser2[0], ser3[0]), ('Benchmark', 'Microbenchmarks (weighted)', 'Benchmark (excl. others)') )
    plt.show()
    """
