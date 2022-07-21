#include "regionofinterest.h"
#include "config.h"

#include "../process.h"
#include "../../support/log.h"
#include "tracer.h"

#include <signal.h>

namespace chopstix {

void TracerRegionOfInterestState::do_trace(Process &child) {
    log::debug("TracerRegionOfInterestState:: do_trace");
    tracer->start_trace(true);
}

void TracerRegionOfInterestState::execute(Process &child) {
    log::debug("TracerRegionOfInterestState:: execute start");
    child.wait(0);
    if (!child.active()) {
        throw std::runtime_error("Not sure what happened, traced process is not active");
    }
    if (child.stopped()) {
        log::debug("TracerRegionOfInterestState:: child stopped, handle signal");
        handle_signal(child, child.stop_sig());
    } else if (child.exited()) {
        log::verbose("TracerRegionOfInterestState:: child exited during region of interest");
        log::verbose("TracerRegionOfInterestState:: exit status: %d", child.exit_status());
        throw std::runtime_error("Exit during region of interest");
    } else if (child.signaled()) {
        log::verbose("TracerRegionOfInterestState:: child term signaled during region of interest");
        log::verbose("TracerRegionOfInterestState:: term signaled: %d", child.term_sig());
        throw std::runtime_error("Termination signal during region of interest");
    } else {
        throw std::runtime_error("Child did not stop!");
    }
    log::debug("TracerRegionOfInterestState:: execute end");
}

void TracerRegionOfInterestState::on_state_start(Process &child) {
    log::debug("TracerRegionOfInterestState:: on_state_start start");
    vdso_addr = child.find_module("[vdso]").addr();
    do_trace(child);
    log::debug("TracerRegionOfInterestState:: on_state_start end");
}

void TracerRegionOfInterestState::handle_signal(Process &child, int signal) {
    log::debug("TracerRegionOfInterestState:: handle_signal start");

    if (signal == SIGSEGV) {
        // forward signal (the child signal handler will take care of it)
        log::debug("TracerRegionOfInterestState:: catching signal SIGSEGV");
        log::debug("TracerRegionOfInterestState:: Segfault info: PC = 0x%x, RA = 0x%x, ADDR = 0x%x",
                   (long) Arch::current()->get_pc(child.pid()),
                   (long) Arch::current()->get_lnk(child.pid()),
                   (long) child.get_segfault_addr());
        tracer->save_page();
        log::debug("TracerRegionOfInterestState:: forward signal SIGSEGV");
        child.syscall(signal);
    } else if (signal == SIGTRAP) {
        // enter syscall
        log::debug("TracerRegionOfInterestState:: catching signal SIGTRAP");
        auto tmp_regs = Arch::current()->create_regs();
        Arch::current()->read_regs(child.pid(), tmp_regs);
        long sc_nr = Arch::current()->parse_syscall(tmp_regs);

        long lnk_reg = Arch::current()->get_lnk(child.pid());
        bool in_vdso = lnk_reg >= vdso_addr;
        bool in_support =
            tracer->symbol_contains("chopstix_start_trace", lnk_reg);
        long cur_pc = Arch::current()->get_pc(child.pid());

        log::debug("TracerRegionOfInterestState:: Lnk register: 0x%x", lnk_reg);
        if (in_support) {
            log::debug("TracerRegionOfInterestState:: in support");
        } else {
            log::debug("TracerRegionOfInterestState:: not in support");
        }
        if (in_vdso) {
            log::debug("TracerRegionOfInterestState:: in vdso");
        } else {
            log::debug("TracerRegionOfInterestState:: not in vdso");
        }

        log::debug("TracerRegionOfInterestState:: Current PC 0x%x", cur_pc);
        Arch::current()->debug_regs(tmp_regs);

        Arch::current()->free_regs(tmp_regs);

        bool sig_return = false;
#if defined(CHOPSTIX_SYSZ_SUPPORT)
        log::debug("TracerRegionOfInterestState:: in sigreturn");
        sig_return = (sc_nr == 173);
#endif

        // finish syscall
        child.syscall(0);
        child.wait(0);

        // continue
        if (!child.exited()) {
            if (!in_support && !in_vdso && !sig_return) {
                log::verbose("TracerRegionOfInterestState:: system call %d from PC 0x%x", sc_nr,
                          cur_pc);
                log::verbose("TracerRegionOfInterestState:: split trace at PC 0x%x", cur_pc);
                tracer->stop_trace();
                tracer->start_trace(false);

            } else {
                log::debug("TracerRegionOfInterestState:: in support / in_vdso");
            }
            // continue
            child.syscall(0);
        } else {
            log::debug("TracerRegionOfInterestState:: child exited with %d",
                         child.exit_status());
            tracer->stop();
        }

    } else {
        // forward signal
        log::debug("TracerRegionOfInterestState:: forward signal: %d", signal);
        child.syscall(signal);
    }
}

void TracerRangedRegionOfInterestState::on_state_start(Process &child) {
    // Set breakpoint before starting trace so it gets reset when pages are
    // dumped
    //
    log::debug("TracerRangedRegionOfInterestState:: on_state_start start");
    tracer->set_breakpoint(end, true);
    TracerRegionOfInterestState::on_state_start(child);
    log::debug("TracerRangedRegionOfInterestState:: on_state_start end");
}

void TracerRangedRegionOfInterestState::on_state_finish(Process &child) {
    log::debug("TracerRangedRegionOfInterestState:: removing end break point of region");
    tracer->set_breakpoint(end, false);
}

void TracerRangedRegionOfInterestState::handle_signal(Process &child,
                                                      int signal) {

    log::debug("TracerRangedRegionOfInterestState:: handle signal start");
    if (signal == SIGILL) {
        if (tracer->check_breakpoint(end)) {
            change_state();
        } else {
            // Restore contents and continue executing 
            tracer->fix_breakpoint(end);
            child.syscall(0);
        }
    } else {
        TracerRegionOfInterestState::handle_signal(child, signal);
    }
    log::debug("TracerRangedRegionOfInterestState:: handle signal end");
}

void TracerTimedRegionOfInterestState::execute(Process &child) {
    log::verbose("TracerTimedRegionOfInterestState:: execute: tracing for %s seconds",
                 std::to_string(time));
    //The following timeout performs a wait, but it is for a specific signal
    //(SIGSTOP). This potentially means that we might miss out on some other
    //signals while we do so (e.g. syscalls, etc). An improvement might
    //be removing the internal wait in Process::timeout?
    child.timeout(time);
    change_state();
    log::debug("TracerTimedRegionOfInterestState:: execute: end tracing for %s seconds",
                 std::to_string(time));
}

}
