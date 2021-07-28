import numpy as np
from struct import iter_unpack
from multiprocessing import Pool
from math import ceil
import os
import gc

def process_work(trace, start, end, n, distance_function):
    matrix = np.zeros((end - start, n), 'float32')
    for i in range(start, end):
        for j in range(0, n):
            matrix[i-start, j] = distance_function(trace.invocation_sets[i], trace.invocation_sets[j])
    return matrix

class Trace:
    distance_matrix_generated = False

    def __init__(self, filename, dmatrix_nproc = os.cpu_count()):
        print("Parsing trace...")
        f = open(filename, 'rb')
        raw_data = f.read()
        f.close()

        pages = iter_unpack('<q', raw_data)
        self.invocations = []
        current_invocation = None
        for page, in pages:
            if page == -3:
                if current_invocation is not None:
                    current_invocation.generate_pages()
                    self.invocations.append(current_invocation)

                current_invocation = Invocation(len(self.invocations))
            elif page == -1:
                current_subtrace = SubTrace()
            elif page == -2:
                current_invocation.add_subtrace(current_subtrace)
            else:
                current_subtrace.add_page(page)

        if current_invocation is not None:
            current_invocation.generate_pages()
            self.invocations.append(current_invocation)

        if dmatrix_nproc == None:
            dmatrix_nproc = os.cpu_count()

        self.dmatrix_nproc = dmatrix_nproc

        print("Finding distinct invocations...")
        self.generate_invocation_sets()

    def get_subtrace_count(self):
        return sum(len(invocation.subtraces) for invocation in self.invocations)

    def get_invocation_count(self):
        return len(self.invocations)

    def get_invocation_set_count(self):
        return len(self.invocation_sets)

    def generate_invocation_sets(self):
        invocation_sets = {}

        for invocation in self.invocations:

            if invocation.hash in invocation_sets:
                invocation_sets[invocation.hash].add(invocation)
            else:
                invocation_sets[invocation.hash] = InvocationSet([invocation])

        self.invocation_sets = list(invocation_sets.values())

    def get_distance_matrix(self, distance_function):
        if self.distance_matrix_generated == False:
            n = self.get_invocation_set_count()
            nprocs = min(n, self.dmatrix_nproc)

            # Minimize memory usage
            del self.invocations
            gc.collect()

            # Generate distance matrix in parallel
            print("Generating distance matrix with %d threads" % nprocs)
            bs = ceil(n/nprocs)
            pool = Pool(nprocs)
            submatrices = pool.starmap(process_work, ((self, i, min(i + bs, n), n, distance_function) for i in range(0, n, bs)))

            # Concatenate submatrices
            self.distance_matrix = np.bmat(list(map(lambda x: [x], submatrices)))
            self.distance_matrix_generated = True

        return self.distance_matrix

    def estimate_needed_memory(self):
        return (self.get_invocation_set_count()**2) * 8 # 8 bytes per element (double)

class Invocation:
    def __init__(self, index):
        self.id = index
        self.subtraces = []
        self.pages = set()

    def add_subtrace(self, subtrace):
        self.subtraces.append(subtrace)

    def generate_pages(self):
        for trace in self.subtraces:
            for page in trace.pages:
                self.pages.add(page)
        self.hash = hash(tuple(self.pages))

class SubTrace:
    def __init__(self):
        self.pages = []

    def add_page(self, address):
        self.pages.append(address)

# Set of Invocations which use the same memory pages
class InvocationSet:
    def __init__(self, invocations):
        self.invocations = [invocation.id for invocation in invocations]
        self.pages = invocations[0].pages
        self.hash = invocations[0].hash

    def add(self, invocation):
        self.invocations.append(invocation.id)
