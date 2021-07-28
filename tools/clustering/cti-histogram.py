#!/bin/python3

import argparse
import matplotlib.pyplot as plt
import numpy as np
from statistics import mean, stdev

from src.distance import disjoint_sets
from src.trace import Trace

parser = argparse.ArgumentParser(description="Inspect ChopStix traces")
parser.add_argument('trace_file')
parser.add_argument('metric', choices=['count', 'distance', 'nearest_neighbour'])
parser.add_argument('--num-threads', '-n', type=int)
args = parser.parse_args()

trace = Trace(args.trace_file, args.num_threads)

print("Analyzing %d subtraces in %d invocations" % (trace.get_subtrace_count(), trace.get_invocation_count()))

if (args.metric == 'count'):
    page_counts = list(map(lambda x: len(x.pages), trace.invocations))
    print("Page count mean: %f, std. dev.: %f" % (mean(page_counts), stdev(page_counts)))

    print(np.unique(page_counts))
    plt.hist(page_counts, width=0.5)
    plt.show()
elif (args.metric == 'distance'):
    distance_matrix = trace.get_distance_matrix(disjoint_sets)

    distances = []
    for i in range(trace.get_invocation_count()):
        for j in range(i, trace.get_invocation_count()):
            distances.append(distance_matrix[i, j])

    plt.plot(range(len(distances)), np.sort(distances))
    plt.show()
elif args.metric == 'nearest_neighbour':
    distance_matrix = trace.get_distance_matrix(disjoint_sets)

    distances = []
    for i in range(trace.get_invocation_count()):
        distances.append(min([distance_matrix[i, j] for j in range(trace.get_invocation_count()) if j != i]))

    plt.plot(range(trace.get_invocation_count()), np.sort(distances))
    plt.show()
