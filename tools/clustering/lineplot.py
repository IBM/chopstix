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
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import sys
from csv import DictReader
import os
import argparse
import re
from src.clustering import dbscan, ClusteringInformation
from src.misc import chop_print
from src.perfmetrics import (
    PerformanceMetrics,
    Benchmark,
    load_invocations_from_file,
    Microbenchmark,
    Function,
    weight_of_microbenchmark,
)
import math

plt.rcParams.update({"font.size": 10})


def new_page(pdf):
    global subfigs, fig, plots_current_page

    if fig != None:
        pdf.savefig(fig)

    fig = plt.figure(figsize=(8.268, 11.693))
    subfigs = fig.subfigures(3, 1, wspace=0.2, hspace=0.5)
    plots_current_page = 0


def get_next_fig(pdf):
    global plots_current_page

    if plots_current_page == 0:
        new_page(pdf)

    current_fig = subfigs[plots_current_page]

    plots_current_page += 1
    plots_current_page %= 3

    return current_fig


def gen_func_pie(benchmark_name, pdf, benchmark):
    fig = get_next_fig(pdf)

    ax = fig.subplots(1, 1)
    ax.set_title(
        "%s weight distribution per function" % benchmark_name, size="x-large"
    )

    labels = []
    percentages = []

    percentage_others = 100.0

    for name in benchmark.functions:
        function = benchmark.functions[name]
        labels.append(name)
        percentage = function.percentage_samples * 100.0
        percentages.append(percentage)
        percentage_others -= percentage

    labels.append("Others")
    percentages.append(max(percentage_others, 0))

    ax.pie(
        percentages,
        labels=labels,
        autopct="%1.1f%%",
        startangle=90,
        counterclock=False,
    )
    ax.axis(
        "equal"
    )  # Equal aspect ratio ensures that pie is drawn as a circle.


def gen_ipc_comparison(
    benchmark_name, pdf, global_ipc, benchmark, microbenchmarks
):
    fig = get_next_fig(pdf)

    ax = fig.subplots(1, 1)
    ax.set_title(
        "%s bencmark & microbenchmark IPC comparison" % benchmark_name,
        size="x-large",
    )

    bench_ipc = global_ipc
    bench_ipc_handled = benchmark.get_metrics().ipc
    micro_ipc = 0

    pairs = []
    for microbenchmark in microbenchmarks:
        weight = weight_of_microbenchmark(benchmark, microbenchmark)
        micro_ipc += weight * microbenchmark.metrics.ipc
        pairs.append(
            (
                weight,
                microbenchmark.metrics.ipc,
                microbenchmark.function_id,
                microbenchmark.invocation_id,
            )
        )

    pairs.sort(key=lambda x: x[0], reverse=True)

    print("Top 5 microbenchmarks")
    for i in range(min(5, len(pairs))):
        print("Weight: %f; IPC: %f; Function: %s; Invocation: %d" % pairs[i])

    ax.bar(
        [
            "Benchmark IPC",
            'Benchmark IPC\nExcluding "Others"',
            "Microbenchmark\nweigthed IPC",
        ],
        [bench_ipc, bench_ipc_handled, micro_ipc],
    )

    box = ax.get_position()
    factor = 0.9
    ax.set_position(
        [
            box.x0,
            box.y0 + box.height * (1 - factor),
            box.width,
            box.height * factor,
        ]
    )
    ax.grid(True)


def gen_micro_pie(benchmark_name, pdf, benchmark, microbenchmarks):
    fig = get_next_fig(pdf)

    ax = fig.subplots(1, 1)
    ax.set_title(
        "%s weight distribution per microbenchmark" % benchmark_name,
        size="x-large",
    )

    labels = []
    percentages = []

    percentage_others = 100.0

    for micro in microbenchmarks:
        labels.append(
            "%s (Invocation %s)" % (micro.function_id, micro.invocation_id)
        )
        function = benchmark.get_function(micro.function_id)
        weight = function.get_weight_of_invocation(micro.invocation_id) * 100.0
        percentages.append(weight)
        percentage_others -= weight

    labels.append("Missing")
    percentages.append(max(percentage_others, 0))

    ax.pie(
        percentages,
        labels=labels,
        autopct="%1.1f%%",
        startangle=90,
        counterclock=False,
    )
    ax.axis(
        "equal"
    )  # Equal aspect ratio ensures that pie is drawn as a circle.


