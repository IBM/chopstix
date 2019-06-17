/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
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
#include "arch.h"

#include "common/check.h"
#include "regs-power.h"

#include <sys/ptrace.h>

// #define REGBUF_SIZE (sizeof(long) * POWER_NUM_REGS)

#ifndef PTRACE_GETREGS
#define PTRACE_GETREGS (__ptrace_request) 12
#endif

#ifndef PTRACE_SETREGS
#define PTRACE_SETREGS (__ptrace_request) 13
#endif

#define REGBUF_SIZE (sizeof(long) * (POWER_VSR31+1))

namespace cxtrace {
namespace arch {

regbuf_type create_regs() { return (regbuf_type)malloc(REGBUF_SIZE); }

void read_regs(pid_t pid, regbuf_type regs) {
    long ret = ptrace(PTRACE_GETREGS, pid, 0, regs);
    check(ret >= 0, "Unable to read registers");
}

void write_regs(pid_t pid, regbuf_type regs) {
    long ret = ptrace(PTRACE_SETREGS, pid, 0, regs);
    check(ret >= 0, "Unable to write registers");
}

long get_lnk(pid_t pid) {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_LNK];
}

long get_sp(pid_t pid) {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_R1];
}

long set_sp(pid_t pid, long sp) {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    long old_sp = buf[POWER_R1];
    buf[POWER_R1] = sp;
    write_regs(pid, buf);
    return old_sp;
}

long get_pc(pid_t pid) {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_NIP];
}

long set_pc(pid_t pid, long pc) {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    long old_pc = buf[POWER_NIP];
    buf[POWER_NIP] = pc;
    buf[POWER_R12] = pc; // Check ABI
    write_regs(pid, buf);
    return old_pc;
}

void dump_regs(FILE *os, regbuf_type regs) {
    // for (int i = 0; i < POWER_NUM_REGS; ++i) {
    //     fprintf(os, "%s %016lx\n", power_regnames[i], regs[i]);
    // }
    
    for (int i = 0; i < 32; ++i) { // General (64-bit)
        fprintf(os, "GPR%d 0x%016lx\n", i, regs[POWER_R0+i]);
    }
    fprintf(os, "CR 0x%08lx\n", regs[POWER_CCR]); // (32-bit)
    fprintf(os, "MSR 0x%08lx\n", regs[POWER_MSR]); // (32-bit)
    // fprintf(os, "NIP 0x%016lx\n", regs[POWER_NIP]); // (64-bit)
    for (int i = 0; i < 32; ++i) { // Floating-Point (64-bit)
        fprintf(os, "FPR%d 0x%016lx\n", i, regs[POWER_FPR0+i]);
    }
    fprintf(os, "FPSCR 0x%08lx\n", regs[POWER_FPSCR]);
    for (int i = 0; i < 32; ++i) { // Vector (128-bit)
        fprintf(os, "VR%d 0x%016lx%016lx\n", i,
            regs[POWER_VR0 + i*2], regs[POWER_VR0 + i*2 + 1]);
    }
    fprintf(os, "VSCR 0x%08lx\n", regs[POWER_VSCR]); // 32-bit
    for (int i = 0; i < 32; ++i) { // Vector Scalar (128-bit)
        fprintf(os, "VSR%d 0x%016lx%016lx\n", i,
            regs[POWER_VSR0 + i*2], regs[POWER_VSR0 + i*2 + 1]);
    }
}

long get_syscall(regbuf_type regs) {
    return regs[POWER_R0];
}

long get_ret(regbuf_type regs) {
    return regs[POWER_R3];
}

void get_args(regbuf_type regs, regbuf_type args) {
    args[0] = regs[POWER_R3];
    args[1] = regs[POWER_R4];
    args[2] = regs[POWER_R5];
    args[3] = regs[POWER_R6];
    args[4] = regs[POWER_R7];
    args[5] = regs[POWER_R8];
}

}
} 
