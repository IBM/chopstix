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
#include "support/log.h"

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

size_t ArchZ::regsize() const { return sizeof(struct ArchZRegs)/sizeof(unsigned long int); }

void ArchZ::serialize_regs(FILE *os, Arch::regbuf_type regs) const {
    log::debug("ArchZ::serialize_regs start");
    struct ArchZRegs *azregs = (struct ArchZRegs *) regs;
    fprintf(os, "PSW_MASK 0x%016lX\n", azregs->gp.psw.mask);
    fprintf(os, "PSW_ADDR 0x%016lX\n", azregs->gp.psw.addr);
    for (int i=0; i<16; i++) {
        fprintf(os, "GR%d 0x%016lX\n", i, azregs->gp.all[i+2]);
    }
    fprintf(os, "AR%d 0x%08X\n", 0, azregs->gp.ac0);
    fprintf(os, "AR%d 0x%08X\n", 1, azregs->gp.ac1);
    fprintf(os, "AR%d 0x%08X\n", 2, azregs->gp.ac2);
    fprintf(os, "AR%d 0x%08X\n", 3, azregs->gp.ac3);
    fprintf(os, "AR%d 0x%08X\n", 4, azregs->gp.ac4);
    fprintf(os, "AR%d 0x%08X\n", 5, azregs->gp.ac5);
    fprintf(os, "AR%d 0x%08X\n", 6, azregs->gp.ac6);
    fprintf(os, "AR%d 0x%08X\n", 7, azregs->gp.ac7);
    fprintf(os, "AR%d 0x%08X\n", 8, azregs->gp.ac8);
    fprintf(os, "AR%d 0x%08X\n", 9, azregs->gp.ac9);
    fprintf(os, "AR%d 0x%08X\n", 10, azregs->gp.ac10);
    fprintf(os, "AR%d 0x%08X\n", 11, azregs->gp.ac11);
    fprintf(os, "AR%d 0x%08X\n", 12, azregs->gp.ac12);
    fprintf(os, "AR%d 0x%08X\n", 13, azregs->gp.ac13);
    fprintf(os, "AR%d 0x%08X\n", 14, azregs->gp.ac14);
    fprintf(os, "AR%d 0x%08X\n", 15, azregs->gp.ac15);

    fprintf(os, "FPC 0x%08X\n", azregs->fp.fpc);

    for (int i=0; i<16; i++) {
        fprintf(os, "FPR%d 0x%016lX\n", i, azregs->fp.all[i+1].ui);
    }

    for (int i=0; i<16; i++) {
        fprintf(os, "VR%d 0x%016lX%016lX\n", i, azregs->vr.all[i].fld.high.ui, azregs->vr.all[i].fld.low.ui);
    }

    for (int i=16; i<32; i++) {
        fprintf(os, "VR%d 0x%016lX%016lX\n", i, azregs->vr.all[i].fld.low.ui, azregs->vr.all[i].fld.high.ui);
    }
}

void ArchZ::debug_regs(Arch::regbuf_type regs) const {
    log::debug("ArchZ::debug_regs start");
    struct ArchZRegs *azregs = (struct ArchZRegs *) regs;
    log::debug("ArchZ::debug_regs: PSW_MASK 0x%x", azregs->gp.psw.mask);
    log::debug("ArchZ::debug_regs: PSW_ADDR 0x%x", azregs->gp.psw.addr);
    for (int i=0; i<16; i++) {
        log::debug("ArchZ::debug_regs: GR%d 0x%x", i, azregs->gp.all[i+2]);
    }
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 0, azregs->gp.ac0);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 1, azregs->gp.ac1);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 2, azregs->gp.ac2);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 3, azregs->gp.ac3);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 4, azregs->gp.ac4);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 5, azregs->gp.ac5);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 6, azregs->gp.ac6);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 7, azregs->gp.ac7);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 8, azregs->gp.ac8);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 9, azregs->gp.ac9);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 10, azregs->gp.ac10);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 11, azregs->gp.ac11);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 12, azregs->gp.ac12);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 13, azregs->gp.ac13);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 14, azregs->gp.ac14);
    log::debug("ArchZ::debug_regs: AR%d 0x%x", 15, azregs->gp.ac15);

    log::debug("ArchZ::debug_regs: FPC 0x%x", azregs->fp.fpc);

    for (int i=0; i<16; i++) {
        log::debug("ArchZ::debug_regs: FPR%d 0x%x", i, azregs->fp.all[i+1].ui);
    }

    for (int i=0; i<16; i++) {
        log::debug("ArchZ::debug_regs: VR%d 0x%x 0x%x", i, azregs->vr.all[i].fld.high.ui, azregs->vr.all[i].fld.low.ui);
    }

    for (int i=16; i<32; i++) {
        log::debug("ArchZ::debug_regs: VR%d 0x%x 0x%x", i, azregs->vr.all[i].fld.low.ui, azregs->vr.all[i].fld.high.ui);
    }
    log::debug("ArchZ::debug_end start");
}

