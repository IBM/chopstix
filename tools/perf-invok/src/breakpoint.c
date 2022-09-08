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
#include <sys/user.h>
#include <sys/uio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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

#if defined(__riscv)
struct RiscVRegs {
    union {
        struct {
            unsigned long int pc;
            unsigned long int ra;
            unsigned long int sp;
            unsigned long int gp;
            unsigned long int tp;
            unsigned long int t0;
            unsigned long int t1;
            unsigned long int t2;
            unsigned long int s0;
            unsigned long int s1;
            unsigned long int a0;
            unsigned long int a1;
            unsigned long int a2;
            unsigned long int a3;
            unsigned long int a4;
            unsigned long int a5;
            unsigned long int a6;
            unsigned long int a7;
            unsigned long int s2;
            unsigned long int s3;
            unsigned long int s4;
            unsigned long int s5;
            unsigned long int s6;
            unsigned long int s7;
            unsigned long int s8;
            unsigned long int s9;
            unsigned long int s10;
            unsigned long int s11;
            unsigned long int t3;
            unsigned long int t4;
            unsigned long int t5;
            unsigned long int t6;
        };
        unsigned long int all[32];
    } gp;
    union {
        unsigned long int all[32];
    } fp;
};

#define RISCV_GPR_SIZE sizeof(((struct  RiscVRegs *)0)->gp)
#define RISCV_FPR_SIZE sizeof(((struct  RiscVRegs *)0)->fp)
#endif

unsigned long long base_address = 0;
unsigned int base_address_set = 0;
unsigned long long basemain_address[2] = {0,0};

void setBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint) {
    address = address + base_address;
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
        perror("ERROR while setting breakpoint (read)"); kill(pid, SIGKILL); exit(EXIT_FAILURE);
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
    if (ret != 0) { perror("ERROR while setting breakpoint (write)"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
}

void resetBreakpoint(unsigned long pid, Breakpoint *breakpoint) {
    debug_print("resetBreakpoint 0x%016llX to 0x%08llX\n", breakpoint->address, breakpoint->originalData);
    errno = 0;
    long ret = ptrace(PTRACE_POKEDATA, pid, breakpoint->address, breakpoint->originalData);
    if (ret != 0) { perror("ERROR while restoring breakpoint"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
}

void compute_base_address(unsigned long pid, char* module, char* mainmodule) {
    if (base_address_set == 1) return;
    if (strncmp(module, "main", 4) == 0) { base_address_set = 1; return;}

    char path[1024];
    char* line = NULL;
    char* addr;
    char* perm;
    char* cmodule;
    size_t len = 0;
    ssize_t read = 0;
    snprintf(path, 1024, "/proc/%ld/maps", pid);
    FILE* fp = fopen(path, "r");
    if (fp == NULL) { kill(pid, SIGKILL); exit(EXIT_FAILURE); }
    mainmodule = basename(mainmodule);

    while ((read = getline(&line, &len, fp)) != -1) {
        addr = strtok(line, " ");
        if(addr == NULL) continue;
        perm = strtok(NULL, " ");
        if(perm == NULL) continue;
        if(perm[2] != 'x') continue;
        strtok(NULL, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");
        cmodule = strtok(NULL, " \n");
        if(cmodule == NULL) continue;
        cmodule = basename(cmodule);
        if(strncmp(mainmodule, cmodule, strlen(module)) != 0) continue;
        addr = strtok(addr, "-");
        if(addr == NULL) continue;
        basemain_address[0] = strtoll(addr, NULL, 16);
        addr = strtok(NULL, "-");
        if(addr == NULL) continue;
        basemain_address[1] = strtoll(addr, NULL, 16);
        break;
    }

    unsigned long long caddr = 0;
    while(1) {
        ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
#if defined(__riscv) // RISC-V (Valid with or without C extension)
        struct RiscVRegs regs;
        struct iovec data = {
            .iov_base = &((struct RiscVRegs *) regs)->gp,
            .iov_len  = RISCV_GPR_SIZE
        };
        ptrace(PTRACE_GETREGSET, pid, PTRACE_GP_REGISTERS, &data);
        caddr = regs.gp.pc;
#elif defined(__s390x__)
        long buf[2];
        struct iovec iov;
        iov.iov_len = sizeof(buf);
        iov.iov_base = buf;
        ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
        caddr = buf[1];
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64) // PPC64
#define POWER_NUM_REGS 44
#define POWER_NIP 32
#ifndef PTRACE_GETREGS
#define PTRACE_GETREGS (__ptrace_request)12
#endif
        long buf[POWER_NUM_REGS];
        ptrace(PTRACE_GETREGS, pid, 0, buf);
        caddr = buf[POWER_NIP];
#elif defined(__x86_64__) || defined(__i386__)
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, 0, &regs);
        caddr = regs.rip;
#endif
        if ((caddr >= basemain_address[0]) && (caddr <basemain_address[1])) { break; };
    }

    fclose(fp);
    fp = fopen(path, "r");
    if (fp == NULL) { kill(pid, SIGKILL); exit(EXIT_FAILURE); }

    while ((read = getline(&line, &len, fp)) != -1) {
        addr = strtok(line, " ");
        if(addr == NULL) continue;
        perm = strtok(NULL, " ");
        if(perm == NULL) continue;
        if(perm[2] != 'x') continue;
        strtok(NULL, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");
        cmodule = strtok(NULL, " \n");
        if(cmodule == NULL) continue;
        cmodule = basename(cmodule);
        if(strncmp(module, cmodule, strlen(module)) != 0) continue;
        addr = strtok(addr, "-");
        if(addr == NULL) continue;
        base_address_set = 1;
        base_address = strtoll(addr, NULL, 16);
        break;
    }

    fclose(fp);

}

#if defined(__s390x__)
void displace_pc(long pid, long displ) {
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;

    errno = 0;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    if (ret != 0) { perror("ERROR while reading PC"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
    long pc = buf[1];

	debug_print("displace_pc from 0x%016lX to 0x%016lX\n", pc, pc + displ);

    buf[1] = pc + displ;
    errno = 0;
    ret = ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov);
    if (ret != 0) { perror("ERROR while setting PC"); kill(pid, SIGKILL); exit(EXIT_FAILURE);};
}
#endif
