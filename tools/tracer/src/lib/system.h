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

#include <signal.h>
#include <stdarg.h>
#include <linux/limits.h>

#include "buffer.h"

namespace cxtrace {

struct System {
  public:
    static System &instance() {
        static System inst;
        return inst;
    }

    void start_trace();
    void stop_trace();

    System(const System &) = delete;
    System &operator=(const System &) = delete;

  private:
    System();
    ~System();

    static void sigsegv_handler(int, siginfo_t *, void *);

    // Settings
    char trace_path[PATH_MAX];

    void register_handlers();
    void record_segv(unsigned long addr);
    void save_page(unsigned long page_addr);

    bool tracing = false;
    int trace_id = 0;
    long pagesize;
    volatile int pagecount = 0;
    int max_pages;
    int group_iter;
    int cur_iter;
    bool save;
    bool drytrace;
    TraceBuffer buf_;

};
}

extern "C" {
void cxtrace_start_trace();
void cxtrace_stop_trace();
}
