#include "prologue.h"

#include "tracer.h"
#include "../process.h"
#include "../../support/log.h"

namespace chopstix {

void TracerPrologueState::execute(Process &child) {
    tracer->stop_trace();
    change_state();
}

}
