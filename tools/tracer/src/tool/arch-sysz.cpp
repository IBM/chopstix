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
#include "regs-sysz.h"

#include <sys/ptrace.h>
#include <sys/uio.h>

#define REGBUF_SIZE (sizeof(long) * SYSZ_NUM_REGS)

namespace cxtrace {
namespace arch {

regbuf_type create_regs() { return (regbuf_type)malloc(REGBUF_SIZE); }

void read_regs(pid_t pid, regbuf_type regs) {
    struct iovec iov;
    iov.iov_len = REGBUF_SIZE;
    iov.iov_base = regs;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to read registers");
}

void write_regs(pid_t pid, regbuf_type regs) {
    struct iovec iov;
    iov.iov_len = REGBUF_SIZE;
    iov.iov_base = regs;
    long ret = ptrace(PTRACE_SETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to write registers");
}

long get_lnk(pid_t pid) {
    assert(0 && "Not implemented");
    return 0;
}

long get_sp(pid_t pid) {
    long buf[18];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get SP");
    return buf[17];
}

long set_sp(pid_t pid, long sp) {
    long buf[18];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get SP");
    long old_sp = buf[17];
    buf[17] = sp;
    ret = ptrace(PTRACE_SETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to set SP");
    return old_sp;
}

long get_pc(pid_t pid) {
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get PC");
    return buf[1];
}

long set_pc(pid_t pid, long pc) {
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get PC");
    long old_pc = buf[1];
    buf[1] = pc;
    ret = ptrace(PTRACE_SETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to set PC");
    return old_pc;
}

void dump_regs(FILE *os, regbuf_type regs) {
    // for (int i = 0; i < SYSZ_NUM_REGS; ++i) {
    for (int i = 0; i < 18; ++i) {
        fprintf(os, "%s %016lx\n", sysz_regnames[i], regs[i]);
    }
}

long get_syscall(regbuf_type regs) { return regs[SYSZ_R2]; }

long get_ret(regbuf_type regs) { return regs[SYSZ_R2]; }

void get_args(regbuf_type regs, regbuf_type args) {
    args[0] = regs[SYSZ_R1] + 1;  // TODO this is a hack
    args[1] = regs[SYSZ_R3];
    args[2] = regs[SYSZ_R4];
    args[3] = regs[SYSZ_R5];
    args[4] = regs[SYSZ_R6];
    args[5] = regs[SYSZ_R7];
}
}
}
