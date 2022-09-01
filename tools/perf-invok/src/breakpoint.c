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

#include "debug.h"
#include "breakpoint.h"
#include <stdio.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#if defined(__s390x__)
#include <sys/uio.h>
#endif

// Breakpoint size, in bytes. Should be the smallest amount of bytes required
// for a breakpoint by a given architecture.

#if defined(__riscv) // RISC-V (Valid with or without C extension)
#define BREAKPOINT_SIZE 2
#elif defined(__s390x__) // System Z
#define BREAKPOINT_SIZE 2
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64) // PPC64
#define BREAKPOINT_SIZE 4
#else // Default
#define BREAKPOINT_SIZE 4
#endif

void setBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint) {
    breakpoint->address = address;
    debug_print("setBreakpoint 0x%016llX to 0x%08X (orig 0x%08llX)\n", address, 0, breakpoint->originalData);
    debug_print("setBreakpoint %p\n", (void *) address);
    errno = 0;
    breakpoint->originalData = ptrace(PTRACE_PEEKTEXT, pid, address, NULL);
    if (errno != 0) {
        switch (errno) {
            case EBUSY:
                debug_print("Error: EBUSY: %s\n", strerror(errno));
                break;
            case EFAULT:
                debug_print("Error: EFAULT: %s\n", strerror(errno));
                break;
            case EINVAL:
                debug_print("Error: EINVAL: %s\n", strerror(errno));
                break;
            case EIO:
                debug_print("Error: EIO: %s\n", strerror(errno));
                break;
            case EPERM:
                debug_print("Error: EPERM: %s\n", strerror(errno));
                break;
            case ESRCH:
                debug_print("Error: ESRCH: %s\n", strerror(errno));
                break;
            default:
                debug_print("Error: %s\n", strerror(errno));
                break;
        }
        perror("ERROR while setting breakpoint (read)"); exit(EXIT_FAILURE);
    };
#if defined(__s390x__)
    unsigned long long mask = 0x0000FFFFFFFFFFFF;
#else
    unsigned long long mask = ~(BREAKPOINT_SIZE == sizeof(long long) ? -1 : ((1llu << (BREAKPOINT_SIZE * 8)) - 1));
#endif
    debug_print("Breakpoint mask: 0x%016llX\n", mask);
    debug_print("Breakpoint previous data: 0x%016llX\n", breakpoint->originalData);
    debug_print("Breakpoint new data: 0x%016llX\n", breakpoint->originalData & mask);
    errno = 0;
    long ret = ptrace(PTRACE_POKEDATA, pid, address, breakpoint->originalData & mask);
    if (ret != 0) { perror("ERROR while setting breakpoint (write)"); exit(EXIT_FAILURE);};
}

void resetBreakpoint(unsigned long pid, Breakpoint *breakpoint) {
    debug_print("resetBreakpoint 0x%016llX to 0x%08llX\n", breakpoint->address, breakpoint->originalData);
    errno = 0;
    long ret = ptrace(PTRACE_POKEDATA, pid, breakpoint->address, breakpoint->originalData);
    if (ret != 0) { perror("ERROR while restoring breakpoint"); exit(EXIT_FAILURE);};
}

#if defined(__s390x__)
void displace_pc(long pid, long displ) {
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;

    errno = 0;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    if (ret != 0) { perror("ERROR while reading PC"); exit(EXIT_FAILURE);};
    long pc = buf[1];

	debug_print("displace_pc from 0x%016lX to 0x%016lX\n", pc, pc + displ);

    buf[1] = pc + displ;
    errno = 0;
    ret = ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov);
    if (ret != 0) { perror("ERROR while setting PC"); exit(EXIT_FAILURE);};
}
#endif
