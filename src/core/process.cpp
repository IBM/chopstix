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

#include "core/arch.h"
#include "support/check.h"
#include "support/log.h"

#include <sys/ptrace.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace chopstix;

Process::~Process() {
    if (!active()) return;
    wait();
    // checkx(exited() || signaled(), "Process did not exit");
}

Process::Process(Process &&other) : pid_(other.pid_), status_(other.status_) {
    other.pid_ = -1;
    other.status_ = 0;
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
    pid_ = fork();
    check(pid_ != -1, "Unable to spawn process");
    if (pid_ != 0) return;
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    execvp(*argv, argv);
    // char **real_args = (char **)malloc(sizeof(char *) * (argc + 3));
    // real_args[0] = strdup("setarch");
    // real_args[1] = strdup("linux64");
    // real_args[2] = strdup("-R");
    // for (int i = 0; i < argc; ++i) real_args[2 + i] = argv[i];
    // execvp(*real_args, real_args);
}

void Process::exec_wait(char **argv, int argc) {
    pid_ = fork();
    check(pid_ != -1, "Unable to spawn process");
    if (pid_ != 0) {
        usleep(50000);
        return;
    }
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    usleep(10000);
    execvp(*argv, argv);
    // char **real_args = (char **)malloc(sizeof(char *) * (argc + 3));
    // real_args[0] = strdup("setarch");
    // real_args[1] = strdup("linux64");
    // real_args[2] = strdup("-R");
    // for (int i = 0; i < argc; ++i) real_args[2 + i] = argv[i];
    // execvp(*real_args, real_args);
}

void Process::ready() {
    wait();
    checkx(stopped(), "The provided command failed");
    checkx(stop_sig() == SIGTRAP, "The provided command failed");
}

void Process::abandon() { pid_ = -1; }
void Process::copy(long pid) { pid_ = pid; }

void Process::wait(int flags) {
    if (!active()) {
        log::debug("Process::wait: not active");
        return;
    }
    pid_t pid = waitpid(pid_, &status_, flags);
    log::debug("Process::wait: waitpid return: %d", pid);
    if (exited()) {
        log::debug("Process::wait: %d exited", pid);
        abandon();
    }
    if (signaled()) {
        log::debug("Process::wait: %d signaled by signal %d", pid, term_sig());
        if (term_sig() == SIGTERM) {
            log::debug("Process::wait: %d signaled by signal SIGTERM", pid);
            abandon();
            return;
        }
    } else {
        checkx(pid != -1, "Process aborted");
    }
}

void Process::waitfor(int which) {
    checkx(pid_ != -1, "No child process");
    log::debug("Process::waitfor: waiting for signal %d", which);
    while (active()) {
        wait();
        if (!stopped()) {
            log::debug("Process::waitfor: not stopped");
            return;
        }
        int sig = stop_sig();
        log::debug("Process::waitfor: signal %d received", sig);
        if (sig == which) {
            log::debug("Process::waitfor: %d signal! stop wait", sig);
            return;
        }
        cont(sig);
    }
    log::debug("Process::waitfor: %d never received while process active",
               which);
}

void Process::touch() {
    if (!active()) return;
    waitpid(pid_, &status_, WNOHANG);
    if (exited()) abandon();
}

bool Process::exited() { return WIFEXITED(status_); }
int Process::exit_status() { return WEXITSTATUS(status_); }
bool Process::signaled() { return WIFSIGNALED(status_); }
int Process::term_sig() { return WTERMSIG(status_); }
bool Process::stopped() { return WIFSTOPPED(status_); }
int Process::stop_sig() { return WSTOPSIG(status_); }

void Process::send(int sig) { kill(pid_, sig); }
void Process::syscall(int sig) {
    status_ = 0;
    log::debug("process::syscall");
    long ret = ptrace(PTRACE_SYSCALL, pid_, 0, sig);
    check(ret != -1, "ptrace_syscall");
}
void Process::cont(int sig) {
    status_ = 0;
    log::debug("process::cont");
    long ret = ptrace(PTRACE_CONT, pid_, 0, sig);
    check(ret != -1, "ptrace_cont");
}
void Process::attach() {
    log::debug("process::attach");
    long ret = ptrace(PTRACE_ATTACH, pid_, 0, 0);
    check(ret != -1, "ptrace_attach");
}
void Process::detach(int sig) {
    log::debug("process::detach");
    long ret = ptrace(PTRACE_DETACH, pid_, 0, sig);
    check(ret != -1, "ptrace_detach");
}

long Process::peek(long addr) {
    long ret = ptrace(PTRACE_PEEKDATA, pid_, addr, 0);
    return ret;
}

void Process::poke(long addr, long data) {
    long ret = ptrace(PTRACE_POKEDATA, pid_, addr, data);
    check(ret != -1, "ptrace_poke");
}

void Process::step(int sig) {
    log::debug("process::step");
    long ret = ptrace(PTRACE_SINGLESTEP, pid_, 0, sig);
    check(ret != -1, "ptrace_singlestep");
}

void Process::set_break(long addr) {
    log::debug("set break at %x", addr);
    auto it = breaks_.find(addr);
    long addr_content = peek(addr);
    if (it == breaks_.end()) breaks_[addr] = addr_content;
    long mask = Arch::current()->get_breakpoint_mask();
    addr_content &= mask;
    log::debug("break contents are %x", addr_content);
    poke(addr, addr_content);
}

void Process::remove_break(long addr) {
    log::debug("remove break at %x", addr);
    auto it = breaks_.find(addr);
    if (it != breaks_.end()) poke(addr, it->second);
}

void *Process::get_segfault_addr() {
    siginfo_t siginfo;
    long ret = ptrace(PTRACE_GETSIGINFO, pid_, NULL, &siginfo);
    check(ret != -1, "ptrace_getsiginfo");
    return siginfo.si_addr;
}

void Process::dyn_call(long addr, Arch::regbuf_type &regs, long sp, std::vector<unsigned long> &args) {
    int sig;
    log::debug("Process::dyn_call: Start");
    Arch::current()->read_regs(pid(), regs);
    Arch::current()->set_pc(pid(), addr);
    Arch::current()->set_sp(pid(), sp);
    Arch::current()->set_args(pid(), args);
    log::debug(
        "Process::dyn_call: Temporal register state. Allow child to continue.");
    cont();
    wait();
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

            log::debug("Segfault info: PC = %x, RA = %x, ADDR = %x",
                       Arch::current()->get_pc(pid()),
                       Arch::current()->get_lnk(pid()), get_segfault_addr());
            log::debug(
                "Process::dyn_call: Ignoring segmentation fault during dynamic "
                "call");
            cont(SIGSEGV);
            wait();

        } else if (sig != SIGILL) {
            log::debug(
                "Process::dyn_call: Expected segmentation fault, found %s",
                strsignal(sig));
            cont(SIGTERM);
            wait();
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
