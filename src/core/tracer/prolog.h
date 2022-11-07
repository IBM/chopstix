#pragma once

#include "state.h"

#include <string>
#include <vector>

namespace chopstix {

class TracerPrologState : public TracerState {
  public:
    TracerPrologState(Tracer *tracer) :
        TracerState(tracer) {}

  protected:
    virtual bool check_finished(Process &child);
};

class TracerTimedPrologState : public TracerPrologState {
  public:
    TracerTimedPrologState(Tracer *tracer, double time) :
        TracerPrologState(tracer), time(time) {}

    virtual void execute(Process &child);
  private:
    double time;
};

class TracerRangedPrologState : public TracerPrologState {
  public:
    TracerRangedPrologState(Tracer *tracer,
                              std::vector<long> &start,
                              std::vector<long> &end) :
       TracerPrologState(tracer), start(start), end(end) {}

    virtual void execute(Process &child);
    virtual void on_state_start(Process &child);
    virtual void on_state_finish(Process &child);
  private:
    std::vector<long> &start;
    std::vector<long> &end;
};

class TracerRangedTimedPrologState : public TracerPrologState {
  public:
    TracerRangedTimedPrologState(Tracer *tracer,
                                   std::vector<long> &start,
                                   double time) :
       TracerPrologState(tracer), start(start), time(time) {}

    virtual void execute(Process &child);
    virtual void on_state_start(Process &child);
    virtual void on_state_finish(Process &child);
  private:
    std::vector<long> &start;
    double time;
};

}
