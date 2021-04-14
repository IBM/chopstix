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
 * NAME        : arch/power.h
 * DESCRIPTION : Architecture-specific implementation for IBM POWER.
 ******************************************************************************/

#pragma once

#include "core/arch.h"

namespace chopstix {

struct ArchPower : public Arch {
    std::string name() const { return "POWER"; }
    std::vector<std::string> prefix() const {
        return {"powerpc64", "powerpc64le", "ppc64", "ppc64le"};
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
    void set_args(pid_t pid, std::vector<unsigned long> &args) const;
    long parse_syscall(regbuf_type regs) const;
    long parse_ret(regbuf_type regs) const;
    void parse_args(regbuf_type regs, regbuf_type args) const;
};

}  // namespace chopstix
