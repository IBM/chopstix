#include "breakpoint.h"

// Tool API used by the different modes

void perfInvokInit(char *output, int cpu, unsigned int requestedMaxSamples, int argc, char **argv, char *module, char *mainmodule);
void perfInvokExit(int exitCode, int signum);

void perfInvokBeginSample();
void perfInvokEndSample();

void perfInvokSetBreakpoint(unsigned long long address);
void perfInvokResetBreakpoint(unsigned long long address);

void perfInvokWait();
void perfInvokContinue();
