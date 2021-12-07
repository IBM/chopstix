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

class PerformanceMetrics:
    __slots__ = 'time', 'instructions', 'cycles', 'mem_instrs', 'misses'

    def __init__(self, time, instructions, cycles, mem_instrs, misses):
        self.time = time
        self.instructions = instructions
        self.cycles = cycles
        self.mem_instrs = mem_instrs
        self.misses = misses

    @property
    def ipc(self):
        return self.instructions / self.cycles

    @property
    def missrate(self):
        return self.misses / self.mem_instrs

def aggregate_metrics(metrics_list):
    aggregated = PerformanceMetrics(0, 0, 0, 0, 0)

    for metrics in metrics_list:
        aggregated.time += metrics.time
        aggregated.instructions += metrics.instructions
        aggregated.cycles += metrics.cycles
        aggregated.mem_instrs += metrics.mem_instrs
        aggregated.misses += metrics.misses

    return aggregated

def average_metrics(self, metrics):
        self.averageCount += 1
        norm = 1 - (1 / self.averageCount)

        self.time = self.time * norm + metrics.time
        self.instructions = self.instructions * norm + metrics.instructions
        self.cycles = self.cycles * norm + metrics.cycles
        self.mem_instrs = self.mem_instrs * norm + metrics.mem_instrs
        self.misses = self.misses * norm + metrics.misses

        return self

class Invocation:
    __slots__ = 'index', 'metrics'

    def __init__(self, index, metrics):
        self.index = index
        self.metrics = metrics

def load_invocations_from_file(path):
    invocations = []

    with open(path) as file:
        reader = csv.DictReader(file)
        index = 0
        for row in reader:
            invocation = Invocation(index, PerformanceMetrics(
                    int(row[' Time Elapsed (us)']),
                    int(row[' Retired Instructions']),
                    int(row['Cycles']),
                    int(row[' Retired Memory Instructions']),
                    int(row[' Data Cache Misses'])
                ))
            index += 1
            invocations.append(invocation)

    return invocations

class InvocationSet:

    __slots__ = '_invocations', '_invocation_ids'

    def __init__(self, invocations, invocation_ids):
        self._invocations = invocations
        self._invocation_ids = invocation_ids

    def get_metrics(self, avg = False):
        return aggregate_metrics([invocation.metrics for invocation in self._invocations])

    def __contains__(self, invocation_id):
        return invocation_id in self._invocation_ids

class Cluster:

    __slots__ = 'invocations'

    def __init__(self):
        self.invocations = []

    def add_invocation(self, invocation):
        self.invocations.append(invocation)

    def get_metrics(self, avg = False):
        return aggregate_metrics([invocation.metrics for invocation in self.invocations])

class Function:

    __slots__ = 'clusters', 'noise_invocation_sets', 'cluster_info', 'percentage_samples', 'invocations'

    def __init__(self, cluster_info, invocations, percentage_samples):
        self.clusters = {}
        self.noise_invocation_sets = []
        self.cluster_info = cluster_info
        self.percentage_samples = percentage_samples
        self.invocations = invocations

        for cluster_id in range(cluster_info.get_cluster_count()):
            cluster = Cluster()
            for invocation_id in cluster_info.get_all_invocations_in_cluster(cluster_id):
                cluster.add_invocation(invocations[invocation_id])

            self.clusters[cluster_id] = cluster

        for invocation_set in cluster_info.get_all_noise_invocation_sets():
            invocation_ids = [invocation_id for invocation_id in cluster_info.get_all_invocations_in_invocation_set(invocation_set)]
            _invocations = [invocations[invocation_id] for invocation_id in invocation_ids]
            self.noise_invocation_sets.append(InvocationSet(_invocations, invocation_ids))

    def get_cluster(self, cluster_id):
        return self.clusters[cluster_id]

    def get_cluster_id_for_invocation(self, invocation_id):
        cluster_id = self.cluster_info.get_cluster_id_for_invocation(invocation_id)
        return cluster_id
        #return self.clusters[cluster_id]

    def get_metrics(self, avg = False):
        cluster_metrics = aggregate_metrics([self.clusters[cluster_id].get_metrics(avg) for cluster_id in self.clusters])
        noise_metrics = aggregate_metrics([invocation_set.get_metrics(avg) for invocation_set in self.noise_invocation_sets])

        return aggregate_metrics([cluster_metrics, noise_metrics])

    def _get_weight_of_noise_invocation(self, invocation_id, avg = False):
        for invocation_set in self.noise_invocation_sets:
            if invocation_id in invocation_set:
                return invocation_set.get_metrics(avg)
        return None

    def get_weight_of_invocation(self, invocation_id):
        cluster_id = self.get_cluster_id_for_invocation(invocation_id)

        if cluster_id >= 0:
            cluster_time = self.get_cluster(cluster_id).get_metrics().time
        else: # Noise point
            metrics = self._get_weight_of_noise_invocation(invocation_id)
            assert metrics != None
            cluster_time = metrics.time

        total_time = self.get_metrics().time

        return (cluster_time / total_time) * self.percentage_samples

    def get_invocation_metrics(self):
        return [invocation.metrics for invocation in self.invocations]

class Benchmark:

    def __init__(self):
        self.functions = {}

    def add_function(self, function_id, function):
        self.functions[function_id] = function

    def get_function(self, function_id):
        return self.functions[function_id]

    def normalize_sample_percentages(self):
        total_samples = 0

        for function_id in self.functions:
            total_samples += self.functions[function_id].percentage_samples

        for function_id in self.functions:
            self.functions[function_id].percentage_samples /= total_samples

    def get_metrics(self, avg = False):
        return aggregate_metrics([self.functions[fid].get_metrics(avg) for fid in self.functions])

class Microbenchmark:
    __slots__ = 'function_id', 'invocation_id', 'metrics', 'invocation_metrics'

    def __init__(self, function_id, invocation_id, metrics):
        self.function_id = function_id
        self.invocation_id = invocation_id
        self.metrics = metrics

    def add_per_invocation_metrics(self, invocation_metrics):
        self.invocation_metrics = invocation_metrics

def weight_of_microbenchmark(benchmark, microbenchmark):
    function = benchmark.get_function(microbenchmark.function_id)
    return function.get_weight_of_invocation(microbenchmark.invocation_id)
