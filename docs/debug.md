# Debug notes

1. Use CHOPSTIX_DEBUG=1 to enable the generation of stack trace when failing.  
2. In source/trace/system.cpp, in the signal handler, there are a couple of 
   macros to obtains the actual address accessed that generated the fault.
3. Use command line parameter `-log-level debug` to dump all kind of information
   about the steps being executed.
