#include "arch/riscv.h"
#include "arch/riscv-regs.h"
#include <sys/ptrace.h>
#include "support/check.h"
#include <stdio.h>
#include <sys/uio.h>
#include <map>

#define PTRACE_GP_REGISTERS 1
#define PTRACE_FP_REGISTERS 2

using namespace chopstix;

const std::map<std::string, int> branches = {
    //Base Instructions
    {"jal",  Branch::RELATIVE | Branch::LINK},
    {"jalr", Branch::REGISTER | Branch::LINK},
    {"beq",  Branch::RELATIVE | Branch::CONDITION},
    {"bne",  Branch::RELATIVE | Branch::CONDITION},
    {"blt",  Branch::RELATIVE | Branch::CONDITION},
    {"bge",  Branch::RELATIVE | Branch::CONDITION},
    {"bltu", Branch::RELATIVE | Branch::CONDITION},
    {"bgeu", Branch::RELATIVE | Branch::CONDITION},
    //Pseudo Instructions
    {"j",   Branch::RELATIVE},
    {"jr",  Branch::REGISTER},
    {"ret", Branch::REGISTER}
};

void ArchRiscV::parse_inst(Instruction &ins) const {
    auto it = branches.find(ins.mnemonic());
    if (it == branches.end()) return;

    auto ops = ins.operands();

    Branch br{ins.addr, 0, it->second};

    if (!ops.empty() && !br.reg()) {
        std::stringstream ss(ops.back());
        ss >> std::hex >> br.target;
    }

    ins.branch = std::make_shared<Branch>(br);
}

size_t ArchRiscV::regsize() const {
    return sizeof(struct RiscVRegs)/sizeof(unsigned long int);
}

void ArchRiscV::serialize_regs(FILE *os, Arch::regbuf_type regs) const {
    struct RiscVRegs *rvregs = (struct RiscVRegs *) regs;
    fprintf(os, "x%d 0x%016lx\n", 0, (long unsigned int) 0); //First GPR is always zero
    for (int i = 1; i < 32; ++i) {  // General (64-bit)
        fprintf(os, "x%d 0x%016lx\n", i, rvregs->gp.all[i]);
    }
    for (int i = 0; i < 32; ++i) {  // Floating-Point (64-bit)
        fprintf(os, "f%d 0x%016lx\n", i, rvregs->fp.all[i]);
    }
}

long ArchRiscV::parse_syscall(regbuf_type regs) const {
    return ((struct RiscVRegs *) regs)->gp.a7;
}

long ArchRiscV::parse_ret(regbuf_type regs) const {
    return ((struct RiscVRegs *) regs)->gp.a0;
}

void ArchRiscV::parse_args(regbuf_type regs, regbuf_type args) const {
    struct RiscVRegs *rvregs = (struct RiscVRegs *) regs;
    args[0] = rvregs->gp.a0;
    args[1] = rvregs->gp.a1;
    args[2] = rvregs->gp.a2;
    args[3] = rvregs->gp.a3;
    args[4] = rvregs->gp.a4;
    args[5] = rvregs->gp.a5;
    args[6] = rvregs->gp.a6;
    args[7] = rvregs->gp.a7;
}

void ArchRiscV::read_regs(pid_t pid, regbuf_type regs) const {
    long ret;

    struct iovec data = {
        .iov_base = &((struct RiscVRegs *) regs)->gp,
        .iov_len  = RISCV_GPR_SIZE
    };
    ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_GP_REGISTERS, &data);
    check(ret >= 0, "ArchRiscV:: read_regs: Unable to read GP registers");

    data = {
        .iov_base = &((struct RiscVRegs *) regs)->fp,
        .iov_len  = RISCV_FPR_SIZE
    };
    ret = ptrace(PTRACE_GETREGSET, pid, PTRACE_FP_REGISTERS, &data);
    check(ret >= 0, "ArchRiscV:: read_regs: Unable to read FP registers");
}

void ArchRiscV::write_regs(pid_t pid, regbuf_type regs) const {
    long ret;

    struct iovec data = {
        .iov_base = &((struct RiscVRegs *) regs)->gp,
        .iov_len  = RISCV_GPR_SIZE
    };
    ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_GP_REGISTERS, &data);
    check(ret >= 0, "ArchRiscV:: write_regs: Unable to write GP registers");

    data = {
        .iov_base = &((struct RiscVRegs *) regs)->fp,
        .iov_len  = RISCV_FPR_SIZE
    };
    ret = ptrace(PTRACE_SETREGSET, pid, PTRACE_FP_REGISTERS, &data);
    check(ret >= 0, "ArchRiscV:: write_regs: Unable to write FP registers");
}

long ArchRiscV::get_pc(pid_t pid) const {
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    return regs.gp.pc;
}

long ArchRiscV::set_pc(pid_t pid, long pc) const {
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    long old_pc = regs.gp.pc;
    regs.gp.pc = pc;
    write_regs(pid, (long*) &regs);
    return old_pc;
}

long ArchRiscV::get_sp(pid_t pid) const {
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    return regs.gp.sp;
}

long ArchRiscV::set_sp(pid_t pid, long sp) const {
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    long old_sp = regs.gp.sp;
    regs.gp.sp = sp;
    write_regs(pid, (long*) &regs);
    return old_sp;
}

long ArchRiscV::get_lnk(pid_t pid) const {
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    return regs.gp.ra;
}

void ArchRiscV::set_args(pid_t pid, std::vector<unsigned long> &args) const {
    check(args.size() <= 8, "Cannot set more than 8 argument registers.");
    struct RiscVRegs regs;
    read_regs(pid, (long*) &regs);
    for (size_t i = 0; i < args.size(); i++) {
        regs.gp.all[10 + i] = args[i];
    }
    write_regs(pid, (long*) &regs);
}
