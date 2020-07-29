/******************************************************************************
 * NAME        : arch/riscv.h
 * DESCRIPTION : Architecture-specific implementation for RISC-V.
 ******************************************************************************/

#pragma once

#include "core/arch.h"

namespace chopstix {

struct ArchRiscV : public Arch {
    std::string name() const { return "RiscV"; }
    std::vector<std::string> prefix() const {
        return {"riscv64"};
    }
    Endianess get_endianess() const {return Endianess::LITTLE;}

    void parse_inst(Instruction &) const;

    size_t regsize() const;
    void serialize_regs(FILE *fp, regbuf_type regs) const;

    void read_regs(pid_t pid, regbuf_type regbuf) const;
    void write_regs(pid_t pid, regbuf_type regbuf) const;
    long get_pc(pid_t pid) const;
    long set_pc(pid_t pid, long pc) const;
    long get_sp(pid_t pid) const;
    long set_sp(pid_t pid, long sp) const;
    long get_lnk(pid_t pid) const;
    long parse_syscall(regbuf_type regs) const;
    long parse_ret(regbuf_type regs) const;
    void parse_args(regbuf_type regs, regbuf_type args) const;
};

}  // namespace chopstix
