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
#include "system.h"

#include "common/check.h"
#include "common/filesystem.h"
#include "common/log.h"
#include "common/param.h"
#include "common/stringutils.h"
#include "memory.h"

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ucontext.h>
#include <unistd.h>

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

namespace cxtrace {

System &sys_ = System::instance();

System::System() {
    // checkx(getparam("begin"), "No begin address set");
    // checkx(getparam("end"), "No end address set");
    checkx(getparam("trace"), "No trace path set");

    safe_strncpy(trace_path, getparam("trace", "trace"), sizeof(trace_path));
    save = atoi(getparam("save", "0")) != 0;
    drytrace = atoi(getparam("drytrace", "0")) != 0;
    max_pages = atoi(getparam("max-pages", "0"));
    group_iter = atoi(getparam("group-iter", "1"));
    cur_iter = 0;

    filesystem::mkdir(trace_path);

    pagesize = sysconf(_SC_PAGESIZE);

    register_handlers();

    log::Logger::instance();
    Memory::instance();
    buf_.setup(trace_path);

    sigaltstack(Memory::instance().alt_stack(), NULL);

    unsetenv("LD_PRELOAD");

    char fname[PATH_MAX];

    fmt::format(fname, sizeof(fname), "%s/_alt_stack", trace_path);
    int stack_fd = ::creat(fname, PERM_664);
    check(stack_fd != -1, "Unable to write file");
    int n = ::write(stack_fd, Memory::instance().alt_stack(),
            sizeof(Memory::stack_type));
    close(stack_fd);

    raise(SIGUSR1);

    fmt::format(fname, sizeof(fname), "%s/_restrict_map", trace_path);
    int res_fd = ::open(fname, O_RDONLY);
    Memory::instance().restrict_map(res_fd);
    ::close(res_fd);
}

System::~System() { }

void System::sigsegv_handler(int sig, siginfo_t *si, void *ptr) {
    ucontext_t *ctx = (ucontext_t *)ptr;
#if defined(ARCH_POWER)
#define POWER_R1 1
#define POWER_NIP 32
    log::debug("R1  = %x", ctx->uc_mcontext.gp_regs[POWER_R1]);
    log::debug("NIP = %x", ctx->uc_mcontext.gp_regs[POWER_NIP]);
#elif defined(ARCH_SYSZ)
    log::debug("PSWM = %x", ctx->uc_mcontext.psw.mask);
    log::debug("PSWA = %x", ctx->uc_mcontext.psw.addr);
#endif
    sys_.record_segv((unsigned long)si->si_addr);
}

void System::register_handlers() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &System::sigsegv_handler;
    sigaction(SIGSEGV, &sa, NULL);
}

void System::record_segv(unsigned long addr) {
    log::debug("segv at %x", addr);
    unsigned long page_addr = Memory::instance().page_addr(addr);
    auto reg = Memory::instance().find_region(page_addr);
    checkx(reg, "Unable to find memory region for %x", addr);
    log::verbose("access at addr:%x page:%x region:%s perm:%s", addr, page_addr, reg->path, reg->perm);
    Memory::instance().unprotect_page(reg, page_addr);
    ++pagecount;
    if (save) {
        save_page(page_addr);
    }
    if (drytrace) {
        buf_.save_page(page_addr);
    }
}

void System::save_page(unsigned long page_addr) {
    char fname[PATH_MAX];
    log::debug("fname at %p", &fname);
    log::debug("saving %x", page_addr);
    fmt::format(fname, sizeof(fname), "%s/page.%d.%x",
            trace_path, trace_id, page_addr);
    log::debug("opening %s", fname);
    int fd = ::creat(fname, PERM_664);
    check(fd != -1, "Unable to save page at '%s'", fname);
    log::debug("writing to %s", fname);
    ssize_t w =::write(fd, (void *)page_addr, pagesize);
    assert(w == pagesize && "Unable to write");
    ::close(fd);
    log::debug("finished saving %x", page_addr);
}

void System::start_trace() {
    log::verbose("start trace %d", trace_id);
    if (cur_iter == 0) {
        tracing = true;
        if (drytrace) {
            buf_.start_trace(trace_id);
        }
        Memory::instance().update();
        Memory::instance().protect_all();
    }
}

void System::stop_trace() {
    if (++cur_iter < group_iter) {
        ++trace_id;
        return;
    }
    cur_iter = 0;
    Memory::instance().unprotect_all();
    tracing = false;
    log::verbose("stop trace %d", trace_id);
    if (drytrace) {
        buf_.stop_trace(trace_id);
    }
    if (max_pages > 0) {
        char fname[PATH_MAX];
        fmt::format(fname, sizeof(fname), "%s/_page_count", trace_path);
        int fd = ::creat(fname, PERM_664);
        check(fd != -1, "Unable to open %s", fname);
        ::write(fd, (void *) &pagecount, sizeof(int));
        ::close(fd);
    }
    // if (trace_id == 0) {
    unsigned long *page = Memory::restricted_pages();
    while (*page != 0) {
        log::verbose("saving libc page:%x", *page);
        save_page(*page);
        ++page;
    }
    // }
    ++trace_id;
}
}

void cxtrace_start_trace() {
    cxtrace::sys_.start_trace();
    raise(SIGTRAP);
}
void cxtrace_stop_trace() {
    cxtrace::sys_.stop_trace();
    raise(SIGTRAP);
}
