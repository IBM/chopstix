# Example workflow 

This document explains how to **_generate a microbenchmark from the most
representative function invocation of the hottest function of a benchmark_**.
To do so, the following high level steps are required:

1. Profile the benchmark to get the _hottest_ function of the benchmark.
2. Profile the function of interest to get the behavior per invocation.
3. Select the invocation of interest based on clustering analysis.
4. Trace the invocation of interest to generate the corresponding Microprobe
   test files (MPTs).
5. Generate a microbenchmark from the MPTs.
6. (Optional) Trace microbenchmark detailed memory access pattern and
   regenerate the microbenchmark using the  MPTs and detailed memory
   access pattern.

The rest of the decoument provides details on each of the steps.

## Bechmark profiling

## Function profiling

## Invocation selection

## Invocation tracing

## Microbenchmark generation

## Microbenchmark 
