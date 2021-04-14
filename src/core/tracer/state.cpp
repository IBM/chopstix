#include "state.h"
#include "core/tracer/tracer.h"

namespace chopstix {

void TracerState::change_state() {
    tracer->set_state(next_state);
}

}
