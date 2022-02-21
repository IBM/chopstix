#include "sample.h"
#include "debug.h"
#include "errno.h"
#include <stdlib.h>

#ifdef PERF_INVOK_PLATFORM_SIFIVE_FU540

#include "platform/sifive_fu540.h"

// API events
#define EVENT_MEM_INSTR_RETIRED (EVENT_BIT_INT_LOAD_RETIRED | EVENT_BIT_INT_STORE_RETIRED | EVENT_BIT_FLT_LOAD_RETIRED | EVENT_BIT_FLT_STORE_RETIRED)
#define EVENT_DATA_CACHE_BUSY EVENT_BIT_DATA_CACHE_BUSY
#define EVENT_DATA_CACHE_MISS EVENT_BIT_DATA_CACHE_MISS

#define READ_CSR(reg) ({unsigned long long __tmp; asm volatile ("csrr %0, " #reg : "=r"(__tmp)); __tmp;})
#define WRITE_CSR(reg, val) asm volatile ("csrw " #reg ", %0" :: "r" (val))

#else

#include <linux/perf_event.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>

struct read_format {
  unsigned long long nr;
  struct {
    unsigned long long value;
    unsigned long long id;
  } values[];
};

int fdCycles, fdInstructions, fdMemoryLoads, fdMemoryWrites, fdMemoryReadMisses,
    fdMemoryWriteMisses;
unsigned long long idCycles, idInstructions, idMemoryLoads, idMemoryWrites,
                   idMemoryReadMisses, idMemoryWriteMisses;
struct timeval start;

#endif

void configureEvents(pid_t pid) {

    debug_print("Configuring events for process %d\n", pid);

#ifdef PERF_INVOK_PLATFORM_SIFIVE_FU540

    debug_print("Configuring EVENT_MEM_INSTR_RETIRED");
    WRITE_CSR(mhpmevent3, EVENT_MEM_INSTR_RETIRED);
    debug_print("Configuring EVENT_DATA_CACHE_MISS");
    WRITE_CSR(mhpmevent4, EVENT_DATA_CACHE_MISS);

#else

    struct perf_event_attr attributes;
    memset(&attributes, 0, sizeof(struct perf_event_attr));

    attributes.size = sizeof(struct perf_event_attr);
    attributes.disabled = 1;
    attributes.exclude_kernel = 1;
    attributes.exclude_hv = 1;
    attributes.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    debug_print("Configuring PERF_COUNT_HW_CPU_CYCLES\n");
    attributes.config = PERF_COUNT_HW_CPU_CYCLES;
    attributes.type = PERF_TYPE_HARDWARE;
    fdCycles = syscall(__NR_perf_event_open, &attributes, pid, -1, -1, 0);
    if (fdCycles == -1) { perror("ERROR while setting PERF_COUNT_HW_CPU_CYCLES"); exit(EXIT_FAILURE); };
    int ret = ioctl(fdCycles, PERF_EVENT_IOC_ID, &idCycles);
    if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_CPU_CYCLES event id"); exit(EXIT_FAILURE); };

    debug_print("Configuring PERF_COUNT_HW_INSTRUCTIONS\n");
    attributes.config = PERF_COUNT_HW_INSTRUCTIONS;
    attributes.type = PERF_TYPE_HARDWARE;
    fdInstructions =
        syscall(__NR_perf_event_open, &attributes, pid, -1, fdCycles, 0);
    if (fdInstructions == -1) { perror("ERROR while setting PERF_COUNT_HW_INSTRUCTIONS"); exit(EXIT_FAILURE); };
    ret = ioctl(fdInstructions, PERF_EVENT_IOC_ID, &idInstructions);
    if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_INSTRUCTIONS event id"); exit(EXIT_FAILURE); };

    debug_print("Configuring PERF_COUNT_HW_CACHE_L1D:READ:ACCESS\n");
    attributes.config = PERF_COUNT_HW_CACHE_L1D
                        | (PERF_COUNT_HW_CACHE_OP_READ << 8)
                        | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16);
    attributes.type = PERF_TYPE_HW_CACHE;
    fdMemoryLoads =
        syscall(__NR_perf_event_open, &attributes, pid, -1, fdCycles, 0);
    if (fdMemoryLoads == -1) {
        perror("WARNING: Unable to set PERF_COUNT_HW_CACHE_L1D:READ:ACCESS");
    } else {
        ret = ioctl(fdMemoryLoads, PERF_EVENT_IOC_ID, &idMemoryLoads);
        if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_CACHE_L1D:READ:ACCESS event id"); exit(EXIT_FAILURE); }
    }

    debug_print("Configuring PERF_COUNT_HW_CACHE_L1D:WRITE:ACCESS\n");
    attributes.config = PERF_COUNT_HW_CACHE_L1D
                        | (PERF_COUNT_HW_CACHE_OP_WRITE << 8)
                        | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16);
    attributes.type = PERF_TYPE_HW_CACHE;
    fdMemoryWrites =
        syscall(__NR_perf_event_open, &attributes, pid, -1, fdCycles, 0);
    if (fdMemoryWrites == -1) {
        perror("WARNING: Unable to set PERF_COUNT_HW_CACHE_L1D:WRITE:ACCESS");
    } else {
        ret = ioctl(fdMemoryWrites, PERF_EVENT_IOC_ID, &idMemoryWrites);
        if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_CACHE_L1D:WRITE:ACCESS event id"); exit(EXIT_FAILURE); }
    }

    debug_print("Configuring PERF_COUNT_HW_CACHE_L1D:READ:MISS\n");
    attributes.config = PERF_COUNT_HW_CACHE_L1D
                        | (PERF_COUNT_HW_CACHE_OP_READ << 8)
                        | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    attributes.type = PERF_TYPE_HW_CACHE;
    fdMemoryReadMisses =
        syscall(__NR_perf_event_open, &attributes, pid, -1, fdCycles, 0);
    if (fdMemoryReadMisses == -1) {
        perror("WARNING: Unable to set PERF_COUNT_HW_CACHE_L1D:READ:MISS");
    } else {
        ret = ioctl(fdMemoryReadMisses, PERF_EVENT_IOC_ID, &idMemoryReadMisses);
        if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_CACHE_L1D:READ:MISS event id"); exit(EXIT_FAILURE); }
    }

    debug_print("Configuring PERF_COUNT_HW_CACHE_L1D:WRITE:MISS\n");
    attributes.config = PERF_COUNT_HW_CACHE_L1D
                        | (PERF_COUNT_HW_CACHE_OP_WRITE << 8)
                        | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    attributes.type = PERF_TYPE_HW_CACHE;
    fdMemoryWriteMisses =
        syscall(__NR_perf_event_open, &attributes, pid, -1, fdCycles, 0);
    if (fdMemoryWriteMisses == -1) {
        perror("WARNING: Unable to set PERF_COUNT_HW_CACHE_L1D:WRITE:MISS");
    } else {
        ret = ioctl(fdMemoryWriteMisses, PERF_EVENT_IOC_ID, &idMemoryWriteMisses);
        if (ret == -1) { perror("ERROR while getting PERF_COUNT_HW_CACHE_L1D:WRITE:MISS event id"); exit(EXIT_FAILURE); }
    }

