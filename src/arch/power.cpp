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
/******************************************************************************
 * NAME        : arch/power.cpp
 * DESCRIPTION : Architecture-specific implementation for IBM POWER.
 ******************************************************************************/

#include "arch/power.h"
#include "arch/power-regs.h"

#include "core/branch.h"
#include "core/instruction.h"

#include "support/check.h"
#include "support/string.h"

#include "fmt/printf.h"

#include <sys/ptrace.h>

#include <map>

using namespace chopstix;

#define REG Branch::REGISTER
#define COND Branch::CONDITION
#define LINK Branch::LINK

#define X3(M)                                                              \
    {M, COND}, {M "l", COND | LINK}, {M "a", COND}, {M "la", COND | LINK}, \
        {M "lr", REG | COND}, {                                            \
        M "lrl", REG | COND | LINK                                         \
    }

#define X4(M) \
    X3(M), {M "ctr", REG | COND}, { M "ctrll", REG | COND | LINK }

namespace {
const std::map<std::string, int> branches = {
    //    OPCODE      FLAGS
    // Basic branches
    {"b", 0},
    {"ba", 0},
    {"bl", LINK},
    {"bla", LINK},
    {"bc", COND},
    {"bca", COND},
    {"bclr", REG | COND},
    {"bcctr", REG | COND},
    {"bcl", COND | LINK},
    {"bcla", COND | LINK},
    {"bclrl", REG | COND | LINK},
    {"bcctrl", REG | COND | LINK},
    // Simple mnemonics
    {"blr", REG},
    {"bctr", REG},
    {"blrl", REG | LINK},
    {"bctrl", REG | LINK},
    X4("bt"),
    X4("bf"),
    X3("bdnz"),
    X3("bdnzt"),
    X3("bdnzf"),
    X3("bdz"),
    X3("bdzt"),
    X3("bdzf"),
    // Extended mnemonics
    X4("blt"),
    X4("ble"),
    X4("beq"),
    X4("bge"),
    X4("bgt"),
    X4("bnl"),
    X4("bne"),
    X4("bng"),
    X4("bso"),
    X4("bns"),
    X4("bun"),
    X4("bnu"),
};

}  // namespace

void ArchPower::parse_inst(Instruction &ins) const {
    auto mnem = ins.mnemonic();
    if (mnem.back() == '+' || mnem.back() == '-') {
        mnem = mnem.substr(0, mnem.size() - 1);
    }
    auto it = branches.find(mnem);
    if (it == branches.end()) return;

    auto ops = ins.operands();

    Branch br{ins.addr, 0, it->second};

    if (!ops.empty() && !br.reg()) {
        std::stringstream ss(ops.back());
        ss >> std::hex >> br.target;
    }

    ins.branch = std::make_shared<Branch>(br);
}

#ifndef PTRACE_GETREGS
#define PTRACE_GETREGS (__ptrace_request)12
#endif

#ifndef PTRACE_SETREGS
#define PTRACE_SETREGS (__ptrace_request)13
#endif

// #define REGBUF_SIZE (sizeof(long) * (POWER_VSR31 + 1))

size_t ArchPower::regsize() const { return POWER_VSR31 + 1; }

void ArchPower::serialize_regs(FILE *os, Arch::regbuf_type regs) const {
    for (int i = 0; i < 32; ++i) {  // General (64-bit)
        fprintf(os, "GPR%d 0x%016lx\n", i, regs[POWER_R0 + i]);
    }
    fprintf(os, "CR 0x%08lx\n", regs[POWER_CCR]);   // (32-bit)
    fprintf(os, "MSR 0x%08lx\n", regs[POWER_MSR]);  // (32-bit)
    // fprintf(os, "NIP 0x%016lx\n", regs[POWER_NIP]); // (64-bit)
    for (int i = 0; i < 32; ++i) {  // Floating-Point (64-bit)
        fprintf(os, "FPR%d 0x%016lx\n", i, regs[POWER_FPR0 + i]);
    }
    fprintf(os, "FPSCR 0x%08lx\n", regs[POWER_FPSCR]);
    for (int i = 0; i < 32; ++i) {  // Vector (128-bit)
        fprintf(os, "VR%d 0x%016lx%016lx\n", i, regs[POWER_VR0 + i * 2],
                regs[POWER_VR0 + i * 2 + 1]);
    }
    fprintf(os, "VSCR 0x%08lx\n", regs[POWER_VSCR]);  // 32-bit
    for (int i = 0; i < 32; ++i) {                    // Vector Scalar (128-bit)
        fprintf(os, "VSR%d 0x%016lx%016lx\n", i, regs[POWER_VSR0 + i * 2],
                regs[POWER_VSR0 + i * 2 + 1]);
    }
}

void ArchPower::read_regs(pid_t pid, regbuf_type regs) const {
    long ret = ptrace(PTRACE_GETREGS, pid, 0, regs);
    check(ret >= 0, "ArchPower:: read_regs: Unable to read registers");
}

void ArchPower::write_regs(pid_t pid, regbuf_type regs) const {
    long ret = ptrace(PTRACE_SETREGS, pid, 0, regs);
    check(ret >= 0, "ArchPower:: write_regs: Unable to write registers");
}

long ArchPower::get_pc(pid_t pid) const {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_NIP];
}

long ArchPower::set_pc(pid_t pid, long pc) const {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    long old_pc = buf[POWER_NIP];
    buf[POWER_NIP] = pc;
    buf[POWER_R12] = pc;  // Check ABI
    write_regs(pid, buf);
    return old_pc;
}

long ArchPower::get_sp(pid_t pid) const {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_R1];
}

long ArchPower::set_sp(pid_t pid, long sp) const {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    long old_sp = buf[POWER_R1];
    buf[POWER_R1] = sp;
    write_regs(pid, buf);
    return old_sp;
}

long ArchPower::get_lnk(pid_t pid) const {
    long buf[POWER_NUM_REGS];
    read_regs(pid, buf);
    return buf[POWER_LNK];
}

long ArchPower::parse_syscall(regbuf_type regs) const { return regs[POWER_R0]; }

long ArchPower::parse_ret(regbuf_type regs) const { return regs[POWER_R3]; }

void ArchPower::parse_args(regbuf_type regs, regbuf_type args) const {
    args[0] = regs[POWER_R3];
    args[1] = regs[POWER_R4];
    args[2] = regs[POWER_R5];
    args[3] = regs[POWER_R6];
    args[4] = regs[POWER_R7];
    args[5] = regs[POWER_R8];
}

void ArchPower::set_args(pid_t pid, std::vector<unsigned long> &args) const {
    failx("Not implemented");
}
