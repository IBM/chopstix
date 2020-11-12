#include "regionofinterest.h"

#include "../process.h"
#include "../../support/log.h"
#include "tracer.h"

#include <signal.h>

namespace chopstix {

void TracerRegionOfInterestState::execute(Process &child) {
    child.wait();
    if (!child.active()) {
        throw std::runtime_error("Not sure what happened");
    }
    if (child.stopped()) {
        handle_signal(child, child.stop_sig());
    } else {
        throw std::runtime_error("Child did not stop!");
    }
}

void TracerRegionOfInterestState::on_state_start(Process &child) {
    vdso_addr = child.find_module("[vdso]").addr();
}

void TracerRegionOfInterestState::handle_signal(Process &child, int signal) {
    if (signal == SIGSEGV) {
        // forward signal
        log::debug("RegionOfInterest:: catching signal SIGSEGV");
        tracer->save_page();

        log::debug("RegionOfInterest:: forward signal SIGSEGV");
        child.syscall(signal);
    } else if (signal == SIGTRAP) {
        log::debug("RegionOfInterest:: catching signal SIGTRAP");
        // enter syscall
        auto tmp_regs = Arch::current()->create_regs();
        Arch::current()->read_regs(child.pid(), tmp_regs);
        long sc_nr = Arch::current()->parse_syscall(tmp_regs);
        free(tmp_regs);
        long lnk_reg = Arch::current()->get_lnk(child.pid());
        bool in_vdso = lnk_reg >= vdso_addr;
        bool in_support =
            tracer->symbol_contains("chopstix_start_trace", lnk_reg);
        long cur_pc = Arch::current()->get_pc(child.pid());

        // finish syscall
        child.syscall();
        child.wait();

        // continue
        if (!child.exited()) {
            if (!in_support && !in_vdso) {
                log::verbose("RegionOfInterest:: system call %d from %x", sc_nr,
                          cur_pc);
                log::verbose("RegionOfInterest:: split trace at %x", cur_pc);
                tracer->stop_trace();
                tracer->start_trace();
            } else {
                log::debug("run_trace:: in support / in_vdso");
            }

            // continue
            child.syscall();
        } else {
            log::verbose("run_trace:: child exited with %d",
                         child.exit_status());
            tracer->stop();
        }

    } else {
        // forward signal
        log::debug("run_trace:: forward signal: %d", signal);
        child.syscall(signal);
    }
}

void TracerRangedRegionOfInterestState::on_state_start(Process &child) {
    TracerRegionOfInterestState::on_state_start(child);
    log::debug("run_trace:: setting end break point of region");
    tracer->set_breakpoint(end, true);
}

void TracerRangedRegionOfInterestState::on_state_finish(Process &child) {
    log::debug("run_trace:: removing end break point of region");
    tracer->set_breakpoint(end, false);
}

void TracerRangedRegionOfInterestState::handle_signal(Process &child,
                                                      int signal) {
    if (signal == SIGILL) change_state();
    else TracerRegionOfInterestState::handle_signal(child, signal);
}

void TracerTimedRegionOfInterestState::execute(Process &child) {
    log::verbose("TimedRegionOfInterest:: tracing for %s",
                 std::to_string(time));
    //The following timeout performs a wait, but it is for a specific signal
    //(SIGSTOP). This potentially means that we might miss out on some other
    //signals while we do so (e.g. syscalls, etc). An improvement might
    //be removing the internat wait in Process::timeout?
    child.timeout(time);
    change_state();
}

}
