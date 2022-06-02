/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 - 2022 IBM Corporation
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
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include "debug.h"
#include "tool.h"
#include "limits.h"
#include "modes.h"

void help(FILE *fd);
void parseArguments(int argc, char **argv);

static unsigned long long addrStart[MAX_BREAKPOINTS] = {0};
static unsigned long long addrEnd[MAX_BREAKPOINTS] = {0};
static unsigned int maxSamples = UINT_MAX;
static unsigned int programStart = 0;
static unsigned int programStartSet = 0;
static unsigned int timeout = 0;
static unsigned int cpu = 0;
static unsigned int endPointCount = 0;
static unsigned int startPointCount = 0;
static float rate = 0.0;
static char *output = NULL;

int main(int argc, char **argv) {

    parseArguments(argc, argv);

    perfInvokInit(output, cpu, maxSamples, argc - programStart, &argv[programStart]);

    if (addrStart[0] > 0 && addrEnd[0] > 0 && endPointCount > 0 && startPointCount > 0) {
        fprintf(stderr, "Measuring performance counters from ");
        for(int i=0; i<startPointCount; i++) fprintf(stderr, "0x%llx ", addrStart[i]);
        fprintf(stderr, " to ");
        for(int i=0; i<endPointCount; i++) fprintf(stderr, "0x%llx ", addrEnd[i]);
        fprintf(stderr, "(max. samples: %u)\n", maxSamples);
        measureInvocationPerformance(addrStart, startPointCount, addrEnd, endPointCount);
    } else {
        fprintf(stderr, "Measuring performance counters from global execution\n");
        measureGlobalPerformance(timeout, rate);
    }

    perfInvokExit(EXIT_SUCCESS);

    return -1; // Unreachable
}

void help(FILE *fd) {
    if (fd == stderr) fprintf(fd, "Wrong command line parameters\n");

    fprintf(fd, "\n");
    fprintf(fd, "Usage:\n");
    fprintf(fd, "\n");
    fprintf(fd, "chop-perf-invok -o output [-begin addr] [-begin addr] ...] [-end addr [-end addr ...]] [-timeout seconds] [-max samples] [-cpu cpu] [-rate rate] [-h] -- command-to-execute\n");
    fprintf(fd, "\n");
    fprintf(fd, "-o name          output file name\n");
    fprintf(fd, "-begin addr      start address of the region to measure\n");
    fprintf(fd, "-end addr        address of the region to measure (can be specified multiple times)\n");
    fprintf(fd, "-timeout seconds stop measuring after the specified number of seconds (default: no timeout)\n");
    fprintf(fd, "-max samples     stop measuring after the specified number of measurements(default: no limit)\n");
    fprintf(fd, "-cpu cpu         pin process to the specified CPU (default: 0)\n");
    fprintf(fd, "-rate rate       Poll the performance counters at the specified rate in Hertz (default: no rate, measure once from start to end\n");
    fprintf(fd, "-h:              print this help message\n");
    fprintf(fd, "\n");

    exit(fd == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

void parseArguments(int argc, char **argv) {
    if(argc < 2) help(stdout);

    enum {
        EXPECTING_OPT, EXPECTING_ADDR_START, EXPECTING_ADDR_END,
        EXPECTING_MAX_SAMPLES, EXPECTING_PROGRAM, EXPECTING_OUTPUT,
        EXPECTING_TIMEOUT, EXPECTING_CPU, EXPECTING_RATE
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
                else if (strcmp(arg, "-rate") == 0) state = EXPECTING_RATE;
                else if (strcmp(arg, "-h") == 0) help(stdout);
                else if (strcmp(arg, "--") == 0) {
                    state = EXPECTING_PROGRAM;
                    programStart = i+1;
                    programStartSet = 1;
                }
                else help(stderr);
                break;
            case EXPECTING_ADDR_START:
                addrStart[startPointCount] = strtoull(argv[i], NULL, 16);
                startPointCount++;
                if (startPointCount > MAX_BREAKPOINTS) {
                    fprintf(stderr, "Maximum number of start breakpoints is %d\n", MAX_BREAKPOINTS);
                    exit(EXIT_FAILURE);
                }
                state = EXPECTING_OPT;
                break;
            case EXPECTING_ADDR_END:
                addrEnd[endPointCount] = strtoull(argv[i], NULL, 16);
                endPointCount++;
                if (endPointCount > MAX_BREAKPOINTS) {
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
            case EXPECTING_RATE:
                rate = strtof(argv[i], NULL);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_PROGRAM:
                break;
        }
        if (programStartSet){
            break;
        }
    }

    // Check required arguments

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
}
