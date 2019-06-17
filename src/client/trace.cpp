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
#include "client.h"
#include "usage.h"

#include "core/arch.h"
#include "core/location.h"
#include "core/maps.h"
#include "core/process.h"
#include "support/check.h"
#include "support/filesystem.h"
#include "support/log.h"
#include "support/options.h"
#include "support/safeformat.h"

#include <cstring>
#include <fstream>

#include <algorithm>
#include <vector>

#include <fcntl.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace chopstix;

namespace fs = filesystem;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

namespace {

static double next_rand() { return (random() + 0.0) / RAND_MAX; }

static std::string library_path() {
    char raw_path[PATH_MAX];
    auto n = readlink("/proc/self/exe", raw_path, PATH_MAX);
    std::string full_path{raw_path};
    auto pos = full_path.rfind("bin/chop");
    return full_path.substr(0, pos) + "/lib/libcxtrace.so";
}

static void preload(std::string path) {
    char *env_preload = getenv("LD_PRELOAD");
    if (env_preload != NULL) {
        path += ':' + env_preload;
    }
    setenv("LD_PRELOAD", path.c_str(), 1);
}

static long read_alt_stack() {
    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "%s/_alt_stack",
                 getopt("trace-dir").as_string());
    int stack_fd = ::open(fname, O_RDONLY);
    stack_t alt_stack;
    int n = read(stack_fd, &alt_stack, sizeof(stack_t));
    log::debug("alt stack at %x-%x", (unsigned long)alt_stack.ss_sp,
               (unsigned long)alt_stack.ss_sp + alt_stack.ss_size);
    return (long)alt_stack.ss_sp + alt_stack.ss_size / 2;
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

static void track_mmap(Process &child, Arch::regbuf_type &regs) {
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
            break;
        }
        checkx(sig == SIGTRAP, "Expected trap/breakpoint A, found %s",
               strsignal(sig));
        Arch::current()->read_regs(child.pid(), regs);

        long syscall_nr = Arch::current()->parse_syscall(regs);
        Arch::current()->parse_args(regs, args);

        //      0   , 1     , 2   , 3    , 4  , 5
        // mmap(addr, length, prot, flags, fd, offset)
        //        0   , 1
        // munmap(addr, length)

        child.syscall();
        child.wait();
        checkx(child.stopped(), "Child did not stop");
        sig = child.stop_sig();
        checkx(sig == SIGTRAP, "Expected trap/breakpoint B, found %s",
               strsignal(sig));
        Arch::current()->read_regs(child.pid(), regs);

        long ret = Arch::current()->parse_ret(regs);

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
            log::debug("munmap %x-%x", munmap.addr,
                       munmap.addr + munmap.length);
            auto end = std::remove_if(restrict_map.begin(), restrict_map.end(),
                                      [&](const mmap_call &mmap) {
                                          return mmap.addr == munmap.addr &&
                                                 mmap.length == munmap.length;
                                      });
            restrict_map.erase(end);
        }
    }
    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "%s/_restrict_map",
                 getopt("trace-dir").as_string());
    FILE *fp = fopen(fname, "w");
    check(fp, "Unable to open restrict_map");
    for (auto &reg : restrict_map) {
        reg.length = upper_div(reg.length, pagesize) * pagesize;
        fprintf(fp, "%lx-%lx %s 0 0:0 0 [restrict]\n", reg.addr,
                reg.addr + reg.length, encode_perm(reg.prot));
    }
    fclose(fp);
}
}  // namespace

