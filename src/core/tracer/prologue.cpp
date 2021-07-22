#include "prologue.h"

#include "tracer.h"
#include "../process.h"
#include "../../support/log.h"

namespace chopstix {

void TracerPrologueState::execute(Process &child) {
    log::verbose("TracerPrologueState:: execute start");
    tracer->stop_trace();
    change_state();
    log::verbose("TracerPrologueState:: execute end");
}

}
