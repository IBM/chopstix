#pragma once

#include "../process.h"

namespace chopstix {

class TracerState;

class Tracer {
  public:
    Tracer(std::string trace_path);
    ~Tracer();

    void start(TracerState *initial_state, int argc, char **argv);
    void stop();

    void set_state(TracerState *state);
    void start_trace();
    bool should_trace() {return false;}
    void save_page();
    void dyn_call(std::string symbol);
    void set_breakpoint(std::vector<long> address, bool state);
  private:
    void init(int argc, char **argv);
    void track_mmap();
    long read_alt_stack();

    TracerState *current_state = nullptr;
    Process child;
    std::map<std::string, Location> symbols;
    long alt_stack;
    Arch::regbuf_type regs;
    Location module_offset = Location::Address(0);
    int trace_id = 0;
    std::string trace_path;
    bool running;
};

}