void ArchZ::read_regs(pid_t pid, regbuf_type regs) const {
    log::debug("ArchZ::read_regs start");

    // General purpose registers
    struct iovec data = {
        .iov_base = &((struct ArchZRegs *) regs)->gp,
        .iov_len  = ARCHZ_GPR_SIZE
    };
    long ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_GP_REGISTERS, &data);
    check(ret >= 0, "Unable to read registers");

    // Floating point registers
    data = {
        .iov_base = &((struct ArchZRegs *) regs)->fp,
        .iov_len  = ARCHZ_FPR_SIZE
    };
    ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_FP_REGISTERS, &data);
    check(ret >= 0, "Unable to read registers");

    // Vector registers
    data = {
        .iov_base = &((struct ArchZRegs *) regs)->vr.vr16,
        .iov_len  = ARCHZ_VXR2_SIZE
    };
    ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_VXR2_REGISTERS, &data);
    check(ret >= 0, "Unable to read registers");

    data = {
        .iov_base = &((struct ArchZRegs *) regs)->vr,
        .iov_len  = ARCHZ_VXR1_SIZE
    };
    ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_VXR1_REGISTERS, &data);
    check(ret >= 0, "Unable to read registers");

    //
    // Fix and check the contents readed
    //
    ((struct ArchZRegs *) regs)->vr.vr15.fld.low = ((struct ArchZRegs *) regs)->vr.low[15];
    ((struct ArchZRegs *) regs)->vr.vr14.fld.low = ((struct ArchZRegs *) regs)->vr.low[14];
    ((struct ArchZRegs *) regs)->vr.vr13.fld.low = ((struct ArchZRegs *) regs)->vr.low[13];
    ((struct ArchZRegs *) regs)->vr.vr12.fld.low = ((struct ArchZRegs *) regs)->vr.low[12];
    ((struct ArchZRegs *) regs)->vr.vr11.fld.low = ((struct ArchZRegs *) regs)->vr.low[11];
    ((struct ArchZRegs *) regs)->vr.vr10.fld.low = ((struct ArchZRegs *) regs)->vr.low[10];
    ((struct ArchZRegs *) regs)->vr.vr9.fld.low = ((struct ArchZRegs *) regs)->vr.low[9];
    ((struct ArchZRegs *) regs)->vr.vr8.fld.low = ((struct ArchZRegs *) regs)->vr.low[8];
    ((struct ArchZRegs *) regs)->vr.vr7.fld.low = ((struct ArchZRegs *) regs)->vr.low[7];
    ((struct ArchZRegs *) regs)->vr.vr6.fld.low = ((struct ArchZRegs *) regs)->vr.low[6];
    ((struct ArchZRegs *) regs)->vr.vr5.fld.low = ((struct ArchZRegs *) regs)->vr.low[5];
    ((struct ArchZRegs *) regs)->vr.vr4.fld.low = ((struct ArchZRegs *) regs)->vr.low[4];
    ((struct ArchZRegs *) regs)->vr.vr3.fld.low = ((struct ArchZRegs *) regs)->vr.low[3];
    ((struct ArchZRegs *) regs)->vr.vr2.fld.low = ((struct ArchZRegs *) regs)->vr.low[2];
    ((struct ArchZRegs *) regs)->vr.vr1.fld.low = ((struct ArchZRegs *) regs)->vr.low[1];
    ((struct ArchZRegs *) regs)->vr.vr0.fld.low = ((struct ArchZRegs *) regs)->vr.low[0];

    ((struct ArchZRegs *) regs)->vr.vr15.fld.high = ((struct ArchZRegs *) regs)->fp.fp15;
    ((struct ArchZRegs *) regs)->vr.vr14.fld.high = ((struct ArchZRegs *) regs)->fp.fp14;
    ((struct ArchZRegs *) regs)->vr.vr13.fld.high = ((struct ArchZRegs *) regs)->fp.fp13;
    ((struct ArchZRegs *) regs)->vr.vr12.fld.high = ((struct ArchZRegs *) regs)->fp.fp12;
    ((struct ArchZRegs *) regs)->vr.vr11.fld.high = ((struct ArchZRegs *) regs)->fp.fp11;
    ((struct ArchZRegs *) regs)->vr.vr10.fld.high = ((struct ArchZRegs *) regs)->fp.fp10;
    ((struct ArchZRegs *) regs)->vr.vr9.fld.high = ((struct ArchZRegs *) regs)->fp.fp9;
    ((struct ArchZRegs *) regs)->vr.vr8.fld.high = ((struct ArchZRegs *) regs)->fp.fp8;
    ((struct ArchZRegs *) regs)->vr.vr7.fld.high = ((struct ArchZRegs *) regs)->fp.fp7;
    ((struct ArchZRegs *) regs)->vr.vr6.fld.high = ((struct ArchZRegs *) regs)->fp.fp6;
    ((struct ArchZRegs *) regs)->vr.vr5.fld.high = ((struct ArchZRegs *) regs)->fp.fp5;
    ((struct ArchZRegs *) regs)->vr.vr4.fld.high = ((struct ArchZRegs *) regs)->fp.fp4;
    ((struct ArchZRegs *) regs)->vr.vr3.fld.high = ((struct ArchZRegs *) regs)->fp.fp3;
    ((struct ArchZRegs *) regs)->vr.vr2.fld.high = ((struct ArchZRegs *) regs)->fp.fp2;
    ((struct ArchZRegs *) regs)->vr.vr1.fld.high = ((struct ArchZRegs *) regs)->fp.fp1;
    ((struct ArchZRegs *) regs)->vr.vr0.fld.high = ((struct ArchZRegs *) regs)->fp.fp0;

}

