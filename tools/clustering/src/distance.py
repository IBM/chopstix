import numpy as np
from math import sqrt

def disjoint_sets(subtraceA, subtraceB):
    x = len(subtraceA.pages) - len(subtraceB.pages)
    y = len(np.setxor1d(subtraceA.pages, subtraceB.pages))
    return sqrt(x**2 + y**2)
