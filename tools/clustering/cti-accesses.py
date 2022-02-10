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
import matplotlib.pyplot as plt
import numpy as np

from src.trace import Trace
from src.clustering import dbscan, estimate_dbscan_epsilon
from src.misc import chop_print

parser = argparse.ArgumentParser(description="ChopStix trace visualization tool.")
parser.add_argument("trace_file", help="Input trace bin file to process.")
parser.add_argument(
    "--clustered",
    "-c",
    help="Cluster invocations according the the --epsilon and --coverage parameters.",
    action="store_true",
)
parser.add_argument(
    "--epsilon",
    "-e",
    help="Epsilon parameter for the DBSCAN clustering algorithm. If not provided, it is automatically estimated using the --coverage parameter.",
    type=float,
)
parser.add_argument(
    "--coverage",
    help="Clustering coverae, used to estimate epsilon if not provided.",
    type=float,
    default=0.9,
)
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
# plt.legend(loc='upper left')
plt.grid(True)
plt.show()
