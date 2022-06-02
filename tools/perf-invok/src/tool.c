#include "tool.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Necessary for sched_setaffinity
#endif

#include <time.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sample.h"
#include "limits.h"
#include "debug.h"

static int pid;
static Sample samples[MAX_SAMPLES];
static unsigned int sampleCount = 0;
static unsigned int flushedSampleCount = 0;
static int printHeaders = 1;
static int sampleInProgress = 1;
static FILE *outputFile = NULL;
static unsigned int endpoint_count = 0;
static unsigned int startpoint_count = 0;
static unsigned int maxSamples = UINT_MAX;
static Breakpoint breakpoints[MAX_BREAKPOINTS] = {0};

static void termOrKillHandler(int signum) {
    perfInvokExit(EXIT_FAILURE);
}

void perfInvokInit(char *output, int cpu, unsigned int requestedMaxSamples, int argc, char **argv) {
    // Pin process to CPU

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    fprintf(stderr, "Pinning process to CPU: %d\n", cpu);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    if (ret != 0) { perror("ERROR while setting affinity"); exit(EXIT_FAILURE);};

    // Fork & execute command

    fprintf(stderr, "Executing '");
    for (int i = 0; i < argc; i++) fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "'\n");

    pid = fork();

    if (pid == 0) {
        // Child process
        ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
        if (ret != 0) { perror("ERROR setting traced process"); exit(EXIT_FAILURE); };
        ret = execvp(argv[0], argv);
        if (ret != 0) { perror("ERROR executing process"); exit(EXIT_FAILURE);};
    } else {
        // Parent process

        // Open output file
        outputFile = (output != NULL ? fopen(output, "w") : NULL);
        assert(outputFile != NULL);

        // Set-up signal handler to kill child and flush samples
        struct sigaction sa;
        sa.sa_handler = termOrKillHandler;
        int ret = sigemptyset(&sa.sa_mask);
        if (ret != 0) { perror("ERROR setting empty signals"); exit(EXIT_FAILURE);};
        sa.sa_flags = 0;

        ret = sigaction(SIGTERM, &sa, NULL);
        if (ret != 0) { perror("ERROR setting SIGTERM"); exit(EXIT_FAILURE);};

        ret = sigaction(SIGINT, &sa, NULL);
        if (ret != 0) { perror("ERROR setting SIGINT"); exit(EXIT_FAILURE);};

        // Wait for child to start
        int status;
        ret = waitpid(pid, &status, 0);
        if (ret == -1) { perror("ERROR waiting child to start"); exit(EXIT_FAILURE);};

        configureEvents(pid);

        maxSamples = requestedMaxSamples;
    }
}

void perfInvokExit(int exitCode) {
    // Check if child is still alive
    if (!kill(pid, 0)) {
        // Kill child
        int ret = kill(pid, SIGKILL);
        if (ret != 0) { perror("ERROR killing process"); exit(EXIT_FAILURE);};
    }

    // If a sample is in progress, finish it
    if (sampleInProgress) {
        endSample(&samples[sampleCount - flushedSampleCount]);
        sampleCount++;
    }

    // Print the samples that haven't been flushed yet
    printSamples(outputFile, sampleCount - flushedSampleCount, samples, printHeaders);

    exit(exitCode);
}

void perfInvokBeginSample() {
    debug_print("Start sample %d\n", sampleCount);
    beginSample(&samples[sampleCount - flushedSampleCount]);
    sampleInProgress = 1;
}

void perfInvokEndSample() {
    endSample(&samples[sampleCount - flushedSampleCount]);
    sampleInProgress = 0;
    debug_print("End sample %d\n", sampleCount);

    sampleCount++;

    // If the buffer is full, flush it to disk
    if (sampleCount % MAX_SAMPLES == 0) {
        printSamples(outputFile, sampleCount - flushedSampleCount,
                     samples, printHeaders);
        printHeaders = 0;
        flushedSampleCount += MAX_SAMPLES;
    }

    if (sampleCount == maxSamples) perfInvokExit(EXIT_SUCCESS);
}

void perfInvokWait() {
    int status;
    long ret = waitpid(pid, &status, 0);
    if (ret == -1) { perror("ERROR during waiting"); exit(EXIT_FAILURE);};

    if (WIFSTOPPED(status)) {
        debug_print("Process stopped: %s\n", strsignal(WSTOPSIG(status)));
    }

    if (WIFEXITED(status)) {
        debug_print("Process exit code: %d\n", WEXITSTATUS(status));
        perfInvokExit(WEXITSTATUS(status));
    }

    if (WIFSIGNALED(status)) {
        debug_print("Process signaled: %s\n", strsignal(WTERMSIG(status)));
        exit(EXIT_FAILURE);
    }

    #if defined(__s390x__)
    if (WIFSTOPPED(status)) {
        // Z architecture advances 2 bytes the PC on SIGILL
        if (WSTOPSIG(status) == SIGILL) {
            displace_pc(pid, -2);
        }
    }
    #endif
}

void perfInvokContinue() {
    long ret = ptrace(PTRACE_CONT, pid, 0, 0);
    if (ret != 0) { perror("ERROR on PTRACE_CONT"); exit(EXIT_FAILURE);};
}

void perfInvokSetBreakpoint(unsigned long long address) {
    unsigned int id = 0;
    while (breakpoints[id].address != 0) id++;
    Breakpoint *bp = &breakpoints[id];

    setBreakpoint(pid, address, bp);
}

void perfInvokResetBreakpoint(unsigned long long address) {
    unsigned int id = 0;
    while (breakpoints[id].address != address) id++;
    Breakpoint *bp = &breakpoints[id];

    resetBreakpoint(pid, bp);

    bp->address = 0;
}
