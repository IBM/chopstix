#include "preamble.h"

#include "tracer.h"
#include "../arch.h"
#include "../location.h"
#include "../../support/log.h"

#include <signal.h>

namespace chopstix {

void TracerPreambleState::do_trace(Process &child) {
    tracer->start_trace();
    log::verbose("Preamble:: call start tracing routine on child");
    tracer->dyn_call("chopstix_start_trace");
    change_state();
}

void TracerTimedPreambleState::execute(Process &child) {
    child.timeout(time);
    do_trace(child);
}

void TracerRangedPreambleState::on_state_start() {
    log::info("RangedPreamble:: setting start break point of region");
    tracer->set_breakpoint(start, true);
}

void TracerRangedPreambleState::on_state_finish() {
    log::info("RangedPreamble:: removing start break point of region");
    tracer->set_breakpoint(start, false);
}

void TracerRangedPreambleState::execute(Process &child) {
    log::debug("RangedPreamble:: continuing to region start");
    child.cont();
    child.waitfor(SIGILL);
    if (child.active()) {
        long cur_pc = Arch::current()->get_pc(child.pid());
        log::debug("run_trace:: Stop at PC: %x", cur_pc);

        if (tracer->should_trace()) {
            do_trace(child);
            child.syscall();
        } else {
            tracer->set_breakpoint(start, false);
            tracer->set_breakpoint(end, true);
            child.cont();
            child.waitfor(SIGILL);
            tracer->set_breakpoint(end, false);
            tracer->set_breakpoint(start, true);
        }
    } else {
        if (child.exited()) {
            int status = child.exit_status();
            if (status == 0) {
                tracer->stop();
            } else {
                throw std::runtime_error("Child exited with non-zero status");
            }
        } else {
            throw std::runtime_error("Child finished without exiting.");
        }
    }
}

}
