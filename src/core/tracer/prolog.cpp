#include "prolog.h"

#include "tracer.h"
#include "../arch.h"
#include "../location.h"
#include "../../support/log.h"

#include <signal.h>

namespace chopstix {

bool TracerPrologState::check_finished(Process &child) {
    log::debug("TracerRangedProlog:: check_finished start");
    if (child.active()) {
        log::debug("TracerRangedProlog:: check_finished end (False)");
        return false;
    }

    if (child.exited()) {
        int status = child.exit_status();
        if (status == 0) {
            tracer->stop();
            log::debug("TracerRangedProlog:: check_finished end (True)");
            return true;
        } else {
            throw std::runtime_error("Child exited with non-zero status");
        }
    } else {
        throw std::runtime_error("Child finished without exiting.");
    }
}

void TracerTimedPrologState::execute(Process &child) {
    log::debug("TracerTimedProlog:: execute start");
    log::verbose("TracerTimedProlog:: wait interval (%s seconds)", std::to_string(time));
    child.timeout(time);
    if (!check_finished(child) && tracer->should_trace()) {
        change_state();
    }
    log::debug("TracerTimedProlog:: execute end");
}

void TracerRangedPrologState::on_state_start(Process &child) {
    log::debug("TracerRangedProlog:: on_start_start: setting start break points of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedPrologState::on_state_finish(Process &child) {
    log::debug("TracerRangedProlog:: on_state_finish: removing start break points of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedPrologState::execute(Process &child) {
    log::debug("TracerRangedProlog:: execute: continuing until Illegal instruction");
    child.cont();
    child.waitfor(SIGILL);
    if (!check_finished(child)) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("TracerRangedProlog:: Stop at PC: %x", cur_pc);

        if (tracer->check_breakpoint(start)) {
            if (tracer->should_trace()) {
                log::verbose("TracerRangedProlog:: execute: start region hit, start tracing");
                change_state();
                log::debug("TracerRangedProlog:: Restarting at PC: %x" , cur_pc);
                child.syscall(0);
            } else {
                log::verbose("TracerRangedProlog:: execute: start region hit, skip");
                tracer->set_breakpoint(start, false);
                tracer->set_breakpoint(end, true);
                child.cont();
                child.waitfor(SIGILL);
                tracer->set_breakpoint(end, false);
                tracer->set_breakpoint(start, true);
            }
        } else {
            // Restore contents and continue executing
            tracer->fix_breakpoint(start);
            child.cont();
            child.waitfor(SIGILL);
        }
    }
    log::debug("TracerRangedProlog:: execute end");
}

void TracerRangedTimedPrologState::on_state_start(Process &child) {
    log::debug("TracerRangedTimedProlog:: on_start_start: setting start break points of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedTimedPrologState::on_state_finish(Process &child) {
    log::debug("TracerRangedTimedProlog:: on_state_finish: removing start break points of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedTimedPrologState::execute(Process &child) {
    log::debug("TracerRangedTimedProlog:: execute: continuing until Illegal instruction");
    child.cont();
    child.waitfor(SIGILL);
    if (!check_finished(child)) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("TracerRangedTimedProlog:: Stop at PC: %x", cur_pc);

        if (tracer->should_trace()) {
            log::verbose("TracerRangedTimedProlog:: execute: start region hit, start tracing");
            change_state();
            log::debug("TracerRangedTimedProlog:: Restarting at PC: %x" , cur_pc);
        } else {
            log::verbose("TracerRangedTimedProlog:: execute: start region hit, skip");
            tracer->set_breakpoint(start, false);
            child.steps(10);
            tracer->set_breakpoint(start, true);
            child.cont();
            child.waitfor(SIGILL);
        }
    }
    log::debug("TracerRangedTimedProlog:: execute end");
}

}