def gen_plots(
    benchmark_name, pdf, function_name, function, microbenchmarks, micro_path
):
    fig = get_next_fig(pdf)

    axes = fig.subplots(1, 2)
    ax_ipc = axes[0]
    ax_instructions = axes[1]

    linewidth = 1

    benchmark_metrics = function.get_invocation_metrics()
    num_data_points = min(len(benchmark_metrics), 2000)
    x_data = range(
        0,
        len(benchmark_metrics),
        math.floor(len(benchmark_metrics) / num_data_points),
    )

    marker = None if len(benchmark_metrics) > 10 else "D"

    ax_ipc.plot(
        x_data,
        [benchmark_metrics[i].ipc for i in x_data],
        label="Benchmark",
        linewidth=linewidth,
        linestyle="solid",
        zorder=10,
        marker=marker,
    )
    ax_instructions.plot(
        x_data,
        [benchmark_metrics[i].instructions for i in x_data],
        label="Benchmark",
        linewidth=linewidth,
        linestyle="solid",
        zorder=10,
        marker=marker,
    )

    for micro in microbenchmarks:
        label = "Invocation %d" % micro.invocation_id
        metrics = load_metrics(micro_path[micro.invocation_id])
        assert len(metrics) > 0
        ax_ipc.plot(
            x_data,
            [metrics[i].ipc for i in x_data],
            label=label,
            linewidth=linewidth,
            linestyle="dashed",
            zorder=1,
            marker=marker,
        )
        ax_instructions.plot(
            x_data,
            [metrics[i].instructions for i in x_data],
            label=label,
            linewidth=linewidth,
            linestyle="dashed",
            zorder=1,
            marker=marker,
        )

    ax_ipc.set_title("Instructions Per Clock")
    ax_instructions.set_title("Retired Instructions")

    fig.suptitle(
        "%s function %s (weight: %0.2f%%)"
        % (benchmark_name, function_name, function.percentage_samples * 100.0),
        size="x-large",
    )

    ax_ipc.ticklabel_format(axis="both", style="sci")
    ax_instructions.ticklabel_format(axis="both", style="sci")

    ax_ipc.grid(True)
    ax_instructions.grid(True)

    # Get the lengend handles and labels
    handles, labels = ax_ipc.get_legend_handles_labels()

    # Shrink the subplots to make room for the legend
    factor = 0.6
    box = ax_ipc.get_position()
    ax_ipc.set_position(
        [
            box.x0,
            box.y0 + box.height * (1 - factor),
            box.width,
            box.height * factor,
        ]
    )
    box = ax_instructions.get_position()
    ax_instructions.set_position(
        [
            box.x0,
            box.y0 + box.height * (1 - factor),
            box.width,
            box.height * factor,
        ]
    )
    # Make the legend
    ax_ipc.legend(
        handles, labels, bbox_to_anchor=(0, -0.05, 2.2, -0.15), loc=9, ncol=4
    )


def load_function_data(results_path, benchmark):
    with open(os.path.join(results_path, "functions.txt")) as file:
        lines = file.readlines()[1:]

        for line in lines:
            _, name, _, _, weight, _, _ = line.split("\t")

            weight = float(weight[:-1]) / 100

            cluster_info = ClusteringInformation.from_file(
                os.path.join(results_path, name + "_cluster.json")
            )
            invocations = load_invocations_from_file(
                os.path.join(results_path, name, "benchmark.csv")
            )

            function = Function(cluster_info, invocations, weight)

            benchmark.add_function(name, function)


def load_metrics(file):
    metrics = []
    with open(file) as f:
        reader = DictReader(f)
        index = 0
        for row in reader:
            metrics.append(
                PerformanceMetrics(
                    int(row[" Time Elapsed (us)"]),
                    int(row[" Retired Instructions"]),
                    int(row["Cycles"]),
                    int(row[" Retired Memory Instructions"]),
                    int(row[" Data Cache Misses"]),
                )
            )
    return metrics


def main():
    parser = argparse.ArgumentParser(
        description="Generate line plots from benchmark/microbenchmark evaluation"
    )
    parser.add_argument("root_dir")
    parser.add_argument("benchmark_name")
    args = parser.parse_args()

    regex = re.compile("[a-zA-Z0-9_]*_([0-9]*)#.*\.csv")

    plots_current_page = 0
    fig = None
    subfigs = None

    root_dir = args.root_dir
    benchmark_name = args.benchmark_name
    benchmark = Benchmark()

    print("Loading benchmark performance data...")

    load_function_data(root_dir, benchmark)

    print("Loading microbenchmark performance data...")

    microbenchmarks = []
    micro_path = {}
    for function in benchmark.functions:
        function_results_path = os.path.join(root_dir, function)

        micro_path[function] = {}
        print(
            "Loading global data for microbenchmarks of function %s" % function
        )
        for file in os.scandir(function_results_path):
            if not file.name.startswith("benchmark") and file.name.endswith(
                "_global.csv"
            ):
                match = regex.match(file.name)
                assert match
                invocation = int(match.group(1))
                metrics = load_metrics(file.path)
                assert len(metrics) == 1
                microbenchmarks.append(
                    Microbenchmark(function, invocation, metrics[0])
                )

        for file in os.scandir(function_results_path):
            if not file.name.startswith(
                "benchmark"
            ) and not file.name.endswith("_global.csv"):
                match = regex.match(file.name)
                assert match
                invocation = int(match.group(1))
                micro_path[function][invocation] = file.path

    metrics = load_metrics(os.path.join(root_dir, "benchmark_global.csv"))
    assert len(metrics) == 1

    print("Data loaded. Generating plots.")

    with PdfPages("report.pdf") as pp:
        # benchmark.normalize_sample_percentages()
        gen_func_pie(benchmark_name, pp, benchmark)
        gen_micro_pie(benchmark_name, pp, benchmark, microbenchmarks)
        gen_ipc_comparison(
            benchmark_name, pp, metrics[0].ipc, benchmark, microbenchmarks
        )

        for function_id in benchmark.functions:
            function = benchmark.functions[function_id]
            gen_plots(
                benchmark_name,
                pp,
                function_id,
                function,
                [
                    micro
                    for micro in microbenchmarks
                    if micro.function_id == function_id
                ],
                micro_path[function_id],
            )

        if fig != None:
            pp.savefig(fig)


if __name__ == "__main__":
    main()
