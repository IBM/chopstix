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

#include <linux/limits.h>
#include <signal.h>
#include <stdarg.h>

#include "buffer.h"
#include "membuffer.h"

#define MAX_BREAKPOINTS 1024
#define MAX_FDS 124

namespace chopstix {

struct BreakpointInformation {
    long address;
    long original_content;
};

struct System {
  public:
    static System &instance() {
        static System inst;
        return inst;
    }

    void start_trace(bool isNewInvocation);
    void stop_trace();
    volatile bool tracing = false;

    int tty_fds[MAX_FDS];
    int tty_count = 0;

    System(const System &) = delete;
    System &operator=(const System &) = delete;

  private:
    System();
    ~System();

    static void sigsegv_handler(int, siginfo_t *, void *);

    // Settings
    char trace_path[PATH_MAX];

    void register_handlers();
    void update_ttys();
    void record_segv(unsigned long addr, unsigned long pc_addr);
    void save_page(unsigned long page_addr);

    int trace_id = 0;
    long pagesize;
    int pagecount = 0;
    int tpagecount = 0;
    int max_pages;
    int max_traces;
    int group_iter;
    int cur_iter;
    bool save;
    bool drytrace;
    TraceBuffer buf_;
    MemBuffer membuf_;
    BreakpointInformation breakpoints[MAX_BREAKPOINTS];
    int breakpoint_count = 0;

    bool mem_trace = false;
    unsigned long previous_addr;
    unsigned long previous_pc_addr;

};
}  // namespace chopstix

extern "C" {
void chopstix_start_trace(unsigned long isNewInvocation);
void chopstix_stop_trace();
}
