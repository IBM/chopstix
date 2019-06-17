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
#include "arch.h"
#include "client.h"
#include "common/check.h"
#include "common/maps.h"
#include "common/filesystem.h"
#include "common/format.h"
#include "common/log.h"
#include "common/param.h"
#include "process.h"

#include <cstring>
#include <fstream>

#include <vector>
#include <algorithm>

#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

using namespace cxtrace;

namespace fs = filesystem;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

static void run_child(char **argv) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(*argv, argv);
}

static void run_parent(pid_t child) {
    int status;
    waitpid(child, &status, 0);
    check(WIFSTOPPED(status), "Expected stop signal");
    ptrace(PTRACE_CONT, child, 0, 0);
    waitpid(child, &status, 0);
    check(WIFEXITED(status), "Expected exit");
}

static long read_alt_stack() {
    char fname[PATH_MAX];
    fmt::format(fname, sizeof(fname), "%s/_alt_stack", getparam("trace"));
    int stack_fd = ::open(fname, O_RDONLY);
    stack_t alt_stack;
    int n = read(stack_fd, &alt_stack, sizeof(stack_t));
    log::debug("alt stack at %x-%x", (unsigned long)alt_stack.ss_sp,
            (unsigned long)alt_stack.ss_sp + alt_stack.ss_size);
    return (long) alt_stack.ss_sp + alt_stack.ss_size / 2;
}

#define PERM_R PROT_READ
#define PERM_W PROT_WRITE
#define PERM_X PROT_EXEC
#define PERM_RW (PROT_READ | PROT_WRITE)
#define PERM_RX (PROT_READ | PROT_EXEC)
#define PERM_WX (PROT_WRITE | PROT_EXEC)
#define PERM_RWX (PROT_READ | PROT_WRITE | PROT_EXEC)

const char *encode_perm(int prot) {
    switch (prot) {
    case PERM_R: return "r--";
    case PERM_W: return "-w-";
    case PERM_X: return "--x";
    case PERM_RW: return "rw-";
    case PERM_RX: return "r-x";
    case PERM_WX: return "-wx";
    case PERM_RWX: return "rwx";
    default: return "---";
    }
}

struct mmap_call {
    unsigned long addr;
    unsigned long length;
    unsigned long prot;
};

static unsigned long upper_div(unsigned long a, unsigned long b) {
    return (a + b - 1) / b;
}

static void track_mmap(Process &child, arch::regbuf_type &regs) {
    std::vector<mmap_call> restrict_map;
    long pagesize = sysconf(_SC_PAGESIZE);
    long args[7];
    while (1) {
        int sig;

        child.syscall();
        child.wait();
        checkx(child.stopped(), "Child did not stop");
        sig = child.stop_sig();
        if (sig == SIGUSR1) {
            log::debug("System finished setup");
            break;
        }
        checkx(sig == SIGTRAP, "Expected trap/breakpoint, found %s",
                strsignal(sig));
        arch::read_regs(child.pid(), regs);

        long syscall_nr = arch::get_syscall(regs);
        arch::get_args(regs, args);

        //      0   , 1     , 2   , 3    , 4  , 5
        // mmap(addr, length, prot, flags, fd, offset)
        //        0   , 1
        // munmap(addr, length)

        child.syscall();
        child.wait();
        checkx(child.stopped(), "Child did not stop");
        sig = child.stop_sig();
        checkx(sig == SIGTRAP, "Expected trap/breakpoint, found %s",
                strsignal(sig));
        arch::read_regs(child.pid(), regs);

        long ret = arch::get_ret(regs);

        if (syscall_nr == SYS_mmap) {
            mmap_call mmap;
            mmap.addr = ret;
            mmap.length = args[1];
            mmap.prot = args[2];
            log::debug("mmap %x-%x %s", mmap.addr, mmap.addr + mmap.length,
                    encode_perm(mmap.prot));
            restrict_map.push_back(mmap);
        } else if (syscall_nr == SYS_munmap) {
            // TODO This is a hack!
            mmap_call munmap;
            munmap.addr = args[0];
            munmap.length = args[1];
            log::verbose("munmap %x-%x", munmap.addr, munmap.addr + munmap.length);
            auto end = std::remove_if(restrict_map.begin(), restrict_map.end(),
                    [&](const mmap_call &mmap) {
                        return mmap.addr   == munmap.addr
                            && mmap.length == munmap.length; });
            restrict_map.erase(end);
        }
    }
    char fname[PATH_MAX];
    fmt::format(fname, sizeof(fname), "%s/_restrict_map", getparam("trace"));
    FILE *fp = fopen(fname, "w");
    check(fp, "Unable to open restrict_map");
    for (auto &reg : restrict_map) {
        reg.length = upper_div(reg.length, pagesize) * pagesize;
        fprintf(fp, "%lx-%lx %s 0 0:0 0 [restrict]\n", reg.addr, reg.addr + reg.length,
                encode_perm(reg.prot));

    }
    fclose(fp);
}

