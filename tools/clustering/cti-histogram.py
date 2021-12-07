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
