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

#include "memory.h"
#include "support/check.h"
#include "support/filesystem.h"
#include "support/log.h"
#include "support/options.h"
#include "support/string.h"

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <ucontext.h>
#include <unistd.h>

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

namespace chopstix {

System &sys_ = System::instance();

System::System() {
    log::Logger::instance();
    log::debug("System:: Start preload library initialization");

    // checkx(getopt("begin"), "No begin address set");
    // checkx(getopt("end"), "No end address set");
    checkx(getopt("trace-dir").is_set(), "No trace path set");
    safe_strncpy(trace_path, getopt("trace-dir"), sizeof(trace_path));
    save = getopt("save").as_bool();
    drytrace = getopt("access-only").as_bool();
    max_pages = getopt("max-pages").as_int();
    group_iter = getopt("group").as_int(1);
    // cur_iter = 0;

    filesystem::mkdir(trace_path);

    pagesize = sysconf(_SC_PAGESIZE);

    register_handlers();

    Memory::instance();
    buf_.setup(trace_path);

    sigaltstack(Memory::instance().alt_stack(), NULL);

    unsetenv("LD_PRELOAD");

    char fname[PATH_MAX];

    sfmt::format(fname, sizeof(fname), "%s/_alt_stack", trace_path);
    int stack_fd =
        syscall(SYS_openat, AT_FDCWD,fname, O_WRONLY | O_CREAT | O_TRUNC, PERM_664);
    check(stack_fd != -1, "Unable to write file");
    int n = ::write(stack_fd, Memory::instance().alt_stack(),
                    sizeof(Memory::stack_type));
    syscall(SYS_close, stack_fd);

    log::debug("System:: Raising SIGUSR1 signal to notify parent");
    raise(SIGUSR1);

    sfmt::format(fname, sizeof(fname), "%s/_restrict_map", trace_path);
    int res_fd = syscall(SYS_openat, AT_FDCWD,fname, O_RDONLY);
    Memory::instance().restrict_map(res_fd);
    syscall(SYS_close, res_fd);

    log::debug("System:: End preload library initialization");
}

System::~System() {}

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
    log::debug("System::record_segv: segv at %x", addr);
    unsigned long page_addr = Memory::instance().page_addr(addr);
    auto reg = Memory::instance().find_region(page_addr);
    checkx(reg, "Unable to find memory region for %x", addr);
    log::verbose(
        "System::record_segv: access at addr:%x page:%x region:%s perm:%s",
        addr, page_addr, reg->path, reg->perm);
    Memory::instance().unprotect_page(reg, page_addr);

    if (!tracing) {
        log::debug("System::record_segv: not tracing");
    }

    ++pagecount;

    if (max_pages > 0 && pagecount > max_pages) {
        log::debug("System::record_segv: max. pages reached");
    } else {
        if (save) {
            save_page(page_addr);

            char fname[PATH_MAX];
            sfmt::format(fname, sizeof(fname), "%s/_page_count.%d", trace_path,
                         trace_id);
            int fd = syscall(SYS_openat, AT_FDCWD,fname, O_WRONLY | O_CREAT | O_TRUNC,
                             PERM_664);
            check(fd != -1, "Unable to open %s", fname);
            int val = write(fd, (void *)&pagecount, sizeof(int));
            check(val == sizeof(int), "Unable to write bytes");
            syscall(SYS_close, fd);
            log::debug("System::record_segv: writing %d to %s", pagecount,
                       fname);
        }
    }

    if (drytrace) {
        buf_.save_page(page_addr);
    }
}

void System::save_page(unsigned long page_addr) {
    char fname[PATH_MAX];
    log::debug("System::save_page: saving %x", page_addr);
    sfmt::format(fname, sizeof(fname), "%s/page.%d.%x", trace_path, trace_id,
                 page_addr);

    log::debug("System::save_page: opening %s", fname);
    int fd = syscall(SYS_openat, AT_FDCWD, fname, O_WRONLY | O_CREAT | O_TRUNC,
                     PERM_664);
    check(fd != -1, "Unable to save page at '%s'", fname);

    log::debug("System::save_page: writing to %s", fname);
    ssize_t w = ::write(fd, (void *)page_addr, pagesize);
    assert(w == pagesize && "Unable to write");

    unsigned long offset_mask = pagesize - 1;
    unsigned long page_mask = ~offset_mask;
    for (int i = 0; i < breakpoint_count; i++) {
        BreakpointInformation &breakpoint = breakpoints[i];

        unsigned long page = page_addr & page_mask;
        unsigned long breakpoint_page =
            (unsigned long) breakpoint.address & page_mask;
        if (breakpoint_page == page) {
            log::debug("System::save_page: fixing breakpoint at 0x%x",
                       breakpoint.address);
            lseek(fd, breakpoint.address & offset_mask, SEEK_SET);
            w = ::write(fd, &breakpoint.original_content, sizeof(long));
            assert(w == sizeof(long) &&
                   "Unable to restore breakpoint contents");
        }
    }

    syscall(SYS_close, fd);
    log::debug("System::save_page: finished saving %x", page_addr);
}

