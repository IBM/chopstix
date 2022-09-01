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
#include "debug.h"

int pid;

unsigned long long max(unsigned long long x, unsigned long long y) {
    if (x>y) return x;
    return y;
}

unsigned long long min(unsigned long long x, unsigned long long y) {
    if (x>y) return y;
    return x;
}

void help(FILE *fd) {
    if (fd == stderr){
        fprintf(fd, "Wrong command line parameters\n");
        fprintf(fd, "\n");
    } else {
        fprintf(fd, "\n");
    }
    fprintf(fd, "Usage:\n");
    fprintf(fd, "\n");
    fprintf(fd, "chop-bp-checker -begin addr -end addr [-step size] [-h] -- command-to-execute\n");
    fprintf(fd, "\n");
    fprintf(fd, "-begin addr      start address of the region to check\n");
    fprintf(fd, "-end addr        end address of the region to check\n");
    fprintf(fd, "-step size       Step size in bytes for the check (default: 4)\n");
    fprintf(fd, "-h               print this help message\n");
    fprintf(fd, "\n");

    if (fd == stderr){
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
}


int main(int argc, char **argv) {
    if(argc < 2) help(stdout);

    unsigned long long addrStart = -1;
    unsigned long long addrEnd = 0;
    unsigned long long step = 4;
    unsigned int programStart = 0;
    unsigned int programStartSet = 0;
    char *output = NULL;

    enum {
        EXPECTING_OPT, EXPECTING_ADDR_START, EXPECTING_ADDR_END, EXPECTING_STEP,
        EXPECTING_PROGRAM
    } state = EXPECTING_OPT;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        switch (state) {
            case EXPECTING_OPT:
                if (strcmp(arg, "-begin") == 0) state = EXPECTING_ADDR_START;
                else if (strcmp(arg, "-end") == 0) state = EXPECTING_ADDR_END;
                else if (strcmp(arg, "-step") == 0) state = EXPECTING_STEP;
                else if (strcmp(arg, "-h") == 0) help(stdout);
                else if (strcmp(arg, "--") == 0) {
                    state = EXPECTING_PROGRAM;
                    programStart = i+1;
                    programStartSet = 1;
                }
                else help(stderr);
                break;
            case EXPECTING_ADDR_START:
                addrStart = min(addrStart, strtoull(argv[i], NULL, 16));
                state = EXPECTING_OPT;
                break;
            case EXPECTING_ADDR_END:
                addrEnd = max(addrEnd, strtoull(argv[i], NULL, 16));
                state = EXPECTING_OPT;
                break;
            case EXPECTING_STEP:
                step = atoi(argv[i]);
                state = EXPECTING_OPT;
                break;
            case EXPECTING_PROGRAM:
                break;
        }
        if (programStartSet){
            break;
        }
    }

    if (programStartSet == 0) {
        fprintf(stderr, "Command to execute required. ");
        help(stderr);
    }

    if (programStart == argc) {
        fprintf(stderr, "Command to execute required. ");
        help(stderr);
    }

    fprintf(stderr, "Executing '");
    for (int i = programStart; i < argc; i++) fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "'\n");

    pid = fork();
    if (pid == 0) {
        unsigned int numParams = argc - 3;
        char **newargs = malloc(sizeof(char*) * (numParams + 2));
        if (newargs == NULL) { perror("ERROR copying args"); exit(EXIT_FAILURE);};
        memcpy(newargs, &argv[programStart], sizeof(char*) * (numParams + 1));
        newargs[numParams + 1] = NULL;
        long ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
        if (ret != 0) { perror("ERROR setting traced process"); exit(EXIT_FAILURE);};

		int persona = personality(0xffffffff);                                         
		if (persona == -1)                                                             
		{                                                                              
			fprintf(stderr, "ERROR Unable to get ASLR info: %s\n", strerror(errno));
			exit(EXIT_FAILURE);                                                        
		}                                                                              
																					   
		persona = persona | ADDR_NO_RANDOMIZE;                                         
		debug_print("Disabling ASLR ...\n");                              
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
		debug_print("ASLR disabled\n");                                   

        ret = execvp(argv[programStart], newargs);
        if (ret != 0) { perror("ERROR executing process"); exit(EXIT_FAILURE);};
    } else {

        int status;
        int ret = waitpid(pid, &status, 0); // Wait for child to start
        if (ret == -1) { perror("ERROR waiting child to start"); exit(EXIT_FAILURE);};

        Breakpoint bp;

        unsigned long long baddr = addrStart;
        unsigned long long addr = addrStart;
        int bread = readBreakpoint(pid, baddr, &bp);
        int bwrite = writeBreakpoint(pid, baddr, &bp);
        for (addr = addrStart; addr <= addrEnd; addr += step) {
            int read = readBreakpoint(pid, addr, &bp);
            int write = writeBreakpoint(pid, addr, &bp);
            printf("%016llX\n", addr);
            if ((bread == read) && (bwrite == write)) {
                // Same protection, keep going
                continue;
            } else {
                printf("From %016llX to %016llX Read: %d Write: %d\n", baddr, addr - step, bread, bwrite);
                bread = read;
                bwrite = write;
                baddr = addr;
            }
        }
        printf("From %016llX to %016llX Read: %d Write: %d\n", baddr, addr, bread, bwrite);

        ret = kill(pid, SIGKILL);
        if (ret == -1) { perror("ERROR unable to send the kill signal"); exit(EXIT_FAILURE);};
        ret = waitpid(pid, &status, 0); // Wait for child to start
        if (ret == -1) { perror("ERROR unable to kill the process"); exit(EXIT_FAILURE);};

        exit(EXIT_SUCCESS);
    }
}
