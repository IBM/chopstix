# Tracing

The process of tracing is in charge of obtaining all the required process 
state to be able to reproduce the traced region of execution afterwards. 

Currently ChopStiX officially supports _Region Of Interest_ (ROI) tracing.
Support for _Temporal_ tracing or _Hybrid_ tracing is experimental (i.e.
not fully implemented nor tested). The properties of the different tracing
modes are the following:

- _ROI Tracing_: Tracing starts/ends when specific code addresses are executed.
  The addresses define what is called the Region of Interest (ROI). 

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


