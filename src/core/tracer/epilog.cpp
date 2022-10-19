#include "epilog.h"

#include "tracer.h"
#include "../process.h"
#include "../../support/log.h"

namespace chopstix {

void TracerEpilogState::execute(Process &child) {
    log::debug("TracerEpilogState:: execute start");
    tracer->stop_trace();
    change_state();
    log::debug("TracerEpilogState:: execute end");
}

}
