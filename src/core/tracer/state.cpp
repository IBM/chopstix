#include "state.h"
#include "core/tracer/tracer.h"

namespace chopstix {

void TracerState::change_state() {
    log::verbose("TracerState:: change_state start");
    tracer->set_state(next_state);
    log::verbose("TracerState:: change_state end");
}

}
