#include "preamble.h"

#include "tracer.h"
#include "../arch.h"
#include "../location.h"
#include "../../support/log.h"

#include <signal.h>

namespace chopstix {

bool TracerPreambleState::check_finished(Process &child) {
    log::verbose("TracerRangedPreamble:: check_finished start");
    if (child.active()) {
        log::verbose("TracerRangedPreamble:: check_finished end (False)");
        return false;
    }

    if (child.exited()) {
        int status = child.exit_status();
        if (status == 0) {
            tracer->stop();
            log::verbose("TracerRangedPreamble:: check_finished end (True)");
            return true;
        } else {
            throw std::runtime_error("Child exited with non-zero status");
        }
    } else {
        throw std::runtime_error("Child finished without exiting.");
    }
}

void TracerTimedPreambleState::execute(Process &child) {
    log::verbose("TracerTimedPreamble:: execute start");
    child.timeout(time);
    if (!check_finished(child) && tracer->should_trace()) {
        change_state();
    }
    log::verbose("TracerTimedPreamble:: execute end");
}

void TracerRangedPreambleState::on_state_start(Process &child) {
    log::verbose("TracerRangedPreamble:: on_start_start: setting start break points of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedPreambleState::on_state_finish(Process &child) {
    log::verbose("TracerRangedPreamble:: on_state_finish: removing start break points of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedPreambleState::execute(Process &child) {
    log::verbose("TracerRangedPreamble:: execute: continuing until Illegal instruction");
    child.cont();
    child.waitfor(SIGILL);
    if (!check_finished(child)) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::verbose("TracerRangedPreamble:: Stop at PC: %x", cur_pc);

        if (tracer->should_trace()) {
            change_state();
            log::verbose("TracerRangedPreamble:: Restarting at PC: %x" , cur_pc);
            child.syscall(0);
        } else {
            tracer->set_breakpoint(start, false);
            tracer->set_breakpoint(end, true);
            child.cont();
            child.waitfor(SIGILL);

            tracer->set_breakpoint(end, false);
            tracer->set_breakpoint(start, true);
        }
    }
    log::verbose("TracerRangedPreamble:: execute end");
}

}
