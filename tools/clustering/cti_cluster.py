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
import argparse
import sys
import random

from src.trace import Trace
from src.clustering import (
    dbscan,
    estimate_dbscan_epsilon,
    dbscan_ipc,
    dbscan_ipc_instr,
    dbscan_instr,
    brute_force_2d_density,
)
from src.perfmetrics import load_invocations_from_file
from src.misc import chop_print


def use_trace(args):

    if args.num_threads < 1:
        chop_print("ERROR: Number of threads should be >= 1")
        exit(1)

    if args.max_memory != None:
        if args.max_memory < 1:
            chop_print("ERROR: Max memory should be >= 1")
            exit(1)

    if args.epsilon != None:
        if args.epsilon <= 0:
            chop_print("ERROR: Epsilon should be >= 0")
            exit(1)

    if not (0 < args.coverage <= 1):
        chop_print("ERROR: Coverage should be in (0,1] range")
        exit(1)

    for trace_file in args.trace_files:
        chop_print("Analyzing trace %s" % trace_file)

        trace = Trace(trace_file, args.num_threads)

        if args.max_memory != None:
            needed = trace.estimate_needed_memory() / (1024**2)
            if needed > args.max_memory:
                chop_print(
                    "Need more memory than allowed to process trace: %d out of %d"
                    % (needed, args.max_memory)
                )
                continue

        chop_print(
            "Clustering %d invocations (%d sets)"
            % (trace.get_invocation_count(), trace.get_invocation_set_count())
        )

        epsilon = args.epsilon
        if epsilon == None:
            epsilon = estimate_dbscan_epsilon(trace, args.coverage)

        cluster_info = dbscan(trace, epsilon)

        cluster_info.to_file(args.output)
        chop_print("Results written to %s" % args.output)


def use_ipc_dbscan(args):

    if args.epsilon != None:
        if args.epsilon <= 0:
            chop_print("ERROR: Epsilon should be >= 0")
            exit(1)

    invocations = load_invocations_from_file(args.perf_invok_csv)
    cluster_info = dbscan_ipc(
        invocations,
        args.epsilon,
        plot_path=args.plot_path,
        benchmark_name=args.benchmark_name,
        function_name=args.function_name,
    )
    cluster_info.to_file(args.output)
    chop_print("Results written to %s" % args.output)


def use_ipc_instr_dbscan(args):

    if args.epsilon != None:
        if args.epsilon <= 0:
            chop_print("ERROR: Epsilon should be >= 0")
            exit(1)

    invocations = load_invocations_from_file(args.perf_invok_csv)
    cluster_info = dbscan_ipc_instr(
        invocations,
        args.epsilon,
        plot_path=args.plot_path,
        benchmark_name=args.benchmark_name,
        function_name=args.function_name,
    )
    cluster_info.to_file(args.output)
    chop_print("Results written to %s" % args.output)


def use_instr_dbscan(args):

    if not (0 < args.minimum_distance_percentage <= 100):
        chop_print("ERROR: minimum_distance_percentage should be in (0,100] range")
        exit(1)

    if not (0 < args.maximum_distance_percentage <= 100):
        chop_print("ERROR: maximum_distance_percentage should be in (0,100] range")
        exit(1)

    if not (0 < args.minimum_cluster_size_percentage <= 100):
        chop_print("ERROR: minimum_cluster_size_percentage should be in (0,100] range")
        exit(1)

    if args.uniq_value_threshold <= 0:
        chop_print("ERROR: uniq_value_threshold should be in > 0")
        exit(1)

    if args.minimum_cluster_count < 0:
        chop_print("ERROR: minimum_cluster_count should be in >= 0")
        exit(1)

    if args.maximum_cluster_count <= 0:
        chop_print("ERROR: maximum_cluster_count should be in > 0")
        exit(1)

    if args.maximum_cluster_count <= args.minimum_cluster_count:
        chop_print(
            "ERROR: maximum_cluster_count should be > than minimum_cluster_count"
        )
        exit(1)

    if args.maximum_distance_percentage <= args.minimum_distance_percentage:
        chop_print(
            "ERROR: maximum_distance_percentage should be > than minimum_distance_percentage"
        )
        exit(1)

    invocations = load_invocations_from_file(args.perf_invok_csv)
    cluster_info = dbscan_instr(
        invocations,
        plot_path=args.plot_path,
        minimum_distance_percentage=args.minimum_distance_percentage,
        uniq_value_threshold=args.uniq_value_threshold,
        maximum_distance_percentage=args.maximum_distance_percentage,
        minimum_cluster_size_percentage=args.minimum_cluster_size_percentage,
        minimum_cluster_count=args.minimum_cluster_count,
        maximum_cluster_count=args.maximum_cluster_count,
        benchmark_name=args.benchmark_name,
        function_name=args.function_name,
    )
    cluster_info.to_file(args.output)
    chop_print("Results written to %s" % args.output)


