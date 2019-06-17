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
#pragma once

#include "common/popen.h"
#include <ostream>
#include <string>

namespace cxtrace {
namespace arch {

using regbuf_type = long *;
regbuf_type create_regs();

void dump_regs(FILE *os, regbuf_type);

void read_regs(pid_t pid, regbuf_type);
void write_regs(pid_t pid, regbuf_type);

long set_pc(pid_t pid, long pc);
long get_pc(pid_t pid);

long set_sp(pid_t pid, long sp);
long get_sp(pid_t pid);

long get_lnk(pid_t pid);

long get_syscall(regbuf_type regs);
long get_ret(regbuf_type regs);
void get_args(regbuf_type regs, regbuf_type args);
}
}
