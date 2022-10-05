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
 * NAME        : core/process.cpp
 * DESCRIPTION :
 ******************************************************************************/

#include "process.h"
#include "config.h"

#include "core/arch.h"
#include "support/check.h"
#include "support/log.h"

#include <sys/ptrace.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace chopstix;

Process::~Process() {
    log::debug("Process:: destructor start");
    if (!active()) {
        log::debug("Process:: not active, nothing to do");
    } else {
        log::debug("Process:: active, wait for it");
        wait(0);
    }
    // checkx(exited() || signaled(), "Process did not exit");
    //
    log::debug("Process:: destructor end");
}

Process::Process(Process &&other) : pid_(other.pid_), status_(other.status_) {
    log::debug("Process:: constructor start");
    other.pid_ = -1;
    other.status_ = 0;
    log::debug("Process:: constructor end");
}

Process &Process::operator=(Process &&other) {
    if (this != &other) {
        pid_ = other.pid_;
        status_ = other.status_;
        other.pid_ = -1;
        other.status_ = 0;
    }
    return *this;
}

void Process::exec(char **argv, int argc) {
    log::debug("Process:: exec start");
    pid_ = fork();
    check(pid_ != -1, "Process:: exec: Unable to spawn process");
    if (pid_ != 0) {
        log::debug("Process:: exec end");
        return;
    }

    log::debug("Process:: exec child");
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    int persona = personality(0xffffffff);
    if (persona == -1)
    {
        fprintf(stderr, "Process:: exec: Unable to get ASLR info: %s\n", strerror(errno));
        abandon();
        exit(EXIT_FAILURE);
    }

    persona = persona | ADDR_NO_RANDOMIZE;
    log::debug("Process:: exec: Disabling ASLR ...");
    persona = personality(persona | ADDR_NO_RANDOMIZE);
    if (persona == -1) {
        fprintf(stderr, "Process:: exec: Unable to set ASLR info: %s\n", strerror(errno));
        abandon();
        exit(EXIT_FAILURE);
    }
    if (!(personality (0xffffffff) & ADDR_NO_RANDOMIZE))
    {
        fprintf(stderr, "Process:: exec: Unable to disable ASLR");
        abandon();
        exit(EXIT_FAILURE);
    }
    log::debug("Process:: exec: ASLR disabled");

    int ret = execvp(*argv, argv);
    if (ret <  0) {
         fprintf(stderr, "Process:: exec: Error: %s\n", strerror(errno));
         abandon();
         exit(EXIT_FAILURE);
    }

}

void Process::exec_wait(char **argv, int argc) {
    log::debug("Process:: exec_wait start");
	char mainpath[1024];
    ssize_t nbytes = readlink(argv[0], mainpath, 1024);
    if ((nbytes == -1) || (nbytes == 1024)) {
         fprintf(stderr, "Process:: exec_wait: Error: Main module name truncated\n");
         abandon();
         exit(EXIT_FAILURE);
    }
    mainpath[nbytes] = '\0';
    mainmodule_ = basename(mainpath);

    pid_ = fork();
    check(pid_ != -1, "Process:: exec_wait: Unable to spawn process");
    if (pid_ != 0) {
        log::debug("Process:: exec_wait: wait for child");
        waitpid(pid_, &status_, WNOHANG);
        log::debug("Process:: exec_wait: wait for child done");
        return;
    }
    log::debug("Process:: exec_wait child created %d", pid_);
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    int persona = personality(0xffffffff);
    if (persona == -1)
    {
        fprintf(stderr, "Process:: exec_wait: Unable to get ASLR info: %s\n", strerror(errno));
        abandon();
        exit(EXIT_FAILURE);
    }
    persona = persona | ADDR_NO_RANDOMIZE;
    if (persona & ADDR_NO_RANDOMIZE)
    {
        log::debug("Process:: exec_wait: ASLR already disabled");
    } else {
          persona = personality(persona | ADDR_NO_RANDOMIZE);
        if (persona == -1) {
            fprintf(stderr, "Process:: exec_wait: Unable to set ASLR info: %s\n", strerror(errno));
            abandon();
            exit(EXIT_FAILURE);
        }
        if (!(personality (0xffffffff) & ADDR_NO_RANDOMIZE))
        {
            fprintf(stderr, "Process:: exec_wait: Unable to disable ASLR");
            abandon();
            exit(EXIT_FAILURE);
        }
    }

    int ret = execvp(*argv, argv);
    if (ret <  0) {
         fprintf(stderr, "Process:: exec_wait: Error: %s\n", strerror(errno));
         abandon();
         exit(EXIT_FAILURE);
    }

}

