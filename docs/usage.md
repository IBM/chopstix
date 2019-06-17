# Usage guide

This document will walk you through a basic ChopStiX workflow example.
During these steps you can run `chop list` to show more information.
For more information see `chop help list`.

We start of by sampling an invocation of ./my_app.

    chop sample ./my_app

This will start running `my_app` and sample performance counters,
which are then written into a session.

This creates a new sampling session. We can then list all collected samples.
    
    chop list sessions
    chop list samples

To continue, we need the static Control Flow Graph (CFG) of the application. 
Each application consists of modules. These correspond to object files and 
executables. We will create the static CFG for our `my_app` program.

    chop disasm my_app

Alternatively you can let ChopStiX deduce all modules, by running `chop disasm`
without parameters. Note however, that you need to have at least one sampling
session to use this feature.

We can now see the instructions (text) of our binaries.

    chop text function -name main

The following step groups all collected samples by instruction and counts
samples per basic block, function, and module.

    chop count

We can then annotate the control flow graph with the sampling frequency
of each basic block.

    chop annotate

We are now able to visualize the annotated control flow graph.

    chop text function -name main -fmt annotate

To generate snippet paths we can either search all binaries, or limit
the search to a specific module or even function. We can also filter
the functions to only consider ones with a minimum score.

ChopStiX then searches all backedges of the selected functions for
the hottest paths. The search ends once we have found a certain
number of snippets, or have a certain coverage. Alternatively we
can also specify a timeout. Some example invocations follow.

    chop search module -name %my_app -target-coverage 80%
    chop search function -name main -target-count 10
    chop search -timeout 10m

We can then list all created paths and can create Microprobe test files,
which can then be compiled into standalone microbenchmarks.

    chop list paths
    chop text path -id 1 -fmt mpt -out snippet.1.mpt
    mp_mpt2c.py -t snippet.1.mpt -O snippet.1.c -t $TARGET \
        --fix-indirect-branches --fix-branch-next --fix-memory-registers
    gcc snippet.1.c -o snippet.1.run
    ./snippet.1.run

## Tracing example

The following example shows how to generate traces of the various invocations
of a particular function. Let us assume that after sampling, we detected that
_func1_ of _myapp_ is the hottest function. We want to trace the first 10
invocation. To do so:

    chop trace $(chop-marks-ppc64 myapp func1) -gzip myapp -max-traces 10 -trace-dir output_dir

The command above will generate 10 traces and will store the contents in 
output_dir. Then, the output directory can be postprocessed to generate the 
corresponding mpts with the following command:

    chop-trace2mpt -o mynewmpt --trace-dir output_dir --gzip

The mpts generated can then be processed by Microprobe to generate microbenchmarks
in different formats. 

The _chop-marks-ppc64_ command is a help script to automatically detect the
addresses of the entry and exit poings of the function to be traced. Otherwise,
that input have to be provided manually. 
