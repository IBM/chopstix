typedef struct {
    unsigned long long address;
    unsigned long long originalData;
} Breakpoint;

void setBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint);
void resetBreakpoint(unsigned long pid, Breakpoint *breakpoint);

#if defined(__s390x__)
#define NT_PRSTATUS 1
void displace_pc(long pid, long displ);
#endif
