#include "prologue.h"

#include "tracer.h"
#include "../process.h"
#include "../../support/log.h"

namespace chopstix {

void TracerPrologueState::execute(Process &child) {
    tracer->dyn_call("chopstix_stop_trace");
    change_state();
}

}
