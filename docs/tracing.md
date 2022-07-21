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



# ROI Tracing

## Tracing example

In the [usage](docs/usage.md) documentation, there is a simple example on
how to invoke `chop` for tracing. 

Also, in the `./examples/tracing/` directory, you can find a more detailed
tracing example, including a script to perform all the necessary steps to
trace a particular function and convert the extracted trace into a
self-runable binary.

# Notes about tracing

1. Code pages of specific libc functions (used by the tracing supoprt library)
   are not protected and always dumped for safety. 
2. System calls are not supported during tracing. Whenever a system call is
   hit during tracing. The trace is finished. The system call is services, and
   tracing starts again. I.e. 


