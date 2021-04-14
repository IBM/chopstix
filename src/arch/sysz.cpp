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
 * NAME        : arch/sysz.cpp
 * DESCRIPTION : Architecture-specific implementation for IBM System Z.
 ******************************************************************************/

#include "arch/sysz.h"
#include "arch/sysz-regs.h"

#include "support/check.h"
#include "support/string.h"

#include "core/branch.h"
#include <sys/ptrace.h>

#include <sys/uio.h>

using namespace chopstix;

#define REG Branch::REGISTER
#define COND Branch::CONDITION
#define LINK Branch::LINK
#define REL Branch::RELATIVE

namespace {

struct branch_map {
    std::string mnem;
    long opcode;
    long mask;
    int size;
    int flags;
};

const std::vector<branch_map> branches = {
    //    MNEM     OPCODE     MASK          FLAGS
    {"bakr", 0xB2400000, 0xFFFF0000, 4, REG | LINK},  // BRANCH AND STACK
    {"bal", 0x45000000, 0xFF000000, 4, LINK},         // BRANCH AND LINK
    {"balr", 0x0500, 0xFF00, 2, REG | LINK},
    {"bas", 0x4D000000, 0xFF000000, 4, LINK},  // BRANCH AND SAVE
    {"basr", 0x0D00, 0xFF00, 2, REG | LINK},
    {"bassm", 0x0C00, 0xFF00, 2, REG | LINK},  // BRANCH AND SAVE SET MODE
    {"bc", 0x47000000, 0xFF000000, 4, COND},   // BRANCH ON CONDITION
    {"bcr", 0x0700, 0xFF00, 2, REG | COND},
    {"bct", 0x46000000, 0xFF000000, 4, COND},  // BRANCH ON COUNT
    {"bctg", 0xE30000000046, 0xFF00000000FF, 6, COND},
    {"bctgr", 0xB9460000, 0xFFFF0000, 4, REG | COND},
    {"bctr", 0x0600, 0xFF00, 2, REG | COND},
    {"bras", 0xA7050000, 0xFF0F0000, 4,
     LINK | REL},  // BRANCH RELATIVE AND SAVE
    {"brasl", 0xC00400000000, 0xFF0F00000000, 6,
     LINK | REL},  // BRANCH RELATIVE AND SAVE LONG
    {"brc", 0xA7040000, 0xFF0F0000, 4,
     COND | REL},  // BRANCH RELATIVE ON CONDITION
    {"brcl", 0xC00400000000, 0xFF0F00000000, 6,
     COND | REL},  // BRANCH RELATIVE ON CONDITION LONG
    {"brct", 0xA7060000, 0xFF0F0000, 4,
     COND | REL},  // BRANCH RELATIVE ON COUNT
    {"brctg", 0xA7070000, 0xFF0F0000, 4, COND | REL},
    {"brcth", 0xCC0600000000, 0xFF0F00000000, 6,
     COND | REL},  // BRANCH RELATIVE ON COUNT HIGH
    {"brxh", 0x84000000, 0xFF000000, 4,
     COND | REL},  // BRANCH RELATIVE ON INDEX HIGH
    {"brxhg", 0xEC0000000044, 0xFF00000000FF, 6, COND | REL},
    {"brxle", 0x85000000, 0xFF000000, 4,
     COND | REL},  // BRANCH RELATIVE ON INDEX LOW OR EQUAL
    {"brxlg", 0xEC0000000045, 0xFF00000000FF, 6, COND | REL},
    {"bxh", 0x86000000, 0xFF000000, 4, COND},  // BRANCH ON INDEX HIGH
    {"bxhg", 0xEB0000000044, 0xFF00000000FF, 6, COND},
    {"bxle", 0x87000000, 0xFF000000, 4, COND},  // BRANCH ON INDEX LOW OR EQUAL
    {"bxleg", 0xEB0000000045, 0xFF00000000FF, 6, COND},
    // == Compare and branch
    {"crb", 0xEC00000000F6, 0xFF00000000FF, 6, COND},  // COMPARE AND BRANCH
    {"cgrb", 0xEC00000000E4, 0xFF00000000FF, 6, COND},
    {"crj", 0xEC0000000076, 0xFF00000000FF, 6,
     COND | REL},  // COMPARE AND BRANCH RELATIVE
    {"cgrj", 0xEC0000000064, 0xFF00000000FF, 6, COND | REL},
    {"cib", 0xEC00000000FE, 0xFF00000000FF, 6,
     COND},  // COMPARE IMMEDIATE AND BRANCH
    {"cgib", 0xEC00000000FC, 0xFF00000000FF, 6, COND},
    {"cij", 0xEC000000007E, 0xFF00000000FF, 6,
     COND | REL},  // COMPARE IMMEDIATE AND BRANCH RELATIVE
    {"cgij", 0xEC000000007C, 0xFF00000000FF, 6, COND | REL},
    {"clrb", 0xEC00000000F7, 0xFF00000000FF, 6,
     COND},  // COMPARE LOGICAL AND BRANCH
    {"clgrb", 0xEC00000000E5, 0xFF00000000FF, 6, COND},
    {"clrj", 0xEC0000000077, 0xFF00000000FF, 6,
     COND | REL},  // COMPARE LOGICAL AND BRANCH RELATIVE
    {"clgrj", 0xEC0000000065, 0xFF00000000FF, 6, COND | REL},
    {"clib", 0xEC00000000FF, 0xFF00000000FF, 6,
     COND},  // COMPARE LOGICAL IMMEDIATE AND BRANCH
    {"clgib", 0xEC00000000FD, 0xFF00000000FF, 6, COND},
    {"clij", 0xEC000000007F, 0xFF00000000FF, 6,
     COND | REL},  // COMPARE LOGICAL IMMEDIATE AND BRANCH RELATIVE
    {"clgij", 0xEC000000007D, 0xFF00000000FF, 6, COND | REL},
    // == Execute
    {"ex", 0x44000000, 0xFF000000, 4, REG},            // EXECUTE
    {"exrl", 0xC60000000000, 0xFF0000000000, 6, REL},  // EXECUTE RELATIVE LONG
    // == Traps
    {"crt", 0xB9720000, 0xFFFF0000, 4, 0},  // COMPARE AND TRAP
    {"cgrt", 0xB9600000, 0xFFFF0000, 4, 0},
    {"cit", 0xEC0000000072, 0xFF00000000FF, 6,
     0},  // COMPARE IMMEDIATE AND TRAP
    {"cgit", 0xEC0000000070, 0xFF00000000FF, 6, 0},
    {"clrt", 0xB9730000, 0xFFFF0000, 4, 0},  // COMPARE LOGICAL AND TRAP
    {"clgrt", 0xB9610000, 0xFFFF0000, 4, 0},
    {"clt", 0xEB0000000023, 0xFF00000000FF, 6, 0},
    {"clgt", 0xEB000000002B, 0xFF00000000FF, 6, 0},
    {"clfit", 0xEC0000000073, 0xFF00000000FF, 6,
     0},  // COMPARE LOGICAL IMMEDIATE AND TRAP
    {"clgit", 0xEC0000000071, 0xFF00000000FF, 6, 0},
    {"lat", 0xE3000000009F, 0xFF00000000FF, 6, 0},  // LOAD AND TRAP
    {"lgat", 0xE30000000085, 0xFF00000000FF, 6, 0},
    {"lfhat", 0xE300000000C8, 0xFF00000000FF, 6, 0},   // LOAD HIGH AND TRAP
    {"llgfat", 0xE3000000009D, 0xFF00000000FF, 6, 0},  // LOAD LOGICAL AND TRAP
    {"llgtat", 0xE3000000009C, 0xFF00000000FF, 6,
     0},  // LOAD LOGICAL THIRTY ONE BITS AND TRAP
};
}  // namespace

