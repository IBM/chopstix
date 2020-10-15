#include "trace.h"

#include <linux/limits.h>

#include "../../support/log.h"
#include "../../support/filesystem.h"

namespace chopstix {

Trace::Trace(int id, Process &child) {
    trace_id = id;

    log::debug(
        "run_trace:: dumping registers, maps and info files for trace %d",
        trace_id);

    pid = child.pid();
    pc = Arch::current()->get_pc(pid);
    registers = Arch::current()->create_regs();
    Arch::current()->read_regs(pid, registers);

    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "/proc/%d/maps", pid);

    FILE *f = fopen(fname, "rb");
    fseek(f, 0, SEEK_END);
    maps_size = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    maps = (char *) malloc(maps_size + 1);
    fread(maps, 1, maps_size, f);
    fclose(f);

    maps[maps_size] = 0;
}

Trace::~Trace() {
    free(maps);
}

void Trace::save(std::string trace_path) {
    char fname[PATH_MAX];

    //Serialize Registers
    sfmt::format(fname, sizeof(fname), "%s/regs.%d", trace_path, trace_id);
    FILE *fp = fopen(fname, "w");
    Arch::current()->serialize_regs(fp, registers);
    fclose(fp);

    //Serialize memory mapping
    sfmt::format(fname, sizeof(fname), "%s/maps.%d", trace_path, trace_id);
    fp = fopen(fname, "w");
    fwrite(maps, maps_size, 1, fp);
    fclose(fp);

    //Serialize Program Counter
    sfmt::format(fname, sizeof(fname), "%s/info.%d", trace_path, trace_id);
    fp = fopen(fname, "w");
    fprintf(fp, "begin_addr %016lx\n", pc);
    fclose(fp);
}

}
