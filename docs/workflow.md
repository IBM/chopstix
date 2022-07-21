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

The initial profiling of the benchmark is done using performance counters.
Depending on the performance counter being sampled, the definition of **_hottest_**
changes. It is up to the user to select what type of analysis wants to perform.

For instance, using `CYCLES`, the functions with more execution time be
the _hottest_. If one uses `INSTRUCTIONS`, the functions executing
most of the instructions will be the _hottest_. One can use any other performance
counter available via `perf` interface to focus the analysis on other aspects.
E.g. one can sample the branch miss-prediction counter to focus the analysis
on the functions with poor branch predictions, same for memory misses or
pipeline vector stalls, etc.

In this example, we are going to focus on CYCLES, which at the end of the day,
it is the first type of analysis one performs before going into more detailed
ones. The command to execute will be as follows: 

    chop sample -data CHOPSTIX_DB -events CYCLES -period 100000 -- BINARY ARGUMENTS

where:

- `CHOPSTIX_DB` is the path of the database where all the samples and extra
  information (see following steps) will be stored.
- `-events` specifies the event to be sampled.
` `-period` specidied how often to sample the event. The lower, the better, but
  one should take into account the overheads generated.
- `BINARY` is the binary executable (ELF) to execute.
- `ARGUMENTS` are the necessary arguments fro the `BINARY`.

Note that measurements are done in the real system. Therefore, when sampling
counters which can be affected by other activity on the system, one needs to
minimize the measurement _noise_ by minimizing any other system activity. Also,
can execute the above command N times, and the results will be accumulated so
that noise can be minimized.

Once the sampling is done, one can postprocess all the sampled information
to list which functions got the most samples, i.e. generated most of the events,
and therefore the **_hottest_** function from that performance counter point
of view. To do so, execute the following commands:

    chop disasm -data CHOPSTIX_DB BINARY $(ldd BINARY)
    chop count -data CHOPSTIX_DB
    chop annotate -data CHOPSTIX_DB 
    chop list functions -data CHOPSTIX_DB

The commands above first disassemble the executed binary and the dynamic
libraries used (note the `ldd $BINARY` subcommand). If you already know that
the functions to trace are in the main `BINARY`, there is no need to disassemble
the dynamic libraries (which can be size and time consuming). The samples
are counted and annotated to the control flow graph (CFG) that ChopStiX generated
during the `disasm` step. The final command lists all the functions, sorted
by the number of samples / coverage with respect to the entire benchmark.
The top one is the **_hottest_** function (`HOTFUNC` from now on).

ChopStiX provides a help script to automatically select function based on
coverage, function size and maximum number of functions to select. For
instance, the command:

    chop-score-table CHOPSTIX_DB 80 10 100

will dump the stats of at most 10 functions, with at least having 100
instructions in size (static funciton size), having a minimum of 80% of
coverage. 


## Function profiling

 chop-marks binary function_name

chop-perf-invok $$(cat $<) -o $@ -max 1000000 -- $$(cat $$arguments) > /dev/null 2> '$@.log'

## Invocation selection

@cti_cluster instr_ipc_density --max-clusters 20 --min-clusters-weight-percentage 1 --target-coverage-percentage 90 --outlier-percent 1 --outlier-minsize-threshold 1000 --minimum-granularity-percentage 1 --granularity-step-percentage 1 --output $@ --plot-path $*/plot_cluster --benchmark-name $${bname} --function-name $${fname} $<

cti_cluster_info summary 

cti_cluster_info representative -c $$clusterid %/cluster.json

## Invocation tracing

chop trace $$bpoints -indices $$invok -max-traces 1 -max-pages 300 -trace-dir $*/$$invok.trace/data -gzip -log-level verbose -log-path $*/$$invok.trace/trace.log $$(cat $$arguments) > $*/$$invok.trace/stdout.log 2> $*/$$invok.trace/stderr.log

chop-trace2mpt --trace-dir $*/$$invok.trace/data/ -o $*/$$invok.ubench --gzip

## Microbenchmark generation

mp_mpt2elf -t "$*/$$invok.ubench#0.mpt.gz" -T z16-z16-z64_linux_gcc -O $*/$$invok.ubench.s --raw-bin --compiler /opt/rh/gcc-toolset-10/root/bin/gcc --wrap-endless --reset --compiler-flags="-march=arch13" --fix-long-jump


## Microbenchmark tuning

  ./tools/chop-trace-mem -output tracemem -base-mpt mpt -output-mpt newmpt -- microbenchmark
 mp_mpt2elf -t "$*/$$invok.ubench#0.mpt.gz" -T z16-z16-z64_linux_gcc -O $*/$$invok.ubench.s --raw-bin --compiler /opt/rh/gcc-toolset-10/root/bin/gcc --wrap-endless --reset --compiler-flags="-march=arch13" --fix-long-jump
