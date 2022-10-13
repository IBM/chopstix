/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/personality.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include "breakpoint.h"
#include "sample.h"
#include "debug.h"

#define MAX_SAMPLES 8192
#define MAX_BREAKPOINTS 1024

int pid;
Sample samples[MAX_SAMPLES];
unsigned int sampleCount = 0;
unsigned int flushedSampleCount = 0;
int printHeaders = 1;
int sampleInProgress = 1;
FILE *outputFile;
unsigned int endpoint_count = 0;
unsigned int startpoint_count = 0;
char *module = "main";
char *mainmodule = "main";
extern unsigned long long base_address;

void help(FILE *fd) {
    if (fd == stderr){
        fprintf(fd, "Wrong command line parameters\n");
        fprintf(fd, "\n");
    } else {
        fprintf(fd, "\n");
    }
    fprintf(fd, "Usage:\n");
    fprintf(fd, "\n");
    fprintf(fd, "chop-perf-invok -o output [-begin addr] [-begin addr] ...] [-end addr [-end addr ...]] [-timeout seconds] [-max samples] [-module] [-cpu cpu] [-h] -- command-to-execute\n");
    fprintf(fd, "\n");
    fprintf(fd, "-o name          output file name\n");
    fprintf(fd, "-begin addr      start address of the region to measure (can be specified multiple times)\n");
    fprintf(fd, "-end addr        end address of the region to measure (can be specified multiple times)\n");
    fprintf(fd, "-module name     module to be traced (if different than main)\n");
    fprintf(fd, "-timeout seconds stop measuring after the specified number of seconds (default: no timeout)\n");
    fprintf(fd, "-max samples     stop measuring after the specified number of measurements(default: no limit)\n");
    fprintf(fd, "-cpu cpu         pin process to the specified CPU (default: 0)\n");
    fprintf(fd, "-h               print this help message\n");
    fprintf(fd, "\n");

    if (fd == stderr){
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}

void handler(int signum) {
    int ret = kill(pid, signum);
    if (ret != 0) { perror("ERROR Sending signal to process"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

    if (sampleInProgress) {
        endSample(&samples[sampleCount - flushedSampleCount]);
        sampleCount++;
    }

    printSamples(outputFile, sampleCount - flushedSampleCount, samples, printHeaders);

    if (outputFile != stderr) {
        ret = fclose(outputFile);
        if (ret != 0) { perror("ERROR closing file"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    }
    exit(EXIT_FAILURE);
}

int perInvocationPerformance(unsigned long long * addrStart,
                              unsigned long long * addrEnd,
                              unsigned int maxSamples,
                              FILE *outputFile) {
    int status;
    Breakpoint bp[MAX_BREAKPOINTS];

    for(int i=0; i<startpoint_count; i++) setBreakpoint(pid, addrStart[i], &bp[i]);

    long ret = ptrace(PTRACE_CONT, pid, 0, 0);
    if (ret != 0) { perror("ERROR: on initial tracing"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

    while (1) {
        ret = waitpid(pid, &status, 0);
        if (ret == -1) { perror("ERROR: during tracing"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        if (sampleCount >= maxSamples) { fprintf(stderr, "INFO: Max samples reached\n"); break;}
        if (WIFEXITED(status)) {
            fprintf(stderr, "INFO: Process finished\n");
            if (sampleCount == 0 ) { 
                fprintf(stderr, "ERROR: No samples gathered\n");
                fprintf(stderr, "ERROR: - Check for the correctness of the breakpoint addresses\n");
                fprintf(stderr, "ERROR: - Check if the function is executed *AFTER* the main (initialization functions might be skipped)\n");
            }
            return WEXITSTATUS(status);
        };

        if (WIFSTOPPED(status)) {
            debug_print("%s\n", strsignal(WSTOPSIG(status)));
        }

        for(int i=0; i<startpoint_count; i++) resetBreakpoint(pid, &bp[i]);
        for(int i=0; i<endpoint_count; i++) setBreakpoint(pid, addrEnd[i], &bp[i]);

        debug_print("Start sample %d\n", sampleCount);
        beginSample(&samples[sampleCount - flushedSampleCount]);
        sampleInProgress = 1;

        #if defined(__s390x__)
        if (WIFSTOPPED(status)) {
            // Z architecture advances 2 bytes the PC on SIGILL
            if (WSTOPSIG(status) == SIGILL) {
                displace_pc(pid, -2);
            }
        }
        #endif
        ret = ptrace(PTRACE_CONT, pid, 0, 0);
        if (ret != 0) { perror("ERROR: during tracing"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        ret = waitpid(pid, &status, 0);
        if (ret == -1) { perror("ERROR: during waiting"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        if (WIFSTOPPED(status)) {
            if (WSTOPSIG(status) != SIGILL) {
                fprintf(stderr, "WARNING: Process stopped during sampling: %s\n", strsignal(WSTOPSIG(status)));
            }
        }
        if (WIFEXITED(status)) {
            fprintf(stderr, "ERROR: Process exited during sampling: %d\n", WEXITSTATUS(status));
            return WEXITSTATUS(status);
        }
        if (WIFSIGNALED(status)) {
            debug_print("ERROR: Process signaled during sampling: %s\n", strsignal(WTERMSIG(status)));
            kill(pid, SIGKILL);
            exit(EXIT_FAILURE);
        }
        sampleInProgress = 0;

        debug_print("End sample %d\n", sampleCount);
        endSample(&samples[sampleCount - flushedSampleCount]);

        sampleCount++;

        if (sampleCount % MAX_SAMPLES == 0) {
            printSamples(outputFile, sampleCount - flushedSampleCount,
                         samples, printHeaders);
            printHeaders = 0;
            flushedSampleCount += MAX_SAMPLES;
        }

        for(int i=0; i<endpoint_count; i++) resetBreakpoint(pid, &bp[i]);
        for(int i=0; i<startpoint_count; i++) setBreakpoint(pid, addrStart[i], &bp[i]);
        #if defined(__s390x__)
        if (WIFSTOPPED(status)) {
            // Z architecture advances 2 bytes the PC on SIGILL
            if (WSTOPSIG(status) == SIGILL) {
                displace_pc(pid, -2);
            }
        }
        #endif
        ret = ptrace(PTRACE_CONT, pid, 0, 0);
        if (ret != 0) { perror("ERROR: during next tracing"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    }

    if (sampleCount == maxSamples) kill(pid, SIGTERM);
    return 0;
}

int globalPerformance(unsigned int timeout) {
    int status;
    beginSample(&samples[0]);
    sampleInProgress = 1;

    int ret =  ptrace(PTRACE_CONT, pid, 0, 0);
    if (ret != 0) { perror("ERROR while setting trace"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

    if (timeout > 0 ) {
        fprintf(stderr, "Timeout set to %d seconds\n", timeout);
        ret = alarm(timeout);
        if (ret != 0) { perror("ERROR while setting timeout"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    } else {
        fprintf(stderr, "No timeout set. Waiting process to finish\n");
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) { perror("ERROR while waiting"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

    sampleInProgress = 0;
    endSample(&samples[0]);
    sampleCount++;

    return status;
}

int main(int argc, char **argv) {
    if(argc < 2) help(stdout);

    unsigned long long addrStart[MAX_BREAKPOINTS];
    unsigned long long addrEnd[MAX_BREAKPOINTS];
    unsigned int maxSamples = UINT_MAX;
    unsigned int programStart = 0;
    unsigned int programStartSet = 0;
    unsigned int timeout = 0;
    unsigned int cpu = -1;
    char *output = NULL;
    addrStart[0] = 0;
    addrEnd[0] = 0;

    enum {
        EXPECTING_OPT, EXPECTING_ADDR_START, EXPECTING_ADDR_END,
        EXPECTING_MAX_SAMPLES, EXPECTING_PROGRAM, EXPECTING_OUTPUT,
        EXPECTING_TIMEOUT, EXPECTING_CPU, EXPECTING_MODULE
    } state = EXPECTING_OPT;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        switch (state) {
            case EXPECTING_OPT:
                if (strcmp(arg, "-begin") == 0) state = EXPECTING_ADDR_START;
                else if (strcmp(arg, "-end") == 0) state = EXPECTING_ADDR_END;
                else if (strcmp(arg, "-max") == 0) state = EXPECTING_MAX_SAMPLES;
                else if (strcmp(arg, "-o") == 0) state = EXPECTING_OUTPUT;
                else if (strcmp(arg, "-timeout") == 0) state = EXPECTING_TIMEOUT;
                else if (strcmp(arg, "-cpu") == 0) state = EXPECTING_CPU;
                else if (strcmp(arg, "-module") == 0) state = EXPECTING_MODULE;
                else if (strcmp(arg, "-h") == 0) help(stdout);
                else if (strcmp(arg, "--") == 0) {
                    state = EXPECTING_PROGRAM;
                    programStart = i+1;
                    programStartSet = 1;
                }
                else help(stderr);
                break;
            case EXPECTING_ADDR_START:
                addrStart[startpoint_count] = strtoull(argv[i], NULL, 16);
                startpoint_count++;
                if (startpoint_count > MAX_BREAKPOINTS) {
                    fprintf(stderr, "Maximum number of start breakpoints is %d\n", MAX_BREAKPOINTS);
                    exit(EXIT_FAILURE);
                }
                state = EXPECTING_OPT;
                break;
            case EXPECTING_ADDR_END:
                addrEnd[endpoint_count] = strtoull(argv[i], NULL, 16);
                endpoint_count++;
                if (endpoint_count > MAX_BREAKPOINTS) {
                    fprintf(stderr, "Maximum number of end breakpoints is %d\n", MAX_BREAKPOINTS);
                    exit(EXIT_FAILURE);
                }
                state = EXPECTING_OPT;
                break;
            case EXPECTING_MAX_SAMPLES:
                maxSamples = atoi(argv[i]);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_TIMEOUT:
                timeout = atoi(argv[i]);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_CPU:
                cpu = atoi(argv[i]);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_OUTPUT:
                output = argv[i];
                state = EXPECTING_OPT;
                break;
            case EXPECTING_MODULE:
                module = argv[i];
                state = EXPECTING_OPT;
                break;
            case EXPECTING_PROGRAM:
                break;
        }
        if (programStartSet){
            break;
        }
    }

    if (output == NULL) {
        fprintf(stderr, "Output is required. ");
        help(stderr);
    }

    if (programStartSet == 0) {
        fprintf(stderr, "Command to execute required. ");
        help(stderr);
    }

    if (programStart == argc) {
        fprintf(stderr, "Command to execute required. ");
        help(stderr);
    }

    if (cpu != -1) {
        cpu_set_t mask;

        CPU_ZERO(&mask);
        int ret = sched_getaffinity(0, sizeof(cpu_set_t), &mask);
        if (ret != 0) { perror("ERROR: while getting affinity"); exit(EXIT_FAILURE);};

        if (!CPU_ISSET(cpu, &mask)) {
            perror("ERROR: CPU specified is not allowed"); exit(EXIT_FAILURE);
        }

        CPU_ZERO(&mask);
        CPU_SET(cpu, &mask);
        fprintf(stderr, "INFO: Pinning process to CPU: %d\n", cpu);
        ret = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
        if (ret != 0) { perror("ERROR: while setting affinity"); exit(EXIT_FAILURE);};
    }

    fprintf(stderr, "INFO: Executing command: ");
    for (int i = programStart; i < argc; i++) fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "\n");

    mainmodule = argv[programStart];

    pid = fork();
    if (pid == 0) {
        // Child
        unsigned int numParams = argc - 3;
        char **newargs = malloc(sizeof(char*) * (numParams + 2));
        if (newargs == NULL) { perror("ERROR: copying args"); exit(EXIT_FAILURE);};
        memcpy(newargs, &argv[programStart], sizeof(char*) * (numParams + 1));
        newargs[numParams + 1] = NULL;
        long ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
        if (ret != 0) { perror("ERROR: setting traced process"); exit(EXIT_FAILURE);};

        int persona = personality(0xffffffff);
        if (persona == -1)
        {
            fprintf(stderr, "ERROR: Unable to get ASLR info: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        persona = persona | ADDR_NO_RANDOMIZE;
        debug_print("child: Disabling ASLR ...\n");
        persona = personality(persona | ADDR_NO_RANDOMIZE);
        if (persona == -1) {
            fprintf(stderr, "ERROR: Unable to set ASLR info: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (!(personality (0xffffffff) & ADDR_NO_RANDOMIZE))
        {
            fprintf(stderr, "ERROR: Unable to disable ASLR");
            exit(EXIT_FAILURE);
        }
        debug_print("child: ASLR disabled\n");

        setenv("LD_BIND_NOW", "1", 1);

        ret = execvp(argv[programStart], newargs);
        if (ret != 0) { perror("ERROR: executing process"); exit(EXIT_FAILURE);};
    } else {
        // Parent
        outputFile = (output != NULL ? fopen(output, "w") : NULL);
        assert(outputFile != NULL);

        struct sigaction sa;
        sa.sa_handler = handler;
        int ret = sigemptyset(&sa.sa_mask);
        if (ret != 0) { perror("ERROR: setting empty signals"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        sa.sa_flags = 0;
        ret = sigaction(SIGTERM, &sa, NULL);
        if (ret != 0) { perror("ERROR: setting SIGTERM"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        ret = sigaction(SIGINT, &sa, NULL);
        if (ret != 0) { perror("ERROR: setting SIGINT"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        ret = sigaction(SIGALRM, &sa, NULL);
        if (ret != 0) { perror("ERROR: setting SIALRM"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

        int status;
        debug_print("parent: Waiting for child...\n");
        ret = waitpid(pid, &status, 0); // Wait for child to start
        if (ret == -1) { perror("ERROR: waiting child to start"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        debug_print("parent: Waiting for child... OK\n");

        ret = ptrace(PTRACE_SYSCALL, pid, 0, 0);
        if (ret != 0) { perror("ERROR: while PTRACE_SYSCALL"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
        ret = waitpid(pid, &status, 0); // Wait for child to start
        if (ret == -1) { perror("ERROR: waiting child to execute first system call"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};

        compute_base_address(pid, module, mainmodule);
        configureEvents(pid);

        if (addrStart[0] > 0 && addrEnd[0] > 0 && endpoint_count > 0 && startpoint_count > 0) {
            fprintf(stderr, "INFO: Measuring performance counters from ");
            for(int i=0; i<startpoint_count; i++) fprintf(stderr, "0x%016llX ", addrStart[i] + base_address);
            fprintf(stderr, " to ");
            for(int i=0; i<endpoint_count; i++) fprintf(stderr, "0x%016llX ", addrEnd[i] + base_address);
            fprintf(stderr, "(max. samples: %u)\n", maxSamples);
            status = perInvocationPerformance(addrStart, addrEnd, maxSamples, outputFile);
        } else {
            fprintf(stderr, "INFO: Measuring performance counters from global execution\n");
            status = globalPerformance(timeout);
        }

        printSamples(outputFile, sampleCount - flushedSampleCount, samples,
                     printHeaders);

        if (outputFile != stderr)  {
            ret = fclose(outputFile);
            if (ret != 0) { perror("ERROR: closing file"); exit(EXIT_FAILURE);};
        }
        return status;
    }
}
