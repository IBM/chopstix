#pragma once

#include "../process.h"
#include <vector>

namespace chopstix {

struct TraceOptions {
    bool dump_registers, dump_maps, dump_info;
};

class TracerState;

class Tracer {
  public:
    Tracer(std::string trace_path, bool dryrun, TraceOptions trace_options);
    ~Tracer();

    void start(TracerState *initial_state, int argc, char **argv);
    void stop();

    void set_state(TracerState *state);
    void start_trace(bool isInvocationStart);
    void stop_trace();
    virtual bool should_trace() {return true;}
    void save_page();
    void dyn_call(std::string symbol, std::vector<unsigned long> &args);
    bool symbol_contains(std::string symbol, long addr);
    void set_breakpoint(std::vector<long> address, bool state);
  private:
    void init(int argc, char **argv);
    void track_mmap();
    long read_alt_stack();
    Location& get_symbol(std::string name);
    void capture_trace();

    TracerState *current_state = nullptr;
    Process child;
    std::map<std::string, Location> symbols;
    long alt_stack;
    Arch::regbuf_type regs;
    Location module_offset = Location::Address(0);
    int trace_id = 0;
    std::string trace_path;
    bool running;
    bool tracing_enabled;
    TraceOptions trace_options;
};

class RandomizedTracer : public Tracer {
  public:
    RandomizedTracer(std::string trace_path, bool dryrun,
                     TraceOptions trace_options, double probability) :
        Tracer(trace_path, dryrun, trace_options), probability(probability) {}

    virtual bool should_trace();
  private:
    double probability;
};

class IndexedTracer : public Tracer {
  public:
    IndexedTracer(std::string trace_path, bool dryrun,
                  TraceOptions trace_options, std::vector<unsigned int> indices) :
        Tracer(trace_path, dryrun, trace_options), indices(indices) {}

    virtual bool should_trace();
  private:
    std::vector<unsigned int> indices;
    unsigned int current_index = 0;
    unsigned int current_execution = 0;
};

}