void Process::ready() {
    log::debug("Process:: ready: start");
    wait(0);
    checkx(stopped(), "Process:: ready: The provided command failed");
    checkx(stop_sig() == SIGTRAP, "Process:: ready: The provided command failed");
    log::debug("Process:: ready: end");
}

void Process::abandon() {
    log::debug("Process:: abandon");
    pid_ = -1;
}
void Process::copy(long pid) {
    pid_ = pid;
}

void Process::wait(int flags) {
    log::debug("Process:: wait start");
    if (!active()) {
        log::debug("Process:: wait: not active");
        return;
    }
    pid_t pid = waitpid(pid_, &status_, flags);
    log::debug("Process::wait: waitpid return: %d", pid);
    if (exited()) {
        log::debug("Process:: wait: %d exited", pid);
        abandon();
    }

#if defined(CHOPSTIX_SYSZ_SUPPORT)
    if (stopped()) {
        // Z architecture advances 2 bytes the PC on SIGILL
        if (stop_sig() == SIGILL) {
                long cur_pc = Arch::current()->get_pc(pid_);
                cur_pc = cur_pc - 2;
                Arch::current()->set_pc(pid_, cur_pc);
        }
    }
#endif

    if (signaled()) {
        log::debug("Process:: wait: %d signaled by signal %d", pid, term_sig());
        if (term_sig() == SIGTERM) {
            log::debug("Process:: wait: %d signaled by signal SIGTERM", pid);
            abandon();
            return;
        }
    } else {
        checkx(pid != -1, "Process:: wait: Process aborted");
    }
}

void Process::waitfor(int which) {
    log::debug("Process:: waitfor start");
    checkx(pid_ != -1, "Process:: waitfor: No child process");
    log::debug("Process:: waitfor: waiting for signal %s", strsignal(which));
    while (active()) {
        wait(0);
        if (!stopped()) {
            log::debug("Process:: waitfor: not stopped");
            return;
        }
        int sig = stop_sig();
        log::debug("Process:: waitfor: signal %s received", strsignal(sig));
        if (sig == which) {
            log::debug("Process:: waitfor: %s signal! stop wait", strsignal(sig));
            return;
        }
        cont(sig);
    }
    log::debug("Process:: waitfor: %s never received while process active",
               strsignal(which));
}

void Process::touch() {
    log::debug("Process:: touch start");
    if (!active()) return;
    waitpid(pid_, &status_, WNOHANG);
    if (exited()) abandon();
    log::debug("Process:: touch end");
}

bool Process::exited() { return WIFEXITED(status_); }
int Process::exit_status() { return WEXITSTATUS(status_); }
bool Process::signaled() { return WIFSIGNALED(status_); }
int Process::term_sig() { return WTERMSIG(status_); }
bool Process::stopped() { return WIFSTOPPED(status_); }
int Process::stop_sig() { return WSTOPSIG(status_); }

void Process::send(int sig) {
    log::debug("Process:: send signal %s", strsignal(sig));
    kill(pid_, sig);
}
void Process::syscall(int sig) {
    status_ = 0;
    log::debug("Process:: syscall (%s)", strsignal(sig));
    long ret = ptrace(PTRACE_SYSCALL, pid_, 0, sig);
    check(ret != -1, "Process:: syscall: ptrace_syscall failed");
}
void Process::cont(int sig) {
    status_ = 0;
    log::debug("Process:: cont (%s)", strsignal(sig));
    long ret = ptrace(PTRACE_CONT, pid_, 0, sig);
    check(ret != -1, "Process:: cont: ptrace_cont failed");
}
void Process::attach() {
    log::debug("Process:: attach");
    long ret = ptrace(PTRACE_ATTACH, pid_, 0, 0);
    check(ret != -1, "Process:: attach: ptrace_attach failed");
}
void Process::detach(int sig) {
    log::debug("Process:: detach");
    long ret = ptrace(PTRACE_DETACH, pid_, 0, sig);
    check(ret != -1, "Process:: detach: ptrace_detach failed");
}

