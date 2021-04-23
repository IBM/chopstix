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
 * NAME        : core/arch.h
 * DESCRIPTION : Abstract base class for architecture-specific implementations
 ******************************************************************************/

#pragma once

#include "support/popen.h"

#include "instruction.h"

#include "config.h"

#include <unistd.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <limits>

namespace chopstix {

struct Instruction;

enum class Endianess {
    LITTLE,
    BIG
};

enum class BreakpointSize {
    HALF_WORD,
    WORD,
    DOUBLE_WORD,
};

struct Arch {
    using regbuf_type = long *;
    using impl_ptr = std::unique_ptr<Arch>;

    static const impl_ptr &current() {
        static impl_ptr arch = get_impl();
        return arch;
    }

    virtual void parse_inst(Instruction &) const = 0;
    virtual std::string name() const = 0;
    virtual std::vector<std::string> prefix() const = 0;
    virtual Endianess get_endianess() const = 0;
    virtual long get_breakpoint_mask() const;

    virtual size_t regsize() const = 0;
    regbuf_type create_regs() const { return new long[regsize()]; }
    virtual void serialize_regs(FILE *fp, regbuf_type regs) const = 0;

    static void free_regs(regbuf_type regs) { delete[] regs; }

    static std::string get_machine();
    static impl_ptr get_impl(std::string name = get_machine());
    Popen objdump(const std::string &filename) const;

    virtual void read_regs(pid_t pid, regbuf_type regbuf) const = 0;
    virtual void write_regs(pid_t pid, regbuf_type regbuf) const = 0;
    virtual long get_pc(pid_t pid) const = 0;
    virtual long set_pc(pid_t pid, long pc) const = 0;
    virtual long get_sp(pid_t pid) const = 0;
    virtual long set_sp(pid_t pid, long sp) const = 0;
    virtual long get_lnk(pid_t pid) const = 0;
    virtual void set_args(pid_t pid, std::vector<unsigned long> &args) const = 0;
    virtual long parse_syscall(regbuf_type regs) const = 0;
    virtual long parse_ret(regbuf_type regs) const = 0;
    virtual void parse_args(regbuf_type regs, regbuf_type args) const = 0;

protected:
    virtual BreakpointSize get_breakpoint_size() const {
        return BreakpointSize::DOUBLE_WORD;
    }
};

}  // namespace chopstix
