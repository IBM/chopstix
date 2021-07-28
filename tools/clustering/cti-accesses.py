#!/bin/python3

import argparse
import matplotlib.pyplot as plt
import numpy as np

from src.trace import Trace
from src.clustering import dbscan, estimate_dbscan_epsilon

parser = argparse.ArgumentParser(description="Inspect ChopStix traces")
parser.add_argument('trace_file')
parser.add_argument('--clustered', '-c', action='store_true')
parser.add_argument('--epsilon', '-e', type=float)
parser.add_argument('--coverage', type=float, default=0.9)
args = parser.parse_args()

trace = Trace(args.trace_file)

print("Plotting %d invocations" % trace.get_invocation_count())

x = []
y = []
for n in range(trace.get_invocation_count()):
    invocation = trace.invocations[n]
    for page in invocation.pages:
        x.append(n)
        y.append(page)

if args.clustered:
    epsilon = args.epsilon
    if epsilon == None:
        epsilon = estimate_dbscan_epsilon(trace, args.coverage)

    cluster_indices = dbscan(trace, epsilon)
    colors = []
    for i in range(trace.get_invocation_count()):
        index = cluster_indices[i]
        for j in range(len(trace.invocations[i].pages)):
            colors.append(index if index > -1 else 7)

    plt.scatter(x, y, label="Page Access", c=colors)
else:
    plt.scatter(x, y, label="Page Access")

plt.xlabel("Invocation ID")
plt.ylabel("Page Address")
#plt.legend(loc='upper left')
plt.grid(True)
plt.show()
