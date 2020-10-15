#pragma once

#include "state.h"

#include <vector>

namespace chopstix {

class TracerPrologueState : public TracerState {
  public:
    TracerPrologueState(Tracer *tracer) : TracerState(tracer) {}

    virtual void execute(Process &child);
};

class TracerTimedPrologueState : public TracerPrologueState {
  public:
    virtual void execute(Process &child);
};

}