void System::start_trace(bool isNewInvocation) {
    log::verbose("System: start_trace: start trace %d start", trace_id);
    check(tracing == false, "Tracing already started");

    if (drytrace) {
        buf_.start_trace(trace_id, isNewInvocation);
    }

    log::debug("System: start_Trace: reading breakpoint information");
    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "%s/_breakpoints", trace_path);
    FILE *fp = fopen(fname, "rb");
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    unsigned int elements = size / sizeof(BreakpointInformation);
    check(elements <= MAX_BREAKPOINTS, "Too many breakpoints enabled");
    rewind(fp);
    breakpoint_count = elements;
    size_t w = fread(breakpoints, sizeof(BreakpointInformation), elements, fp);
    assert(w == (sizeof(BreakpointInformation) * elements) &&
           "Unable to read breakpoint information");
    fclose(fp);
    log::debug("System: start_Trace: stored info of %d breakpoints", elements);

    Memory::instance().update();
    log::debug("System: start_Trace: map updated");
    Memory::instance().protect_all();

    tracing = true;
    pagecount = 0;

    // log::debug("Tracing: %d", tracing);
    // log::verbose("Systen: start_trace: start trace %d end", trace_id);
}

void System::stop_trace() {
    log::verbose("System:: stop_trace: Start");
    log::debug("Tracing: %d", tracing);
    check(tracing == true, "Tracing already stopped");
    tracing = false;
    tpagecount += pagecount;

    Memory::instance().unprotect_all();
    log::verbose("System:: stop_trace: Trace id: %d", trace_id);

    if (drytrace) {
        buf_.stop_trace(trace_id);
    }

    // if (max_pages > 0) {
    //    char fname[PATH_MAX];
    //    sfmt::format(fname, sizeof(fname), "%s/_page_count.%d", trace_path,
    //    trace_id); int fd = ::creat(fname, PERM_664); check(fd != -1, "Unable
    //    to open %s", fname);
    //    ::write(fd, (void *)&pagecount, sizeof(int));
    //    ::close(fd);
    //    log::debug("System::stop_trace: writing %d to %s", pagecount, fname);
    //}

    if (save) {
        unsigned long *page = Memory::restricted_pages();
        while (*page != 0) {
            log::verbose("System:: stop_trace: saving unprotected reserved "
                         "symbol pages:%x", *page);
            save_page(*page);
            ++page;
        }

        mem_region *reg = Memory::instance().restricted_regions();
        while (reg->addr[0] != 0) {
            log::verbose("System:: stop_trace: saving pages of unprotected "
                         "regions: %x-%x %s (%s)", reg->addr[0], reg->addr[1],
                         reg->perm, reg->path);

            for (unsigned long page = reg->addr[0]; page < reg->addr[1];
                 page += pagesize) {
                log::verbose("System:: stop_trace: saving pages: 0x%x", page);
                save_page(page);
            }
            // save_page(*page);
            //++page;
            ++reg;
        }
    }

    ++trace_id;
    log::verbose("System:: stop_trace: End");
}
}  // namespace chopstix

void chopstix_start_trace(unsigned long isNewInvocation) {
    chopstix::sys_.start_trace(isNewInvocation);

    // Generate a SIGILL event, without using system routines like 'raise'
    // to avoid more page faults that needed (note that all pages have been
    // protected.
    __asm__(".long 0x00000000");
    __asm__(".long 0x00000000");
    // raise(SIGTRAP);
}
void chopstix_stop_trace() {
    chopstix::sys_.stop_trace();
    __asm__(".long 0x00000000");
    __asm__(".long 0x00000000");
    // raise(SIGTRAP);
}
