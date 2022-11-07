#include "tool.h"
#include <errno.h>
#include <sys/personality.h>

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
#include "config.h"

static int pid;
static Sample samples[MAX_SAMPLES];
static unsigned int sampleCount = 0;
static unsigned int flushedSampleCount = 0;
static int printHeaders = 1;
static int sampleInProgress = 0;
static int full = 0;
static FILE *outputFile = NULL;
static unsigned int endpoint_count = 0;
static unsigned int startpoint_count = 0;
static unsigned int maxSamples = UINT_MAX;
static Breakpoint breakpoints[MAX_BREAKPOINTS] = {0};
extern unsigned long long base_address;
static int max_rlevel_seen = -1;


static void termOrKillHandler(int signum) {
    perfInvokExit(EXIT_FAILURE, signum);
}

void perfInvokInit(char *output, int cpu, unsigned int requestedMaxSamples, int argc, char **argv, char *module, char *mainmodule) {
    // Pin process to CPU

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    fprintf(stderr, "Pinning process to CPU: %d\n", cpu);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    if (ret != 0) { perror("ERROR while setting affinity"); perfInvokExit(EXIT_FAILURE, SIGKILL);};

    // Fork & execute command

    fprintf(stderr, "Executing '");
    for (int i = 0; i < argc; i++) fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "'\n");

    pid = fork();

    if (pid == 0) {
        // Child process
        ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
        if (ret != 0) { perror("ERROR setting traced process"); perfInvokExit(EXIT_FAILURE, SIGKILL); };

		int persona = personality(0xffffffff);
		if (persona == -1)
		{
			fprintf(stderr, "ERROR Unable to get ASLR info: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		persona = persona | ADDR_NO_RANDOMIZE;
		debug_print("Disabling ASLR ...");
		persona = personality(persona | ADDR_NO_RANDOMIZE);
		if (persona == -1) {
			fprintf(stderr, "ERROR Unable to set ASLR info: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (!(personality (0xffffffff) & ADDR_NO_RANDOMIZE))
		{
			fprintf(stderr, "ERROR Unable to disable ASLR");
			exit(EXIT_FAILURE);
		}
		debug_print("ASLR disabled");

        ret = execvp(argv[0], argv);
        if (ret != 0) { perror("ERROR executing process"); perfInvokExit(EXIT_FAILURE, SIGKILL);};
    } else {
        // Parent process

        // Open output file
        outputFile = (output != NULL ? fopen(output, "w") : NULL);
        assert(outputFile != NULL);

        // Set-up signal handler to kill child and flush samples
        struct sigaction sa;
        sa.sa_handler = termOrKillHandler;
        int ret = sigemptyset(&sa.sa_mask);
        if (ret != 0) { perror("ERROR setting empty signals"); perfInvokExit(EXIT_FAILURE, SIGKILL);};
        sa.sa_flags = 0;

        ret = sigaction(SIGTERM, &sa, NULL);
        if (ret != 0) { perror("ERROR setting SIGTERM"); perfInvokExit(EXIT_FAILURE, SIGKILL);};

        ret = sigaction(SIGINT, &sa, NULL);
        if (ret != 0) { perror("ERROR setting SIGINT"); perfInvokExit(EXIT_FAILURE, SIGKILL);};

        // Wait for child to start
        int status;
        ret = waitpid(pid, &status, 0);
        if (ret == -1) { perror("ERROR waiting child to start"); perfInvokExit(EXIT_FAILURE, SIGKILL);};

        compute_base_address(pid, module, mainmodule);

        configureEvents(pid);

        maxSamples = requestedMaxSamples;
    }
}

void perfInvokExit(int exitCode, int signum) {
    // Check if child is still alive
    if (!kill(pid, 0)) {
        // Kill child
        int ret = kill(pid, signum);
        if (ret != 0) { perror("ERROR Sending signal to process"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    }

    // If a sample is in progress, finish it
    if (sampleInProgress) {
        endSample(&samples[sampleCount - flushedSampleCount]);
        // Only account the interrupted sample if it is the only one (full
        // application execution)
        if ((sampleCount == 0) && full) {
            sampleCount++;
        }
    } else if (sampleCount == 0) {
        fprintf(stderr, "ERROR: Process timeout too short\n");
        exit(EXIT_FAILURE);
    }

    if ((sampleCount < 1000) && !full) {
        if (outputFile != stderr) {
            int ret = fclose(outputFile);
            if (ret != 0) { perror("ERROR closing file"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        }
        fprintf(stderr, "ERROR: Process timeout with less than 1000 samples\n");
        exit(EXIT_FAILURE);
    }

    // Print the samples that haven't been flushed yet
    printSamples(outputFile, sampleCount - flushedSampleCount, samples, printHeaders);

    fprintf(stderr, "INFO: %d samples gathered\n", sampleCount);
    fprintf(stderr, "INFO: %d levels of recursivity detected\n", max_rlevel_seen);

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

    if (sampleCount == maxSamples) perfInvokExit(EXIT_SUCCESS, SIGKILL);
}

void perfInvokWait() {
    int status;
    long ret;
    #if MULTIPROCESS
        ret = waitpid(-1, &status, 0);
        if (ret != pid) { perror("ERROR: during tracing. Unexpected pid (did the process created subprocesses?)."); kill(ret, SIGKILL); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    #else
        ret = waitpid(pid, &status, 0);
    #endif
    if (ret == -1) { perror("ERROR during waiting"); perfInvokExit(EXIT_FAILURE, SIGKILL);};

    if (WIFSTOPPED(status)) {
        debug_print("Process stopped: %s\n", strsignal(WSTOPSIG(status)));
    }

    if (WIFEXITED(status)) {
        debug_print("Process exit code: %d\n", WEXITSTATUS(status));
        perfInvokExit(WEXITSTATUS(status), 0);
    }

    if (WIFSIGNALED(status)) {
        debug_print("Process signaled: %s\n", strsignal(WTERMSIG(status)));
        perfInvokExit(EXIT_FAILURE, SIGKILL);
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
    if (ret != 0) { perror("ERROR on PTRACE_CONT"); perfInvokExit(EXIT_FAILURE, SIGKILL);};
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