void ArchZ::write_regs(pid_t pid, regbuf_type regs) const {
    log::debug("ArchZ::write_regs start");

    // General purpose registers
    struct iovec data = {
        .iov_base = &((struct ArchZRegs *) regs)->gp,
        .iov_len  = ARCHZ_GPR_SIZE
    };
    long ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_GP_REGISTERS, &data);
    check(ret >= 0, "Unable to write registers");

    // Floating point registers
    data = {
        .iov_base = &((struct ArchZRegs *) regs)->fp,
        .iov_len  = ARCHZ_FPR_SIZE
    };
    ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_FP_REGISTERS, &data);
    check(ret >= 0, "Unable to write registers");

    // Vector registers
    data = {
        .iov_base = &((struct ArchZRegs *) regs)->vr.vr16,
        .iov_len  = ARCHZ_VXR2_SIZE
    };
    ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_VXR2_REGISTERS, &data);
    check(ret >= 0, "Unable to write registers");

    freg_t low[16];
    low[15] = ((struct ArchZRegs *) regs)->vr.vr15.fld.low;
    low[14] = ((struct ArchZRegs *) regs)->vr.vr14.fld.low;
    low[13] = ((struct ArchZRegs *) regs)->vr.vr13.fld.low;
    low[12] = ((struct ArchZRegs *) regs)->vr.vr12.fld.low;
    low[11] = ((struct ArchZRegs *) regs)->vr.vr11.fld.low;
    low[10] = ((struct ArchZRegs *) regs)->vr.vr10.fld.low;
    low[9] = ((struct ArchZRegs *) regs)->vr.vr9.fld.low;
    low[8] = ((struct ArchZRegs *) regs)->vr.vr8.fld.low;
    low[7] = ((struct ArchZRegs *) regs)->vr.vr7.fld.low;
    low[6] = ((struct ArchZRegs *) regs)->vr.vr6.fld.low;
    low[5] = ((struct ArchZRegs *) regs)->vr.vr5.fld.low;
    low[4] = ((struct ArchZRegs *) regs)->vr.vr4.fld.low;
    low[3] = ((struct ArchZRegs *) regs)->vr.vr3.fld.low;
    low[2] = ((struct ArchZRegs *) regs)->vr.vr2.fld.low;
    low[1] = ((struct ArchZRegs *) regs)->vr.vr1.fld.low;
    low[0] = ((struct ArchZRegs *) regs)->vr.vr0.fld.low;

    data = {
        .iov_base = low,
        .iov_len  = ARCHZ_VXR1_SIZE
    };
    ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_VXR1_REGISTERS, &data);
    check(ret >= 0, "Unable to write registers");

}

