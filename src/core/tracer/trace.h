#pragma once

#include "../process.h"
#include "../arch.h"

namespace chopstix {

class Trace {
  public:
    Trace(int id, Process &child);
    ~Trace();

    void save(std::string trace_path);
  private:
    Arch::regbuf_type registers;
    long pc;
    int trace_id;
    long pid;
    long maps_size;
    char *maps;
};

}
