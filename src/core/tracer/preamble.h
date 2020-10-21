#pragma once

#include "state.h"

#include <vector>

namespace chopstix {

class TracerPreambleState : public TracerState {
  public:
    TracerPreambleState(Tracer *tracer) :
        TracerState(tracer) {}

  protected:
    virtual void do_trace(Process &child);
    virtual bool check_finished(Process &child);
};

class TracerTimedPreambleState : public TracerPreambleState {
  public:
    TracerTimedPreambleState(Tracer *tracer, double time) :
        TracerPreambleState(tracer), time(time) {}

    virtual void execute(Process &child);
  private:
    double time;
};

class TracerRangedPreambleState : public TracerPreambleState {
  public:
    TracerRangedPreambleState(Tracer *tracer, std::vector<long> &start,
                              std::vector<long> &end) :
       TracerPreambleState(tracer), start(start), end(end) {}

    virtual void execute(Process &child);
    virtual void on_state_start(Process &child);
    virtual void on_state_finish(Process &child);
  private:
    std::vector<long> &start;
    std::vector<long> &end;
};

}
