import csv

class PerformanceMetrics:
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
