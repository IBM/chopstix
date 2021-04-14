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
 * NAME        : arch/x86.cpp
 * DESCRIPTION : Architecture-specific implemnetation for x86.
 ******************************************************************************/

#include "arch/x86.h"

#include "support/check.h"

#include "core/branch.h"
#include "core/instruction.h"

#include "support/log.h"

#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/user.h>

using namespace chopstix;

void ArchX86::parse_inst(Instruction &inst) const {
    auto mnem = inst.mnemonic();

    if (mnem.empty()) return;

    Branch br;
    br.source = inst.addr;

    if (mnem.find("jm") == 0) {
        // Nothing to do here
    } else if (mnem.find("j") == 0) {
        br.set(Branch::CONDITION);
    } else if (mnem.find("call") == 0) {
        br.set(Branch::LINK);
    } else if (mnem.find("ret") == 0) {
        br.set(Branch::REGISTER);
    } else if (mnem.find("repz") == 0) {
        auto ops = inst.operands();
        if (ops.empty()) return;
        if (ops.front().find("ret") != 0) return;
        br.set(Branch::REGISTER);
    } else {
        return;
    }

    if (!br.reg()) {
        auto ops = inst.operands();
        if (ops.empty()) return;
        if (ops.front().front() == '*') {
            br.set(Branch::REGISTER);
        } else {
            br.target = std::stoull(ops.front(), nullptr, 16);
        }
    }

    inst.branch = std::make_shared<Branch>(br);
}

// void ArchX86::insert_trap(long &raw) const {
//    failx("Not implemented for '%s'", name());
// }

size_t ArchX86::regsize() const {
    return sizeof(struct user_regs_struct) + sizeof(struct user_fpregs_struct);
}


void ArchX86::serialize_regs(FILE *os, Arch::regbuf_type regs) const {
    failx("Not implemented for '%s'", name());
}

void ArchX86::read_regs(pid_t pid, regbuf_type regs) const {
    long ret = ptrace(PTRACE_GETREGS, pid, 0, regs);
    check(ret >= 0, "ArchX86:: read_regs: Unable to read registers");
}

void ArchX86::write_regs(pid_t pid, regbuf_type regs) const {
    long ret = ptrace(PTRACE_SETREGS, pid, 0, regs);
    check(ret >= 0, "ArchX86:: write_regs: Unable to write registers");
}

long ArchX86::get_pc(pid_t pid) const {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    return regs.rip;
}

long ArchX86::set_pc(pid_t pid, long pc) const {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    long old_pc = regs.rip;
    regs.rip = pc;
    ptrace(PTRACE_SETREGS, pid, 0, &regs);
    return old_pc;
}

long ArchX86::get_sp(pid_t pid) const { failx("Not implemented"); }
long ArchX86::set_sp(pid_t pid, long sp) const { failx("Not implemented"); }
long ArchX86::get_lnk(pid_t pid) const { failx("Not implemented"); }
long ArchX86::parse_syscall(regbuf_type regs) const {
    failx("Not implemented");
}
long ArchX86::parse_ret(regbuf_type regs) const { failx("Not implemented"); }
void ArchX86::parse_args(regbuf_type regs, regbuf_type args) const {
    failx("Not implemented");
}
void ArchX86::set_args(pid_t pid, std::vector<unsigned long> &args) const {
    failx("Not implemented");
}
