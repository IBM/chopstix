#!/bin/python3

import argparse
import sys
import random

from src.trace import Trace
from src.clustering import dbscan, estimate_dbscan_epsilon, dbscan_ipc
from src.perfmetrics import load_invocations_from_file

def use_trace(args):
    for trace_file in args.trace_files:
        print("Analyzing trace %s" % trace_file)

        trace = Trace(trace_file, args.num_threads)

        if args.max_memory != None:
            needed = trace.estimate_needed_memory() / (1024**2)
            if needed > args.max_memory:
                print("Need more memory than allowed to process trace: %d out of %d" % (needed, args.max_memory))
                continue

        print("Clustering %d invocations (%d sets)" % (trace.get_invocation_count(), trace.get_invocation_set_count()))

        epsilon = args.epsilon
        if epsilon == None:
            epsilon = estimate_dbscan_epsilon(trace, args.coverage)

        cluster_info = dbscan(trace, epsilon)

        cluster_info.to_file(args.output)

def use_ipc(args):
    invocations = load_invocations_from_file(args.perf_csv)
    cluster_info = dbscan_ipc(invocations, args.epsilon)
    cluster_info.to_file(args.output)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Cluster invocations using DBScan")

    subparsers = parser.add_subparsers()
    subparsers.required = True

    parser_trace = subparsers.add_parser('trace', add_help=False,
            description="Cluster using traces captured by ChopStiX")
    parser_trace.set_defaults(function=use_trace)
    parser_trace.add_argument('trace_files', nargs='+', help='Traces to cluster.')
    parser_trace.add_argument('--num-threads', '-n', type=int, help='Number of threads to use during the clustering')
    parser_trace.add_argument('--max-memory', type=int, help="Don't use more than this amount of memory during clustering")
    parser_trace.add_argument('--epsilon', '-e', type=float, help='Epsilon parameter to pass to the DBSCAN clusterer')
    parser_trace.add_argument('--output', '-o', type=str, default='clusters.json', help="Output file")

    parser_ipc = subparsers.add_parser('ipc', add_help=False,
            description="Cluster using measured IPC per invocation")
    parser_ipc.set_defaults(function=use_ipc)
    parser_ipc.add_argument('perf_csv', help='Captured performance profile.')
    parser_ipc.add_argument('--epsilon', '-e', type=float, help='Epsilon parameter to pass to the DBSCAN clusterer', default=0.01)
    parser_ipc.add_argument('--output', '-o', type=str, default='clusters.json', help="Output file")

    args = parser.parse_args()
    args.function(args)
