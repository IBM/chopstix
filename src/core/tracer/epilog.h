#pragma once

#include "state.h"

#include <vector>

namespace chopstix {

class TracerEpilogState : public TracerState {
  public:
    TracerEpilogState(Tracer *tracer) : TracerState(tracer) {}

    virtual void execute(Process &child);
};

class TracerTimedEpilogState : public TracerEpilogState {
  public:
    virtual void execute(Process &child);
};

}
