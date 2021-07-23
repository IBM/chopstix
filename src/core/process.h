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
 * NAME        : core/process.h
 * DESCRIPTION :
 ******************************************************************************/

#pragma once

#include <functional>
#include <map>

#include "core/arch.h"
#include "core/location.h"

namespace chopstix {

typedef struct {
    long addr;
    long original_content;
} BreakpointInformation;

class Process {
  public:
    // using callback_fn = std::function<void(void)>;
    // using break_map = std::map<long, long>;
    // using callback_map = std::map<long, callback_fn>;
    using breakpoint_cache = std::map<long, long>;

    Process(int pid = -1) : pid_(pid), status_(0) {}
    ~Process();

    Process(const Process &) = delete;
    Process &operator=(const Process &) = delete;
    Process(Process &&);
    Process &operator=(Process &&);

    long pid() const { return pid_; }

    void exec(char **argv, int argc);
    void exec_wait(char **argv, int argc);
    void abandon();
    void copy(long pid);

    bool active() const { return pid_ != -1; }

    void send(int sig);
    void touch();
    void ready();
    void wait(int flags = 0);
    void waitfor(int sig);
    void expect(int sig);

    bool exited();
    int exit_status();
    bool signaled();
    int term_sig();
    bool stopped();
    int stop_sig();

    void syscall(int sig = 0);
    void cont(int sig = 0);
    void attach();
    void detach(int sig = 0);
    void step(int sig = 0);
    void steps(long steps);

    long peek(long addr);
    void poke(long addr, long data);

    void set_break(long addr);
    void remove_break(long addr);

    void timeout(double time);

    void dyn_call(long addr, Arch::regbuf_type &regs, long sp, std::vector<unsigned long> &args);

    Location find_symbol(const std::string &name) const {
        return Location::Symbol(pid(), name);
    }

    Location find_symbol(const std::string &sym, const std::string &mod) {
        return Location::Symbol(pid_, sym, mod);
    }

    Location find_module(const std::string &name) const {
        return Location::Module(pid(), name);
    }

    std::vector<BreakpointInformation> get_breakpoint_info() {
        std::vector<BreakpointInformation> infos;
        for (auto brk : this->breaks_)
            infos.push_back({brk.first, brk.second});
        return infos;
    }

    void *get_segfault_addr();

  private:
    int pid_;
    int status_;

    breakpoint_cache breaks_;
};
}  // namespace chopstix
