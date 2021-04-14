#include "state.h"

#include <vector>

namespace chopstix {

class TracerRegionOfInterestState : public TracerState {
  public:
    TracerRegionOfInterestState(Tracer *tracer) : TracerState(tracer) {}

    virtual void execute(Process &child);
    virtual void on_state_start(Process &child);
  protected:
    virtual void handle_signal(Process &child, int signal);
    virtual void do_trace(Process &child);
    long vdso_addr;
};

class TracerRangedRegionOfInterestState : public TracerRegionOfInterestState {
  public:
    TracerRangedRegionOfInterestState(Tracer *tracer, std::vector<long> &end) :
        TracerRegionOfInterestState(tracer), end(end) {}

    virtual void on_state_start(Process &child);
    virtual void on_state_finish(Process &child);
  protected:
    virtual void handle_signal(Process &child, int signal);
  private:
    std::vector<long> &end;
};

class TracerTimedRegionOfInterestState : public TracerRegionOfInterestState {
  public:
    TracerTimedRegionOfInterestState(Tracer *tracer, double time) :
        TracerRegionOfInterestState(tracer), time(time) {}

    virtual void execute(Process &child);
  private:
    double time;
};

}