int main(int argc, char **argv) {
    set_defaults();
    int n = parse_args(argc - 1, argv + 1) + 1;

    checkx(argc > n, "No command. See '%s --help'", argv[0]);
    // checkx(getparam("begin"), "No begin address set");
    // checkx(getparam("end"), "No end address set");
    checkx(getparam("trace"), "No trace path");

    bool save_regs = atoi(getparam("save", "0")) != 0;
    bool drytrace = atoi(getparam("drytrace", "0")) != 0;
    const char *trace_path = getparam("trace", "trace");
    double sample_freq = atof(getparam("freq", "1"));
    if (getparam("seed")) srandom(atoi(getparam("seed")));
    bool nolib = atoi(getparam("nolib", "0")) != 0;
    double tidle = atof(getparam("tidle", "0"));
    double tsample = atof(getparam("tsample", "0"));
    int max_traces = atoi(getparam("max-traces", "0"));
    int max_pages = atoi(getparam("max-pages", "0"));
    int group_iter = atoi(getparam("group-iter", "1"));


    fs::mkdir(getparam("trace"));

    setenv("LD_BIND_NOW", "1", 1);
    preload(library_path());

    Process child;
    child.exec(argv + n);
    child.ready();

    unsetenv("LD_PRELOAD");

    log::verbose("spawned child process %d", child.pid());

    auto addr_begin = getaddrs("begin");
    auto addr_end = getaddrs("end");

    bool with_region = addr_begin.size() > 0;

    auto start_trace = child.find_symbol("cxtrace_start_trace", library_path());
    auto stop_trace = child.find_symbol("cxtrace_stop_trace", library_path());

    Location module_offset = getparam("module")
                                 ? child.find_module(getparam("module"))
                                 : Location::Address(0);

    Location the_stack = child.find_module("[stack]");

    Location vdso = child.find_module("[vdso]");

    auto regs = arch::create_regs();
    auto tmp_regs = arch::create_regs();

    long trace_id = 0;

    char fname[PATH_MAX];

    bool sample_next;

    track_mmap(child, regs);

    // child.cont();
    // child.waitfor(SIGUSR1);
    log::debug("system finished setup");

    long alt_stack = read_alt_stack();
    long cur_pc;

    while (1) {
        if (tidle > 0) child.timeout(tidle);
        sample_next = next_rand() < sample_freq;
        if (!child.active()) break;
        if (with_region) {
            for (auto addr : addr_begin) {
                child.set_break(addr + module_offset.addr());
            }
            child.cont();
            child.waitfor(SIGILL);
            if (!child.active()) break;
        }
        cur_pc = arch::get_pc(child.pid());
        // arch::set_pc(child.pid(), cur_pc);
        // log::verbose("enter region %x", cur_pc);
        if (with_region) {
            for (auto addr : addr_begin) {
                child.remove_break(addr + module_offset.addr());
            }
        } 
        if (sample_next) {
            log::verbose("start trace %d at 0x%x", trace_id, cur_pc);
            if (save_regs || (drytrace && trace_id == 0)) {
                if (trace_id % group_iter == 0) {
                    arch::read_regs(child.pid(), regs);
                    fmt::format(fname, sizeof(fname), "%s/regs.%d", trace_path, trace_id);
                    FILE *fp = fopen(fname, "w");
                    arch::dump_regs(fp, regs);
                    fclose(fp);
                    fmt::format(fname, sizeof(fname), "/proc/%d/maps", child.pid());
                    std::string from{fname};
                    fmt::format(fname, sizeof(fname), "%s/maps.%d", trace_path, trace_id);
                    std::string to{fname};
                    fs::copy(from, to);
                }
            }
            if (!nolib) child.dyn_call(start_trace, regs, alt_stack);
        }

        if (tsample > 0) child.timeout(tsample);
        if (!child.active()) break;
        if (with_region) {
            for (auto addr : addr_end) {
                child.set_break(addr + module_offset.addr());
            }
            child.syscall();
            bool inside_lib = false;
            while (1) {
                child.wait();
                if (!child.stopped()) break;
                int sig = child.stop_sig();
                if (sig == SIGILL) {
                    // hit breakpoint
                    break;
                // } else if (sig == SIGUSR1) {
                //     inside_lib = !inside_lib;
                //     child.syscall();
                } else if (sig == SIGTRAP) {
                    // enter syscall
                    arch::read_regs(child.pid(), tmp_regs);
                    long sc_nr = arch::get_syscall(tmp_regs);
                    long lnk_reg = arch::get_lnk(child.pid());
                    bool in_vdso = lnk_reg >= vdso.addr();
                    bool in_support = start_trace.entry().contains(lnk_reg);
                    cur_pc = arch::get_pc(child.pid());
                    // finish syscall
                    child.syscall();
                    child.wait();
                    if (!in_support && !in_vdso) {
                        log::verbose("system call %d from %x", sc_nr, cur_pc);
                        log::verbose("split trace %d at %x", trace_id, cur_pc);
                        if (!nolib) child.dyn_call(stop_trace, regs, alt_stack);
                        ++trace_id;
                        if (save_regs || (drytrace && trace_id == 0)) {
                            if (trace_id % group_iter == 0) {
                                arch::read_regs(child.pid(), regs);
                                fmt::format(fname, sizeof(fname), "%s/regs.%d", trace_path, trace_id);
                                FILE *fp = fopen(fname, "w");
                                arch::dump_regs(fp, regs);
                                fclose(fp);
                                fmt::format(fname, sizeof(fname), "/proc/%d/maps", child.pid());
                                std::string from{fname};
                                fmt::format(fname, sizeof(fname), "%s/maps.%d", trace_path, trace_id);
                                std::string to{fname};
                                fs::copy(from, to);
                            }
                        }
                        log::verbose("start trace %d at %x", trace_id, cur_pc);
                        if (!nolib) child.dyn_call(start_trace, regs, alt_stack);
                    }
                    // continue
                    child.syscall();
                } else {
                    // forward signal
                    child.syscall(sig);
                }
            }
            // child.cont();
            // child.waitfor(SIGILL);
            if (!child.active()) break;
            for (auto addr : addr_end) {
                child.remove_break(addr + module_offset.addr());
            }
        }
        cur_pc = arch::get_pc(child.pid());
        // arch::set_pc(child.pid(), cur_pc);
        // log::verbose("exit region %x", cur_pc);
        if (sample_next) {
            if (!nolib) child.dyn_call(stop_trace, regs, alt_stack);
            log::verbose("stop trace %d at %x", trace_id, cur_pc);

            ++trace_id;
        }
        if (max_pages > 0) {
            fmt::format(fname, sizeof(fname), "%s/_page_count", trace_path);
            int pagecount;
            int fd = ::open(fname, O_RDONLY);
            int n = ::read(fd, &pagecount, sizeof(int));
            if (pagecount >= max_pages) {
                log::verbose("collected enough pages");
                child.cont(SIGTERM);
                child.wait();
            }
        
        }
        if (max_traces && trace_id == max_traces) {
            log::verbose("collected enough traces");
            child.cont(SIGTERM);
            child.wait();
            break;
        }
    }

    log::info("collected %d traces", trace_id);

    free(regs);
    free(tmp_regs);

    if (child.exited()) {
        log::verbose("child exited with %d", child.exit_status());
    }
}