def use_instr_ipc_2d_density(args):

    if args.max_clusters <= 0:
        chop_print("ERROR: max_clusters should be in > 0")
        exit(1)

    if not (0 < args.min_clusters_weight_percentage <= 100):
        chop_print("ERROR: min_clusters_weight_percentage should be in (0,100] range")
        exit(1)

    if not (0 < args.target_coverage_percentage <= 100):
        chop_print("ERROR: target_coverage_percentage should be in (0,100] range")
        exit(1)

    if not (0 < args.outlier_percent <= 100):
        chop_print("ERROR: outlier_percent should be in (0,100] range")
        exit(1)

    if args.outlier_minsize_threshold <= 0:
        chop_print("ERROR: outlier_minsize_threshold should be in > 0")
        exit(1)

    if not (0 < args.minimum_granularity_percentage <= 100):
        chop_print("ERROR: minimum_granularity_percentage should be in (0,100] range")
        exit(1)

    if not (0 < args.granularity_step_percentage <= 100):
        chop_print("ERROR: granularity_step_percentage should be in (0,100] range")
        exit(1)

    invocations = load_invocations_from_file(args.perf_invok_csv)
    cluster_info = brute_force_2d_density(
        invocations,
        None,
        plot_path=args.plot_path,
        max_clusters=args.max_clusters,
        min_clusters_weight_percentage=args.min_clusters_weight_percentage,
        target_coverage_percentage=args.target_coverage_percentage,
        outlier_percent=args.outlier_percent,
        outlier_minsize_threshold=args.outlier_minsize_threshold,
        minimum_granularity_percentage=args.minimum_granularity_percentage,
        granularity_step_percentage=args.granularity_step_percentage,
        benchmark_name=args.benchmark_name,
        function_name=args.function_name,
    )
    cluster_info.to_file(args.output)
    chop_print("Results written to %s" % args.output)


