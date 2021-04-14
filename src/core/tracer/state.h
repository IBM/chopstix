#pragma once

namespace chopstix {

class Tracer;
class Process;

class TracerState {
public:
    TracerState(Tracer *tracer) : tracer(tracer) {}

    void set_next_state(TracerState *state) {next_state = state;}

    virtual void execute(Process &child) = 0;
    virtual void on_state_start(Process &child) {}
    virtual void on_state_finish(Process &child) {}
protected:
    void change_state();

    Tracer *tracer;
    TracerState *next_state;
};

}