#endif

    debug_print("Configuring events for process %d done\n", pid);

}

void beginSample(Sample *sample) {

    debug_print("Begin sample, reset counters\n");
#ifdef PERF_INVOK_PLATFORM_SIFIVE_FU540

    WRITE_CSR(mhpmcounter3, 0);
    WRITE_CSR(mhpmcounter4, 0);
    WRITE_CSR(instret, 0);
    WRITE_CSR(cycle, 0);
    sample->time = READ_CSR(time);

#else

    memset(sample, 0, sizeof(Sample));

    gettimeofday(&start, NULL);

    ioctl(fdCycles, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    ioctl(fdCycles, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

#endif
    debug_print("Begin sample, reset counters done\n");
}

void endSample(Sample *sample) {

    debug_print("End sample, read counters\n");

#ifdef PERF_INVOK_PLATFORM_SIFIVE_FU540

    sample->cycles = READ_CSR(cycle);
    sample->retiredInstructions = READ_CSR(instret);
    sample->retiredMemoryInstructions = READ_CSR(mhpmcounter3);
    sample->dataCacheMisses = READ_CSR(mhpmcounter4);
    sample->time = (READ_CSR(time) - sample->time) * TIME_TO_US;

#else

    ioctl(fdCycles, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

    struct timeval end;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &end);
    sample->time = end.tv_sec * 1000000 + end.tv_usec;

    char buf[4096];
    struct read_format *rf = (struct read_format *) buf;
    if (read(fdCycles, buf, sizeof(buf)) == 0) { perror("ERROR while reading perf counters"); exit(EXIT_FAILURE); };
    for (unsigned int i = 0; i < rf->nr; i++) {
        unsigned long long id = rf->values[i].id;
        unsigned long long value = rf->values[i].value;
        if (id == idCycles) {
            sample->cycles = value;
        } else if (id == idInstructions) {
            sample->retiredInstructions = value;
        } else if (id == idMemoryLoads || id == idMemoryWrites) {
            sample->retiredMemoryInstructions += value;
        } else if (id == idMemoryReadMisses || id == idMemoryWriteMisses) {
            sample->dataCacheMisses += value;
        }
    }

#endif

    debug_print("End sample, read counters done\n");

}

void printSamples(FILE *fd, unsigned int sampleCount, Sample *samples,
                  int printHeaders) {

    if (printHeaders) {
        fprintf(fd, "Cycles, Time Elapsed (us), Retired Instructions, "
                    "Retired Memory Instructions, Data Cache Misses, "
                    "Instructions Per Cycle, Miss Percentage\n");
    }

    for (unsigned int i = 0; i < sampleCount; i++) {
        Sample *sample = &samples[i];

        float ipc = sample->retiredInstructions / (float) sample->cycles;
        float missrate =
            sample->dataCacheMisses / (float) sample->retiredMemoryInstructions;

        fprintf(fd, "%llu, %llu, %llu, %llu, %llu, %f, %f\n",
                sample->cycles, sample->time,
                sample->retiredInstructions, sample->retiredMemoryInstructions,
                sample->dataCacheMisses,
                ipc, 100.0f * missrate);
    }
}
