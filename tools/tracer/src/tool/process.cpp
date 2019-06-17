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
#include "process.h"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>

#include <assert.h>
#include <thread>
#include "common/check.h"
#include "common/log.h"

using namespace cxtrace;

Process::~Process() {
    if (!active()) return;
    touch();
    if (exited()) return;
    send(SIGKILL);
    wait();
}

Process::Process(Process &&other)
    : pid_(other.pid_), status_(other.status_), bps_(other.bps_) {
    other.drop();
}

Process &Process::operator=(Process &&other) {
    if (this != &other) {
        pid_ = other.pid_;
        status_ = other.status_;
        bps_ = other.bps_;
        other.drop();
    }
    return *this;
}

void Process::exec(char **argv) {
    assert(pid_ == -1 && "Process already running");
    pid_ = fork();
    check(pid_ != -1, "Unable to spawn process");
    if (pid_ != 0) return;
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(*argv, argv);
}

void Process::ready() {
    wait();
    checkx(stopped(), "Expected stop signal");
    checkx(stop_sig() == SIGTRAP, "Expected stop signal");
}

// Interactions
void Process::send(int sig) { int ret = kill(pid_, sig); }
void Process::touch() { wait(WNOHANG); }
void Process::wait(int flags) {
    pid_t p = waitpid(pid_, &status_, flags);
    check(p != -1, "Unable to wait");
    check(p == pid_ || p == 0, "Unknown process %d", p);
    if (exited()) drop();
}
void Process::waitfor(int which) {
    assert(pid_ != -1 && "No child process");
    while (1) {
        wait();
        if (!stopped()) return;
        int sig = stop_sig();
        if (sig == which) return;
        cont(sig);
    }
}

void Process::waitfor_sc(int which) {
    assert(pid_ != -1 && "No child process");
    while (1) {
        wait();
        if (!stopped()) return;
        int sig = stop_sig();
        if (sig == which || sig == SIGTRAP) return;
        syscall(sig);
    }
}

// Status parsing
bool Process::exited() const { return WIFEXITED(status_); }
int Process::exit_status() const { return WEXITSTATUS(status_); }
bool Process::signaled() const { return WIFSIGNALED(status_); }
int Process::term_sig() const { return WTERMSIG(status_); }
int Process::core_dump() const { return WCOREDUMP(status_); }
bool Process::stopped() const { return WIFSTOPPED(status_); }
int Process::stop_sig() const { return WSTOPSIG(status_); }
bool Process::continued() const { return WIFCONTINUED(status_); }

// Ptrace wrappers
void Process::syscall(int sig) {
    long ret = ptrace(PTRACE_SYSCALL, pid_, 0, sig);
    check(ret != -1, "ptrace_syscall");
}

void Process::cont(int sig) {
    long ret = ptrace(PTRACE_CONT, pid_, 0, sig);
    check(ret != -1, "ptrace_cont");
}

void Process::attach() {
    long ret = ptrace(PTRACE_ATTACH, pid_, 0, 0);
    check(ret != -1, "ptrace_attach");
}

void Process::detach() {
    long ret = ptrace(PTRACE_DETACH, pid_, 0, 0);
    check(ret != -1, "ptrace_detach");
}

void Process::step(int sig) {
    long ret = ptrace(PTRACE_SINGLESTEP, pid_, 0, sig);
    check(ret != -1, "ptrace_singlestep");
}

long Process::peek(long addr) {
    errno = 0;
    long ret = ptrace(PTRACE_PEEKDATA, pid_, addr, 0);
    check(errno == 0, "ptrace_peek");
    log::debug("peek [%x] = %x", addr, ret);
    return ret;
}

void Process::poke(long addr, long data) {
    long ret = ptrace(PTRACE_POKEDATA, pid_, addr, data);
    check(ret != -1, "ptrace_poke");
    log::debug("poke [%x] = %x", addr, data);
}

void Process::set_break(long addr) {
    log::debug("set break at %x", addr);
    auto bp = bps_.find(addr);
    if (bp == bps_.end()) bps_[addr] = peek(addr);
    // poke(addr, bps_[addr] | -1);
    poke(addr, 0);
}

void Process::remove_break(long addr) {
    log::debug("remove break at %x", addr);

    auto bp = bps_.find(addr);
    if (bp != bps_.end()) poke(addr, bp->second);
}

void Process::expect(int sig) {
    wait();
    checkx(stopped(), "Process did not stop");
    int stop = stop_sig();
    checkx(stop == sig, "Expected %s, found %s", strsignal(sig),
           strsignal(stop));
}

void Process::dyn_call(long addr, arch::regbuf_type &regs, long sp) {
    log::debug("dyn call to %x", addr);
    arch::read_regs(pid(), regs);
    arch::set_pc(pid(), addr);
    arch::set_sp(pid(), sp);
    cont();
    wait();
    checkx(stopped(), "Process did not stop");
    int sig = stop_sig();
    if (sig != SIGTRAP) {
        arch::read_regs(pid(), regs);
        arch::dump_regs(stderr, regs);
    }
    checkx(sig == SIGTRAP, "Expected trap/breakpoint, found %s",
           strsignal(sig));
    arch::write_regs(pid(), regs);
}

void Process::timeout(double time) {
    detach();
    std::this_thread::sleep_for(std::chrono::duration<double>(time));
    touch();
    if (!active()) return;
    attach();
    waitfor(SIGSTOP);
}
