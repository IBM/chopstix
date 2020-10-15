#include "state.h"

#include <vector>

namespace chopstix {

class TracerRegionOfInterestState : public TracerState {
  public:
    TracerRegionOfInterestState(Tracer *tracer) : TracerState(tracer) {}

    virtual void execute(Process &child);
  protected:
    virtual void handle_signal(Process &child, int signal);
};

class TracerRangedRegionOfInterestState : public TracerRegionOfInterestState {
  public:
    TracerRangedRegionOfInterestState(Tracer *tracer, std::vector<long> &end) :
        TracerRegionOfInterestState(tracer), end(end) {}

    virtual void on_state_start();
    virtual void on_state_finish();
  protected:
    virtual void handle_signal(Process &child, int signal);
  private:
    std::vector<long> &end;
};

}
