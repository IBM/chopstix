#pragma once

#include "../process.h"
#include "../arch.h"

namespace chopstix {

struct TraceOptions {
    bool dump_registers, dump_maps, dump_info;
};

class Trace {
  public:
    Trace(int id, Process &child);
    ~Trace();

    void save(std::string trace_path, TraceOptions &opts);
  private:
    Arch::regbuf_type registers;
    long pc;
    int trace_id;
    long pid;
    long maps_size;
    char *maps;
};

}