def main():

    parser = argparse.ArgumentParser(description="Cluster invocations")

    subparsers = parser.add_subparsers(dest="command")
    subparsers.required = True

    subparser = subparsers.add_parser(
        "trace", description="Cluster using memory access traces generated by ChopStiX"
    )
    subparser.set_defaults(function=use_trace)
    subparser.add_argument("trace_files", nargs="+", help="Input traces")
    subparser.add_argument(
        "--num-threads",
        "-n",
        type=int,
        help="Number of threads to use during the clustering",
        default=1,
    )
    subparser.add_argument(
        "--max-memory",
        type=int,
        help="Don't use more than this amount of memory during clustering",
    )
    subparser.add_argument(
        "--epsilon",
        "-e",
        type=float,
        help="Epsilon parameter to pass to the DBSCAN clusterer",
    )
    subparser.add_argument(
        "--coverage",
        help="Clustering coverage, used to estimate epsilon if not provided",
        type=float,
        default=0.9,
    )
    subparser.add_argument(
        "--output", "-o", type=str, default="clusters.json", help="Output file"
    )

    subparser = subparsers.add_parser(
        "ipc",
        description="Cluster using IPC per invocation traces generate by ChopStiX perf-invok support tool",
    )
    subparser.set_defaults(function=use_ipc_dbscan)
    subparser.add_argument("perf-invok-csv", help="Input trace")
    subparser.add_argument(
        "--epsilon",
        "-e",
        type=float,
        help="Epsilon parameter to pass to the DBSCAN clusterer",
        default=0.01,
    )
    subparser.add_argument(
        "--output", "-o", type=str, default="clusters.json", help="Output file"
    )
    subparser.add_argument(
        "--plot-path",
        "-p",
        type=str,
        default=None,
        help="Plot path. If defined, generated plots will have this prefix",
    )
    subparser.add_argument(
        "--benchmark-name",
        type=str,
        default="Unk",
        help="Benchmark name to be used if plots are generated",
    )
    subparser.add_argument(
        "--function-name",
        type=str,
        default="Unk",
        help="Function name to be used if plots are generated",
    )

    subparser = subparsers.add_parser(
        "ipc-instr",
        description="Cluster using IPC and retired instructions per invocation traces generate by ChopStiX perf-invok support tool",
    )
    subparser.set_defaults(function=use_ipc_instr_dbscan)
    subparser.add_argument("perf-invok-csv", help="Input trace")
    subparser.add_argument(
        "--epsilon",
        "-e",
        type=float,
        help="Epsilon parameter to pass to the DBSCAN clusterer",
        default=0.01,
    )
    subparser.add_argument(
        "--output", "-o", type=str, default="clusters.json", help="Output file"
    )
    subparser.add_argument(
        "--plot-path",
        "-p",
        type=str,
        default=None,
        help="Plot path. If defined, generated plots will have this prefix",
    )
    subparser.add_argument(
        "--benchmark-name",
        type=str,
        default="Unk",
        help="Benchmark name to be used if plots are generated",
    )
    subparser.add_argument(
        "--function-name",
        type=str,
        default="Unk",
        help="Function name to be used if plots are generated",
    )

    subparser = subparsers.add_parser(
        "instr",
        description="Cluster using retired instructions per invocation traces generate by ChopStiX perf-invok support tool",
    )
    subparser.set_defaults(function=use_instr_dbscan)
    subparser.add_argument("perf-invok-csv", help="Input trace")
    subparser.add_argument(
        "--uniq-value-threshold",
        type=int,
        help="If the total of unique values is below this threshold, each value defines a cluster",
        default=50,
    )
    subparser.add_argument(
        "--minimum-distance-percentage",
        type=float,
        help="If the normalized distance between two points is below this percentage, the points are considered equal (filtered). E.g. if 1 is defined, the points with less of 1%% variability are considered equal. The percentage of variability is computed with respect to the entire range of values seen",
        default=0.1,
    )
    subparser.add_argument(
        "--maximum-distance-percentage",
        type=float,
        help="If the normalized distance between two points is above this percentage, the points are considered in a different cluster. E.g. if 5 is defined, the points with less that 5%% variability are considered in the same cluster. The percentage of variability is computed with respect to the entire range of values seen. For instance if values seen are in range [1,2000], and 5%% is specified, the maximum distance within a cluster is 100",
        default=5,
    )
    subparser.add_argument(
        "--minimum-cluster-size-percentage",
        type=float,
        help="Minimum number of points within a cluster in percentage over the entire set of points. E.g. if 1 is defined, the clusters at least will contain 1%% of the points",
        default=1,
    )
    subparser.add_argument(
        "--minimum-cluster-count",
        type=int,
        help="Minimum number of clusters to define (goal, not always possible)",
        default=10,
    )
    subparser.add_argument(
        "--maximum-cluster-count",
        type=int,
        help="Maximum number of clusters to define (goal, not always possible)",
        default=50,
    )
    subparser.add_argument(
        "--output", "-o", type=str, default="clusters.json", help="Output file"
    )
    subparser.add_argument(
        "--plot-path",
        "-p",
        type=str,
        default=None,
        help="Plot path. If defined, generated plots will have this prefix",
    )
    subparser.add_argument(
        "--benchmark-name",
        type=str,
        default="Unk",
        help="Benchmark name to be used if plots are generated",
    )
    subparser.add_argument(
        "--function-name",
        type=str,
        default="Unk",
        help="Function name to be used if plots are generated",
    )

    subparser = subparsers.add_parser(
        "instr_ipc_density",
        description="Cluster using measured retired instructions and IPC per invocation using 2D density",
    )
    subparser.set_defaults(function=use_instr_ipc_2d_density)
    subparser.add_argument("perf-invok-csv", help="Input trace")
    subparser.add_argument(
        "--max-clusters",
        help="Maximum number of clusters to define",
        type=int,
        default=20,
    )
    subparser.add_argument(
        "--min-clusters-weight-percentage",
        help="Minimum weight in %% of total # of instructions to consider a cluster",
        type=float,
        default=1,
    )
    subparser.add_argument(
        "--target-coverage-percentage",
        help="Minimum target %% of total # of instructions",
        type=float,
        default=90,
    )
    subparser.add_argument(
        "--outlier-percent",
        help="Top/Bottom percentage of data points to remove from the clustering algorithm. E.g. if 1%% is specified, the 2%% of the samples will be removed. The bottom 1%% n # of instructions and the top 1%% in number of instructions",
        type=float,
        default=1,
    )
    subparser.add_argument(
        "--outlier-minsize-threshold",
        help="Only apply --outlier-percent if the number of samples is above this value",
        type=int,
        default=1000,
    )
    subparser.add_argument(
        "--minimum-granularity-percentage",
        help="Initial granularity in the 2D cluster grid. If 2%% is defined, the 2D grid (#instructions vs IPC) will be divided in a 50x50 grid. So each, square in the grid is 2%% of the range of each metric. Setting it very low can increase the number of clusters defined but it also increases the runtime. Decrease it if the number of clusters defined is too low.",
        type=float,
        default=1,
    )
    subparser.add_argument(
        "--granularity-step-percentage",
        help="Initial increase step percentage when looking for a solution. This parameter controls how fast the algorithm converges to a solution trading off granularity. To maintain the granularity it should be set equal to --minimum-granularity-percentage",
        type=float,
        default=1,
    )
    subparser.add_argument(
        "--output", "-o", type=str, default="clusters.json", help="Output file"
    )
    subparser.add_argument(
        "--plot-path",
        "-p",
        type=str,
        default=None,
        help="Plot path. If defined, generated plots will have this prefix",
    )
    subparser.add_argument(
        "--benchmark-name",
        type=str,
        default="Unk",
        help="Benchmark name to be used if plots are generated",
    )
    subparser.add_argument(
        "--function-name",
        type=str,
        default="Unk",
        help="Function name to be used if plots are generated",
    )

    args = parser.parse_args()
    args = vars(args)
    for k, v in args.copy().items():
        args[k.replace("-", "_")] = v
    args = argparse.Namespace(**args)
    args.function(args)
    exit(0)


if __name__ == "__main__":
    main()