void ArchZ::parse_inst(Instruction &ins) const {
    long opcode = ins.opcode();
    int len = ins.raw.size() / 2;
    bool is_branch = false;
    int flags;

    for (auto &br : branches) {
        if (len != br.size) continue;
        long opmasked = opcode & br.mask;
        if (opmasked == br.opcode) {
            is_branch = true;
            flags = br.flags;
            break;
        }
    }

    if (!is_branch) return;

    Branch br{ins.addr, 0, flags};

    auto ops = ins.operands();

    if (!ops.empty() && !br.reg()) {
        std::stringstream ss(ops.back());
        ss >> std::hex >> br.target;
    }

    ins.branch = std::make_shared<Branch>(br);
}

// void ArchZ::insert_trap(long &raw) const { failx("Not implemented"); }

size_t ArchZ::regsize() const { return SYSZ_NUM_REGS; }

#define REGBUF_SIZE (sizeof(long) * SYSZ_NUM_REGS)

void ArchZ::serialize_regs(FILE *os, Arch::regbuf_type regs) const {
    for (int i = 0; i < SYSZ_NUM_REGS; ++i) {
        fprintf(os, "%s %016lx\n", sysz_regnames[i], regs[i]);
    }
}

void ArchZ::read_regs(pid_t pid, regbuf_type regs) const {
    struct iovec iov;
    iov.iov_len = REGBUF_SIZE;
    iov.iov_base = regs;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to read registers");
}

void ArchZ::write_regs(pid_t pid, regbuf_type regs) const {
    struct iovec iov;
    iov.iov_len = REGBUF_SIZE;
    iov.iov_base = regs;
    long ret = ptrace(PTRACE_SETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to write registers");
}

long ArchZ::get_pc(pid_t pid) const {
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get PC");
    return buf[1];
}

long ArchZ::set_pc(pid_t pid, long pc) const {
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

long ArchZ::get_sp(pid_t pid) const {
    long buf[18];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, 1, &iov);
    check(ret >= 0, "Unable to get SP");
    return buf[17];
}

long ArchZ::set_sp(pid_t pid, long sp) const {
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

long ArchZ::get_lnk(pid_t pid) const { failx("Not implemented"); }

long ArchZ::parse_syscall(regbuf_type regs) const { return regs[SYSZ_R2]; }
long ArchZ::parse_ret(regbuf_type regs) const { return regs[SYSZ_R2]; }
void ArchZ::parse_args(regbuf_type regs, regbuf_type args) const {
    args[0] = regs[SYSZ_R1] + 1;  // TODO this is a hack
    args[1] = regs[SYSZ_R3];
    args[2] = regs[SYSZ_R4];
    args[3] = regs[SYSZ_R5];
    args[4] = regs[SYSZ_R6];
    args[5] = regs[SYSZ_R7];
}

void ArchZ::set_args(pid_t pid, std::vector<unsigned long> &args) const {
    failx("Not implemented");
}
