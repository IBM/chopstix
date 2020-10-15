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

void TracerRegionOfInterestState::handle_signal(Process &child, int signal) {
    if (signal == SIGSEGV) {
        // forward signal
        log::debug("run_trace:: catching signal SIGSEGV");
        tracer->save_page();

        log::debug("run_trace:: forward signal SIGSEGV");
        child.syscall(signal);
    } else if (signal == SIGTRAP) {
        child.syscall();
        //child.wait();
    } else {
        // forward signal
        log::debug("run_trace:: forward signal: %d", signal);
        child.syscall(signal);
    }
}

void TracerRangedRegionOfInterestState::on_state_start() {
    log::debug("run_trace:: setting end break point of region");
    tracer->set_breakpoint(end, true);
}

void TracerRangedRegionOfInterestState::on_state_finish() {
    log::debug("run_trace:: removing end break point of region");
    tracer->set_breakpoint(end, false);
}

void TracerRangedRegionOfInterestState::handle_signal(Process &child,
                                                      int signal) {
    if (signal == SIGILL) change_state();
    else TracerRegionOfInterestState::handle_signal(child, signal);
}

}
