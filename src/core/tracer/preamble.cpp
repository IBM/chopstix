#include "preamble.h"

#include "tracer.h"
#include "../arch.h"
#include "../location.h"
#include "../../support/log.h"

#include <signal.h>

namespace chopstix {

bool TracerPreambleState::check_finished(Process &child) {
    log::debug("TracerRangedPreamble:: check_finished start");
    if (child.active()) {
        log::debug("TracerRangedPreamble:: check_finished end (False)");
        return false;
    }

    if (child.exited()) {
        int status = child.exit_status();
        if (status == 0) {
            tracer->stop();
            log::debug("TracerRangedPreamble:: check_finished end (True)");
            return true;
        } else {
            throw std::runtime_error("Child exited with non-zero status");
        }
    } else {
        throw std::runtime_error("Child finished without exiting.");
    }
}

void TracerTimedPreambleState::execute(Process &child) {
    log::debug("TracerTimedPreamble:: execute start");
    log::verbose("TracerTimedPreamble:: wait interval (%s seconds)", std::to_string(time));
    child.timeout(time);
    if (!check_finished(child) && tracer->should_trace()) {
        change_state();
    }
    log::debug("TracerTimedPreamble:: execute end");
}

void TracerRangedPreambleState::on_state_start(Process &child) {
    log::debug("TracerRangedPreamble:: on_start_start: setting start break points of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedPreambleState::on_state_finish(Process &child) {
    log::debug("TracerRangedPreamble:: on_state_finish: removing start break points of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedPreambleState::execute(Process &child) {
    log::debug("TracerRangedPreamble:: execute: continuing until Illegal instruction");
    child.cont();
    child.waitfor(SIGILL);
    if (!check_finished(child)) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("TracerRangedPreamble:: Stop at PC: %x", cur_pc);

        if (tracer->check_breakpoint(start)) {
            if (tracer->should_trace()) {
                log::verbose("TracerRangedPreamble:: execute: start region hit, start tracing");
                change_state();
                log::debug("TracerRangedPreamble:: Restarting at PC: %x" , cur_pc);
                child.syscall(0);
            } else {
                log::verbose("TracerRangedPreamble:: execute: start region hit, skip");
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
    log::debug("TracerRangedPreamble:: execute end");
}

void TracerRangedTimedPreambleState::on_state_start(Process &child) {
    log::debug("TracerRangedTimedPreamble:: on_start_start: setting start break points of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedTimedPreambleState::on_state_finish(Process &child) {
    log::debug("TracerRangedTimedPreamble:: on_state_finish: removing start break points of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedTimedPreambleState::execute(Process &child) {
    log::debug("TracerRangedTimedPreamble:: execute: continuing until Illegal instruction");
    child.cont();
    child.waitfor(SIGILL);
    if (!check_finished(child)) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("TracerRangedTimedPreamble:: Stop at PC: %x", cur_pc);

        if (tracer->should_trace()) {
            log::verbose("TracerRangedTimedPreamble:: execute: start region hit, start tracing");
            change_state();
            log::debug("TracerRangedTimedPreamble:: Restarting at PC: %x" , cur_pc);
        } else {
            log::verbose("TracerRangedTimedPreamble:: execute: start region hit, skip");
            tracer->set_breakpoint(start, false);
            child.steps(10);
            tracer->set_breakpoint(start, true);
            child.cont();
            child.waitfor(SIGILL);
        }
    }
    log::debug("TracerRangedTimedPreamble:: execute end");
}

}
