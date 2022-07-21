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

will dump the stats of at most 10 functions having a minimum of 80% of
coverage, where each of them having at least 100 instructions in size (static
function size).

## Function profiling

Once a `HOTFUNC` is has been selected. The next step is to know which 
particular invocation we want to trace and reproduce. This is not straighforward
since hot functions tipically are executed multiple times and most of the time
exhibit much different behaviors over time depending on the input parameters.
In such case, the question is: which particular invocation (or set of 
invocations) represent best the typical behavior of the function? 

First, we need to obtain the corresponding addreses in memory where the
function starts and ends. i.e. the entry and exit points of the function.
One can do so manually, by inspecting the code but ChopStiX provides a helper
script that facilitates the process: 

    chop-marks BINARY HOTFUNC

will return the addresses of the being/end points for the funciton. From now
on, we call them `HOTFUNC_MARKS`. This information is going to be used for 
profiling the function in the next step as well as for tracing the selected
invocation in the tracing step.

Now that we know the `HOTFUNC_MARKS` that define the region of interest (ROI),
we can profile its performance using the following command: 

    chop-perf-invok HOTFUNC_MARKS -o HOTFUNC_PROFILE -max 1000000 -- BINARY ARGUMENTS

The command will generate a CSV file named `HOTFUNC_PROFILE` with a maximum 
of 1M entries. The stats include instruction, cycle and execution time
information, etc. Therefore, they are subject to experimental noise. Like in
the first step, you might want to minimize the activity on the rest of the
system or repeat the experiment various times to improve the robustness
of the approach. 

## Invocation selection

You can manually analyse the stats of the CSV generated to understand in
detail the behavior of each invocation and manually select a particular
invocation you find worh to trace. Alternatively, you can use the clustering
analysis explained in the rest of the section to perform the selection
automatically.

ChopStiX provide a set of support scripts to analyse the generated CSV, 
create clusters and select a representative invocation for each of them.
To do so, execute:

    cti_cluster instr_ipc_density --max-clusters 20 --min-clusters-weight-percentage 1 --target-coverage-percentage 80 --output CLUSTER_INFO --plot-path CLUSTER_PLOT --benchmark-name BINARY_NAME --function-name HOTFUNC HOTFUNC_PROFILE

where:

- `CLUSTER_INFO` is the output JSON file that will contain of the clustering
  information.
- `CLUSTER_PLOT` is the path to the plot that will be generated to visually 
  debug the clustering analysis.
- `BINARY_NAME` is the benchmark name.

The command will generate a maximum of 20 clusters or lesss is total coverage
reaches 80% before. Each cluster will have at least 1% of coverage. There
exists more parameters to control the clustering process. Please refer to the
actual command line information for further details. Once the `CLUSTER_INFO`
is generated a summary can be viewed using:

    cti_cluster_info summary CLUSTER_INFO 

and a representative of a cluster can be obtained using:

    cti_cluster_info representative -c CLUSTER_ID CLUSTER_INFO

where:

- `CLUSTER_ID` is the ID of the cluster, you can get from the previous summary.
  Cluster IDs are sorted by coverage, therefore, selecting cluster ID to be 0,
  will select a representative of the most common behavior.

After executing the previous command, we obtain the invocation number of the
most representative behavior of the function, `HOTFUNC_INVOCATION` from now on.

## Invocation tracing

Next step is to trace that particular `HOTFUNC_INVOCATION` of the `HOTFUNC`. 
To do so, execute:

chop trace HOTFUNC_MARKS -indices HOTFUNC_INVOCATION -max-traces 1 -trace-dir TRACEDIR -gzip BINARY ARGUMENTS 

where:

- `TRACEDIR` is the output trace directory.

The command above will trace the ROI defined by `HOTFUNC_MARKS` on the `HOTFUNC_INVOCATION`
invocation. We only need to generate one trace, therefore we set `-max-traces 1`
to stop tracing as soon as the particular traced invocation ends. The raw dump
will be place into `TRACEDIR`. To convert it into Microprobe test files MPTs to
be used later on, execute the following command:

    chop-trace2mpt --trace-dir TRACEDIR -o MPTBASENAME --gzip

where:

- `MPTBASENAME` will be the output base name of the MPTs generated.

## Microbenchmark generation

From the MPTs generated in the previous step, we can then use Microprobe 
framework to process them and convert them to different formats. Check the
documentation of Microprobe for all the possibilities available. 

A typicall use case is to convert the MPT into a self runnable ELF. I.e. the
function invocation will converted into an executable that will continously
execute the traced function in an endless loop. To do so, you can use
the following command:

mp_mpt2elf -t MPTFILE -T MICROPROBE_TARGET -O OUTPUT_NAME.s --raw-bin --compiler COMPILER --wrap-endless --reset --compiler-flags="COMPILER_FLAGS" --fix-long-jump

where:

- `MPTFILE` is the MPT file generated in the previous step.
- `MICROPROBE_TARGET` is a Microprobe target. E.g. `z16-z16-z64_linux_gcc` or
  `power_v310-power10-ppc64_linux_gcc`.
- `COMPILER` compiler is the path to the compiler binary.
- `COMPILER_FLAGS` are the compiler flags to be used.

The command will generate an assembly file `OUTPUT_NAME.s` and its corresponding
compiled program `OUTPUT_NAME.elf`. Microprobe will maintain all original code
and addresses. The extra initialization code added is to wrap the ROI into an
endless loop and reset the user registers to the original state so that each 
iteration the function is executed with the same initial values in the 
registers. You can debug the assembly generated by opening the assembly file
generated.

You can try the functional correctness of the `OUTPUT_NAME.elf` by executing 
it and checking the functional correctness and the performance profile. For
most of the simple functions, the workflow stops here and you succeeded in
generating a small version (microbenchmark) of the most representative 
function invocation of the **_hottest_** fucntion of a benchmark.

Still, there might be errors or the performance might not be expected.
This is because we only reset the state of the registers. However, there might
be ROIs on which their functional correctness or performance profile depend on
the state of the memory, which we do not reset at each iteration.
To fix such errors and missmatches, we need to proceed to the next step.

## Microbenchmark tuning

  ./tools/chop-trace-mem -output tracemem -base-mpt mpt -output-mpt newmpt -- microbenchmark
 mp_mpt2elf -t "$*/$$invok.ubench#0.mpt.gz" -T z16-z16-z64_linux_gcc -O $*/$$invok.ubench.s --raw-bin --compiler /opt/rh/gcc-toolset-10/root/bin/gcc --wrap-endless --reset --compiler-flags="-march=arch13" --fix-long-jump
