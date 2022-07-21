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
Depending on the performance counter being sample, the definition of *_hottest_*

    chop sample -data $*/sample.chop.db -events CYCLES -period 100000

    @chop disasm -data $*/sample.chop.db $*/bin/run.$(TARGET) $$(ldd $*/bin/run.$(TARGET) | cut -d " " -f 3 | grep -v libc.so)
    @chop count -data $*/sample.chop.db                                         
    @chop annotate -data $*/sample.chop.db                                      
    @chop list functions -data $*/sample.chop.db > $@   



 chop-score-table $*/sample.chop.db $$coverage 10 1 -functions

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