long Process::peek(long addr) {
    log::debug("Process:: peek/read data from 0x%x", addr);
    errno = 0;
    long ret = ptrace(PTRACE_PEEKDATA, pid_, addr, 0);
    log::debug("Process:: poke/read data 0x%x readed from 0x%x", ret, addr);
    if (errno != 0) {
        switch (errno) {
            case EBUSY:
                check(false, "Process:: poke: ptrace_poke failed: EBUSY: %s", strerror(errno));
            case EFAULT:
                check(false, "Process:: poke: ptrace_poke failed: EFAULT: %s", strerror(errno));
            case EINVAL:
                check(false, "Process:: poke: ptrace_poke failed: EINVAL: %s", strerror(errno));
            case EIO:
                check(false, "Process:: poke: ptrace_poke failed: EIO: %s", strerror(errno));
            case EPERM:
                check(false, "Process:: poke: ptrace_poke failed: EPERM: %s", strerror(errno));
            case ESRCH:
                check(false, "Process:: poke: ptrace_poke failed: ESRCH: %s", strerror(errno));
            default:
                check(false, "Process:: poke: ptrace_poke failed: %s", strerror(errno));
        }
    }
    return ret;
}

void Process::poke(long addr, long data) {
    log::debug("Process:: poke/write data 0x%x to 0x%x", data, addr);
    errno=0;
    long ret = ptrace(PTRACE_POKEDATA, pid_, addr, data);
    if (ret != 0) {
        switch (errno) {
            case EBUSY:
                check(false, "Process:: poke: ptrace_poke failed: EBUSY: %s", strerror(errno));
            case EFAULT:
                check(false, "Process:: poke: ptrace_poke failed: EFAULT: %s", strerror(errno));
            case EINVAL:
                check(false, "Process:: poke: ptrace_poke failed: EINVAL: %s", strerror(errno));
            case EIO:
                check(false, "Process:: poke: ptrace_poke failed: EIO: %s", strerror(errno));
            case EPERM:
                check(false, "Process:: poke: ptrace_poke failed: EPERM: %s", strerror(errno));
            case ESRCH:
                check(false, "Process:: poke: ptrace_poke failed: ESRCH: %s", strerror(errno));
            default:
                check(false, "Process:: poke: ptrace_poke failed: %s", strerror(errno));
        }
    }
    long wdata = peek(addr);
    check(wdata == data, "Process: poke: ptrace_poke wrote wrong data");
    log::debug("Process:: poke/write data 0x%x written to 0x%x", data, addr);

}

void Process::step(int sig) {
    log::debug("Process:: step one (%d)", sig);
    long ret = ptrace(PTRACE_SINGLESTEP, pid_, 0, sig);
    check(ret != -1, "Process: step: ptrace_singlestep failed");
}

void Process::step_to_main_module() {
    log::debug("Process:: step to main module: start");

    long main_module_start = 0xdeadbeef;
    long main_module_end = 0xdeadbeef;
    const char* cmodule;

    auto maps = parse_maps(pid());
    for (auto &entry : maps) {
        if(entry.perm[2] != 'x') continue;
        cmodule = basename(entry.path.c_str());
        if(strncmp(mainmodule_, cmodule, strlen(mainmodule_)) != 0) continue;
        main_module_start = entry.addr[0];
        main_module_end = entry.addr[1];
    }

    if(main_module_start == 0xdeadbeef) {
        send(SIGKILL);
        errno = 0;
        check(false, "Process:: step_to_main_module : unable to compute address");
    }

    while(1) {
        long pc = Arch::current()->get_pc(pid());
        log::debug("Step: PC = 0x%x", pc);
        if ((pc >= main_module_start) && (pc < main_module_end)) break;
        Process::step(0);
        wait(0);
    }
    log::debug("Process:: step to main module: end");
}

void Process::set_break(long addr) {
    log::debug("Process:: set break: address %x", addr);
    auto it = breaks_.find(addr);
    long addr_content = peek(addr);
    if (it == breaks_.end()) breaks_[addr] = addr_content;
    long mask = Arch::current()->get_breakpoint_mask();
    addr_content &= mask;
    log::debug("Process:: set break: break contents are %x", addr_content);
    poke(addr, addr_content);
}

void Process::set_break_size(long addr, long size) {
    log::debug("Process:: set break size: address %x size %d", addr, size);
    auto it = breaks_.find(addr);
    long addr_content = peek(addr);
    if (it == breaks_.end()) breaks_[addr] = addr_content;
    long mask;
	switch(Arch::current()->get_breakpoint_size()) {
        case BreakpointSize::HALF_WORD:
            mask = (((long)1 << ((2 - size)*8)) - 1);
            break;
        case BreakpointSize::WORD:
            mask = (((long)1 << ((4 - size)*8)) - 1);
            break;
        default:
        case BreakpointSize::DOUBLE_WORD:
            mask = (((long)1 << ((8 - size)*8)) - 1);
            break;
    }
    addr_content &= mask;
    log::debug("Process:: set break size: break contents are %x", addr_content);
    poke(addr, addr_content);
}

