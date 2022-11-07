#pragma once

#include "../process.h"
#include "../../support/log.h"
#include <vector>

namespace chopstix {

struct TraceOptions {
    bool dump_registers, dump_maps, dump_info;
    long max_traces;
};

class TracerState;

class Tracer {
  public:
    Tracer(std::string module, std::string trace_path, bool dryrun, TraceOptions trace_options);
    ~Tracer();

    void start(TracerState *initial_state, int argc, char **argv);
    void stop();

    void set_state(TracerState *state);
    void start_trace(bool isInvocationStart);
    void stop_trace();
    virtual bool should_trace() {
        log::debug("Tracer::should_trace");
        if (trace_options.max_traces > 0 && trace_id >= trace_options.max_traces) {
            log::debug("Tracer::should_trace false");
            return false;
        }
        log::debug("Tracer::should_trace true");
        return true;
    }
    void save_page();
    void dyn_call(std::string symbol, std::vector<unsigned long> &args);
    bool symbol_contains(std::string symbol, long addr);
    void set_breakpoint(std::vector<long> address, bool state);
    bool check_breakpoint(std::vector<long> address);
    void fix_breakpoint(std::vector<long> address);
    int trace_id = 0;
    TraceOptions trace_options;
  protected:
    bool running;
  private:
    void init(int argc, char **argv);
    void track_mmap();
    void compute_module_offset();

    long read_alt_stack();
    Location& get_symbol(std::string name);
    void capture_trace();

    TracerState *current_state = nullptr;
    Process child;
    std::map<std::string, Location> symbols;
    long alt_stack;
    Arch::regbuf_type regs;
    Location module_offset = Location::Address(0);
    std::string trace_path;
    std::string module;
    bool tracing_enabled;
};

class RandomizedTracer : public Tracer {
  public:
    RandomizedTracer(std::string module, std::string trace_path, bool dryrun,
                     TraceOptions trace_options, double probability) :
        Tracer(module, trace_path, dryrun, trace_options), probability(probability) {}

    virtual bool should_trace();
  private:
    double probability;
};

class IndexedTracer : public Tracer {
  public:
    IndexedTracer(std::string module, std::string trace_path, bool dryrun,
                  TraceOptions trace_options, std::vector<unsigned int> indices) :
        Tracer(module, trace_path, dryrun, trace_options), indices(indices) {}

    virtual bool should_trace();
  private:
    std::vector<unsigned int> indices;
    unsigned int current_index = 0;
    unsigned int current_execution = 0;
};

}
