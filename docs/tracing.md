# Tracing

The process of tracing is in charge of obtaining all the required process 
state to be able to reproduce it afterwards. To do so, one needs the 
initial processor register state (i.e. all the user-accessible registers) and
the initial memory state used by during the tracing period. 

Obtaining the initial register state is straightforward since it is just a 
matter of dumping all registers when starting the tracing period. One can
do the same with the memory state. However, it would be impractical since the
size of the virtual address space of the traced process can be quite large
(and probably, most of it would not be accessed during the tracing period). 
In order to obtain only the required memory state, ChopStiX dumps the 
code and data regions accessed during the tracing period. To do so, it 
uses the memory protection mechanisms available. When starting the tracing 
period, ChopStiX forbids access to the entire memory address space of the
process and whenever an exception is generated due to an access, the
accessed page is dumped to disk and read/write/execute access is restored.
This way, only the required memory state (at page level granularity) is dumped
to disk.

Currently ChopStiX officially supports _Region Of Interest_ (ROI) tracing.
Support for _Temporal_ tracing or _Hybrid_ tracing is experimental (i.e.
not fully implemented nor tested). The properties of the different tracing
modes are the following:

- _ROI Tracing_: Tracing starts/ends when specific code addresses are executed.
  The addresses define what is called the Region of Interest (ROI). For example,
  if we want to trace a particular function, the start address will be the
  function entry point and the end addresses will be the function exit points.
- _Temporal Tracing_: Tracing starts/end at user specified time intervals.
  For example, one can specify to trace for 5 seconds after waiting for a 10
  seconds.
- _Hybrid Tracing_: Tracing start/ends when specific code address are executed.
  However, the end tracing is activated _after_ a specified amount of time. I.e.
  we can to start tracing at least for 5 seconds after a particular function
  is reached and stop the first time another function is executed from that
  point.

## ROI Tracing

For defining the Region of Interest (ROI) to be traced, multiple _begin_ and 
_end_ addresses can be provided. The first time a begin _address_ is hit, the
tracing starts until the first time an _end_ address is hit. That corresponds
to a _ROI invocation_. Using user parameters one can select the _ROI invocations_
to trace. E.g. one might want to trace the 10th time a particular function
is executed.

It is up to the user to provide the correct set of being/end addresses.
Depending on the toolchain configuration and system the static addresses
of the binary (i.e. the ones shown when dumping the binary using _objdump_)
might correspond or not with the addresses of the program image loaded by the
system loader before starting the execution. The tool `chop-marks-dyn-addr`
is provided to support the task of finding out the displacement between the 
objdump address and the address loaded in memory. The steps to perform the 
find the right addresses are the following:

    objdump -d binary 

To obtain the dump of the binary.

    chop-marks-dyn-addr binary main

To obtain information of the loaded segments in memory when the program is
executed. Discard any error message, the key information you want to obtain 
is the base address of the PT\_LOAD segments. You can add that base address 
with the address of the binary dump to obtain the final address that should
be used for tracing. 

ROIs can be defined at any boundary, and it is up to the user to select what
to trace. This provides flexibility since one can begin/end tracing at function
entry/exit points or at begin/end of computational loops or a combinations or
any other address. Typically, the tracing of function invocations is quite 
common because previous profiling steps are typically done at function level
(to know what to trace) and also because having clearly defined boundaries 
enables some assumptions and optimizations later on when converting the 
traced region into a self-executable binary or another format for tracing and
simulation. Therefore, ChopStiX provides a helper script script to automatically
compute the begin/end addresses of function-level ROIs (i.e. the function 
entry/exit addresses). To do so, execute:

    chop-marks binary function_name

to obtain the list of being/end addresses corresponding to the entry/exit
points of the *function_name* without requiring further address math. The
parameters can be directly fed into the `chop trace` command as following:

    chop trace $(chop-marks binary function_name) EXTRA_TRACING_OPTIONS ./binary BINARY_ARGUMENTS

Check `chop trace --help` for all the details and tracing options.  In the
[usage](usage.md) documentation, there is a simple example on how to
invoke `chop` for tracing. 

Once tracing is done, the raw output needs to be processed in order to be
converted into a MPTs (Microprobe Test files). To do so execute:

    chop-trace2mpt trace_directory basename

Then, it is up to the Microprobe tool to process and convert the Microprobe
test definition to another format. Check the Microprobe documentation for
the different possibilities.  In the `./examples/tracing/` directory,
you can find a more detailed tracing example, including a script to perform
all the necessary steps to trace a particular function and convert the
extracted trace into a self-runnable binary. The example also includes 
the steps required to complete the MPTs generated with a detailed memory
access traces using a Valgrind's based `chop-trace-mem`.

## Notes about tracing

1. Code pages of specific libc functions (used by the tracing support library)
   are not protected and always dumped for safety. 
2. System calls are not supported during tracing. Whenever a system call is
   hit during tracing. The trace is finished. The system call is serviced, and
   tracing starts again. I.e. tracing a function invocation with a system
   call will generate two traces.
3. ChopStiX minimizes the number of systems calls using library interposition
   mechanisms. Common libc functions such as _printf_/_puts_ are overridden
   with custom implementations during tracing to minimize the number of 
   system calls. The list of overridden functions is preliminary, and more
   functions will be added in the future to nullify the number of system calls
   while maintaining functional correctness.
4. Recursive tracing is not supported. Tracing will start a first entry address
   and stop at first end address.
5. ChopStiX disables ASLR (Address Space Layout Randomization) to ensure 
   reproducibility.