int run_trace(int argc, char **argv) {
    PARSE_OPTIONS(trace, argc, argv);

    log::verbose("run_trace:: Starting client setup");

    CHECK_USAGE(trace, argc > 0, "No command given.");

    CHECK_USAGE(trace,
                (getopt("begin").is_set() && getopt("end").is_set()) ||
                    (getopt("interval").is_set() && getopt("active").is_set()),
                "No tracing parameters (Region of Interest or Temporal-based "
                "sampling)");

    bool save_regs = getopt("save").as_bool();
    bool drytrace = getopt("access-only").as_bool();
    std::string trace_path = getopt("trace-dir").as_string();
    double sample_freq = getopt("prob").as_float();
    bool nolib = !getopt("trace").as_bool();
    double tidle = getopt("interval").as_time();
    double tsample = getopt("active").as_time();
    int max_traces = getopt("max-traces").as_int();
    int max_pages = getopt("max-pages").as_int();
    int group_iter = getopt("group").as_int();
    auto addr_begin = getopt("begin").as_hex_vec();
    auto addr_end = getopt("end").as_hex_vec();
    bool with_region = addr_begin.size() > 0;

    checkx(!fs::exists(trace_path), "Output trace directory path '%s' already exists!", trace_path);
    fs::mkdir(trace_path);

    log::verbose("run_trace:: Starting child process");
    Process child;
    setenv("LD_BIND_NOW", "1", 1);
    preload(library_path());
    child.exec(argv, argc);
    child.ready();
    unsetenv("LD_PRELOAD");
    log::verbose("run_trace:: Spawned child process %d", child.pid());
    sleep(2);

    log::verbose("run_trace:: Looking for preloaded library symbols");
    auto start_trace =
        child.find_symbol("chopstix_start_trace", library_path());
    auto stop_trace = child.find_symbol("chopstix_stop_trace", library_path());

    Location module_offset =
        getopt("module") ? child.find_module(getopt("module").as_string())
                         : Location::Address(0);

    Location vdso = child.find_module("[vdso]");

    auto regs = Arch::current()->create_regs();
    auto tmp_regs = Arch::current()->create_regs();

    long trace_id = 0;

    char fname[PATH_MAX];

    bool sample_next;

    track_mmap(child, regs);

    long alt_stack = read_alt_stack();
    long cur_pc;

    log::verbose("run_trace:: Client finished setup");

    while (1) {
        if (tidle > 0) {
            log::verbose("run_trace:: not tracing for %s",
                         getopt("interval").as_string());
            child.timeout(tidle);
        }

        if (!child.active()) {
            log::verbose("run_trace:: child not active. Stopping.");
            break;
        }

        if (with_region) {
            log::verbose("run_trace:: setting start break point of region");
            for (auto addr : addr_begin) {
                child.set_break(addr + module_offset.addr());
            }
            log::debug("run_trace:: continuing to region start");
            child.cont();
            child.waitfor(SIGILL);
            if (!child.active()) {
                log::debug("run_trace:: child finished before region start");
                if (trace_id == 0) {
                    log::info(
                        "run_trace:: child finished without tracing. Cheeck "
                        "region addresses/command executed.");
                }
                break;
            }
        }

        cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("run_trace:: Stop at PC: %x", cur_pc);

        if (with_region) {
            log::debug("run_trace:: removing start break point of region");
            for (auto addr : addr_begin) {
                child.remove_break(addr + module_offset.addr());
            }
        }

        sample_next = next_rand() < sample_freq;
        if (sample_next) {
            log::verbose("run_trace:: start trace %d at 0x%x", trace_id,
                         cur_pc);
            if (save_regs || (drytrace && trace_id == 0)) {
                if (trace_id % group_iter == 0) {
                    log::debug(
                        "run_trace:: dumping registers, maps and info files "
                        "for trace %d",
                        trace_id);
                    Arch::current()->read_regs(child.pid(), regs);
                    sfmt::format(fname, sizeof(fname), "%s/regs.%d", trace_path,
                                 trace_id);
                    FILE *fp = fopen(fname, "w");
                    Arch::current()->serialize_regs(fp, regs);
                    fclose(fp);
                    sfmt::format(fname, sizeof(fname), "/proc/%d/maps",
                                 child.pid());
                    std::string from{fname};
                    sfmt::format(fname, sizeof(fname), "%s/maps.%d", trace_path,
                                 trace_id);
                    std::string to{fname};
                    fs::copy(from, to);
                    sfmt::format(fname, sizeof(fname), "%s/info.%d", trace_path,
                                 trace_id);
                    fp = fopen(fname, "w");
                    fprintf(fp, "begin_addr %016lx\n", cur_pc);
                    fclose(fp);
                }
            }

            if (!nolib) {
                log::verbose("run_trace:: call start tracing routine on child");
                child.dyn_call(start_trace, regs, alt_stack);
            }
        } else {
            log::verbose("run_trace:: no trace at 0x%x", cur_pc);
        }

        if (tsample > 0) {
            log::verbose("run_trace:: tracing for %s",
                         getopt("active").as_string());
            child.timeout(tsample);
        }

        if (!child.active()) {
            log::verbose("run_trace:: child not active. Stopping.");
            break;
        }

        if (with_region) {
            log::verbose("run_trace:: setting end break point of region");
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
                    break;
                } else if (sig == SIGTRAP) {
                    // enter syscall
                    Arch::current()->read_regs(child.pid(), tmp_regs);
                    long sc_nr = Arch::current()->parse_syscall(tmp_regs);
                    long lnk_reg = Arch::current()->get_lnk(child.pid());
                    bool in_vdso = lnk_reg >= vdso.addr();
                    bool in_support = start_trace.entry().contains(lnk_reg);
                    cur_pc = Arch::current()->get_pc(child.pid());
                    // finish syscall
                    child.syscall();
                    child.wait();
                    // continue
                    if (child.exited()) {
                        log::verbose("run_trace:: child exited with %d",
                                     child.exit_status());
                        goto end_tracing;
                    }

                    if (!in_support && !in_vdso) {
                        log::verbose("run_trace:: system call %d from %x",
                                     sc_nr, cur_pc);
                        log::verbose("run_trace:: split trace %d at %x",
                                     trace_id, cur_pc);
                        if (!nolib) {
                            log::debug("run_trace:: call stop trace");
                            child.dyn_call(stop_trace, regs, alt_stack);
                        }
                        ++trace_id;
                        if (save_regs || (drytrace && trace_id == 0)) {
                            if (trace_id % group_iter == 0) {
                                log::verbose(
                                    "run_trace:: Setting up new trace");
                                Arch::current()->read_regs(child.pid(), regs);
                                sfmt::format(fname, sizeof(fname), "%s/regs.%d",
                                             trace_path, trace_id);
                                FILE *fp = fopen(fname, "w");
                                Arch::current()->serialize_regs(fp, regs);
                                fclose(fp);
                                sfmt::format(fname, sizeof(fname),
                                             "/proc/%d/maps", child.pid());
                                std::string from{fname};
                                sfmt::format(fname, sizeof(fname), "%s/maps.%d",
                                             trace_path, trace_id);
                                std::string to{fname};
                                fs::copy(from, to);
                                sfmt::format(fname, sizeof(fname), "%s/info.%d",
                                             trace_path, trace_id);
                                fp = fopen(fname, "w");
                                fprintf(fp, "begin_addr %016lx\n", cur_pc);
                                fclose(fp);
                            }
                        }
                        log::verbose("run_trace:: start trace %d at %x",
                                     trace_id, cur_pc);
                        if (!nolib)
                            child.dyn_call(start_trace, regs, alt_stack);
                    } else {
                        log::debug("run_trace:: in support / in_vdso");
                    }
                    // continue
                    child.syscall();
                } else if (sig == SIGSEGV) {
                    // forward signal
                    log::debug("run_trace:: catching signal SIGSEGV");
                    if (max_pages > 0) {
                        sfmt::format(fname, sizeof(fname), "%s/_page_count.%d",
                                     trace_path, trace_id);
                        log::debug("run_trace: trying to read from %s", fname);
                        int fd = open(fname, O_RDONLY);
                        if (fd != -1) {
                            log::debug("run_trace: fd: %d", fd);
                            int pagecount;
                            int n = read(fd, &pagecount, sizeof(int));
                            close(fd);
                            if (fd == sizeof(int)) {
                                log::debug("run_trace: read %d from %s",
                                           pagecount, fname);
                                if (pagecount >= max_pages) {
                                    log::info(
                                        "run_trace:: collected enough pages: "
                                        "%d",
                                        pagecount);
                                    // child.cont(SIGTERM);
                                    // child.wait();
                                    goto finish_region;
                                    // continue;
                                }
                            }
                        }
                    }
                    log::debug("run_trace:: forward signal SIGSEGV");
                    child.syscall(sig);
                } else {
                    // forward signal
                    log::debug("run_trace:: forward signal: %d", sig);
                    child.syscall(sig);
                }
            }

        finish_region:
            // child.cont();
            // child.waitfor(SIGILL);
            if (!child.active()) break;
            for (auto addr : addr_end) {
                child.remove_break(addr + module_offset.addr());
            }
        }
        cur_pc = Arch::current()->get_pc(child.pid());
        // Arch::current()->set_pc(child.pid(), cur_pc);
        // log::verbose("run_trace:: exit region %x", cur_pc);
        if (sample_next) {
            if (!nolib) child.dyn_call(stop_trace, regs, alt_stack);
            log::verbose("run_trace:: stop trace %d at %x", trace_id, cur_pc);
            ++trace_id;
        }

        // if (max_pages > 0) {
        //    sfmt::format(fname, sizeof(fname), "%s/_page_count.%d",
        //    trace_path, trace_id-1); int pagecount; int fd = ::open(fname,
        //    O_RDONLY); int n = ::read(fd, &pagecount, sizeof(int));
        //    log::debug("run_trace: read %d from %s", pagecount, fname);
        //    if (pagecount >= max_pages) {
        //        log::info("run_trace:: collected enough pages: %d", n);
        //        child.cont(SIGTERM);
        //        child.wait();
        //    }
        //}

        if (max_traces && trace_id == max_traces) {
            log::info("run_trace:: collected enough traces");
            child.cont(SIGTERM);
            child.wait();
            break;
        }
    }

end_tracing:

    log::info("run_trace:: collected %d traces", trace_id);

    free(regs);
    free(tmp_regs);

    if (child.exited()) {
        log::verbose("run_trace:: child exited with %d", child.exit_status());
    }

    if (getopt("gzip").as_bool()) {
        log::info("run_trace:: compressing trace directory");
        char cmd[PATH_MAX];
        snprintf(cmd, sizeof(cmd), "gzip -rf9 %s", trace_path.c_str());
        int ret = system(cmd);
        check(ret == 0, "'%s' command failed", cmd);
    }

    return 0;
}
