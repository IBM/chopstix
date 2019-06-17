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

#include <unistd.h>

#include <map>

#include "arch.h"
#include "common/location.h"

namespace cxtrace {
struct Process {
  public:
    using breakpoint_cache = std::map<long, long>;

    Process(pid_t pid = -1) : pid_(pid), status_(0) {}
    ~Process();

    Process(const Process &) = delete;
    Process &operator=(const Process &) = delete;
    Process(Process &&);
    Process &operator=(Process &&);

    pid_t pid() const { return pid_; }
    int status() { return status_; }

    void exec(char **argv);
    bool active() const { return pid_ != -1; }
    void drop() { pid_ = -1; }
    void ready();

    bool exited() const;
    int exit_status() const;
    bool signaled() const;
    int term_sig() const;
    int core_dump() const;
    bool stopped() const;
    int stop_sig() const;
    bool continued() const;

    void send(int sig);
    void touch();
    void wait(int flags = 0);
    void expect(int sig);
    void waitfor(int sig);
    void waitfor_sc(int sig);

    void syscall(int sig = 0);
    void cont(int sig = 0);
    void attach();
    void detach();
    void step(int sig = 0);

    long peek(long addr);
    void poke(long addr, long data);

    void set_break(long addr);
    void remove_break(long addr);

    Location find_symbol(const std::string &name) {
        return Location::Symbol(pid_, name);
    }

    Location find_symbol(const std::string &sym,
                         const std::string &mod) {
        return Location::Symbol(pid_, sym, mod);
    }

    Location find_module(const std::string &name) {
        return Location::Module(pid_, name);
    }
    void dyn_call(long addr, arch::regbuf_type &regs, long sp);
    void timeout(double time);

  private:
    pid_t pid_;
    int status_;

    breakpoint_cache bps_;
};
}