long ArchZ::get_pc(pid_t pid) const {
    log::debug("ArchZ::get_pc start");
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    check(ret >= 0, "Unable to get PC");
    return buf[1];
}

long ArchZ::set_pc(pid_t pid, long pc) const {
    log::debug("ArchZ::set_pc start");
    long buf[2];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    check(ret >= 0, "Unable to get PC");
    long old_pc = buf[1];
    buf[1] = pc;
    ret = ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov);

    log::debug("ArchZ::set_pc old PC: 0x%x new PC: 0x%x", old_pc, pc);

    check(ret >= 0, "Unable to set PC");
    return old_pc;
}

long ArchZ::get_sp(pid_t pid) const {
    log::debug("ArchZ::get_sp start");
    long buf[18];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    check(ret >= 0, "Unable to get SP");
    return buf[17];
}

long ArchZ::set_sp(pid_t pid, long sp) const {
    log::debug("ArchZ::set_sp start");
    long buf[18];
    struct iovec iov;
    iov.iov_len = sizeof(buf);
    iov.iov_base = buf;
    long ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
    check(ret >= 0, "Unable to get SP");
    long old_sp = buf[17];
    buf[17] = sp;
    ret = ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov);
    check(ret >= 0, "Unable to set SP");
    return old_sp;
}

long ArchZ::get_lnk(pid_t pid) const {
    log::debug("ArchZ::get_lnk start");
    struct ArchZRegs azregs;
    read_regs(pid, (long*) &azregs);
    return azregs.gp.r14;
}

long ArchZ::parse_syscall(regbuf_type regs) const {
    log::debug("ArchZ::parse_syscall start");
    struct ArchZRegs *azregs = (struct ArchZRegs *) regs;
    return azregs->gp.r2;
}

long ArchZ::parse_ret(regbuf_type regs) const {
    log::debug("ArchZ::parse_ret start");
    struct ArchZRegs *azregs = (struct ArchZRegs *) regs;
    return azregs->gp.r2;
}

void ArchZ::parse_args(regbuf_type regs, regbuf_type args) const {
    struct ArchZRegs *azregs = (struct ArchZRegs *) regs;
    args[0] = azregs->gp.r1 + 1;  // TODO this is a hack
    args[1] = azregs->gp.r3;
    args[2] = azregs->gp.r4;
    args[3] = azregs->gp.r5;
    args[4] = azregs->gp.r6;
    args[5] = azregs->gp.r7;
}

void ArchZ::set_args(pid_t pid, std::vector<unsigned long> &args) const {
    log::debug("ArchZ::set_args start");
    check(args.size() <= 5, "Cannot set more than 5 argument registers.");
    struct ArchZRegs regs;
    read_regs(pid, (long*) &regs);
    for (size_t i = 0; i < args.size(); i++) {
        regs.gp.all[4 + i] = args[i];
    }
    write_regs(pid, (long*) &regs);
}