void Process::remove_break(long addr) {
    log::debug("Process:: remove_break: address %x", addr);
    auto it = breaks_.find(addr);
    if (it != breaks_.end()) poke(addr, it->second);
}

void *Process::get_segfault_addr() {
    log::debug("Process:: get_segfault_addr");
    siginfo_t siginfo;
    long ret = ptrace(PTRACE_GETSIGINFO, pid_, NULL, &siginfo);
    check(ret != -1, "Process:: get_segfault_addr: ptrace_getsiginfo failed");
    return siginfo.si_addr;
}

void Process::dyn_call(long addr, Arch::regbuf_type &regs, long sp, std::vector<unsigned long> &args) {
    int sig;
    log::debug("Process::dyn_call: Start");
    long cur_pc = Arch::current()->get_pc(pid());
    log::debug("Process::dyn_call: Dynamic call from PC = 0x%x", cur_pc);

    Arch::current()->read_regs(pid(), regs);
    Arch::current()->set_pc(pid(), addr);
    Arch::current()->set_sp(pid(), sp);
    Arch::current()->set_args(pid(), args);
    log::debug("Process::dyn_call: Temporal register state. Allow child to continue.");
    cont();
    wait(0);
    while (stopped() || signaled()) {
        if (stopped()) {
            sig = stop_sig();
        } else {
            sig = term_sig();
        }

        if (sig == SIGSEGV) {
            // Any segmentation fault during the calling of an inserted routine
            // should be just skipped since it is as a result of our tracing
            // library code and not from the application code.

            log::debug("Process::dyn_call: Segfault info: PC = 0x%x, RA = 0x%x, ADDR = 0x%x",
                       (long) Arch::current()->get_pc(pid()),
                       (long) Arch::current()->get_lnk(pid()),
                       (long) get_segfault_addr());
            log::debug(
                "Process::dyn_call: Ignoring segmentation fault during dynamic "
                "call");
            cont(SIGSEGV);
            wait(0);

        } else if (sig != SIGILL) {
            log::debug(
                "Process::dyn_call: Expected segmentation fault, found %s",
                strsignal(sig));
            cont(SIGTERM);
            wait(0);
            exit(1);

        } else {
            break;
        }
    }

    if (!stopped()) {
        if (exited()) {
            log::debug("Process:: dyn_call: Process exited with status: %d",
                       exit_status());
            exit(1);
        } else if (signaled()) {
            log::debug("Process:: dyn_call: Process signaled with signal: %d",
                       term_sig());
        }
    }

    checkx(stopped(), "Process::dyn_call: Expected stop signal");
    sig = stop_sig();
    checkx(sig == SIGILL,
           "Process::dyn_call: Expected Illegal Instruction, found %s",
           strsignal(sig));

    log::debug("Process::dyn_call: Restoring temporal register state.");
    Arch::current()->write_regs(pid(), regs);

    cur_pc = Arch::current()->get_pc(pid());
    log::debug("Process::dyn_call: Restarting from PC = 0x%x", cur_pc);

    log::debug("Process::dyn_call: End");
}

void Process::timeout(double time) {
    log::debug("Process::timeout: Start");
    detach();
    std::this_thread::sleep_for(std::chrono::duration<double>(time));
    touch();
    if (!active()) return;
    attach();
    waitfor(SIGSTOP);
    log::debug("Process::timeout: End");
}

void Process::steps(long steps) {
    log::debug("Process:: steps: start");
    for (long step = 0; step < steps; ++step) {
        long pc = Arch::current()->get_pc(pid());
        log::debug("Step: %d PC = 0x%x (contents: 0x%x)", step, pc, peek(pc));
        if (exited()) {
            log::debug("Step: %d Process exited with status %d", step, exit_status());
        }
        if (signaled()) {
            log::debug("Step: %d Process signaled with signal %s", step, strsignal(term_sig()));
        }
        if (stopped()) {
            log::debug("Step: %d Process stopped with signal %s", step, strsignal(stop_sig()));
        }
        Process::step(0);
        wait(0);
    }
    log::debug("Process:: steps: end");
}
