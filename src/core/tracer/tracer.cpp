#include "tracer.h"

#include "core/process.h"
#include "state.h"
#include "../../support/check.h"
#include "../../support/log.h"
#include "../../support/filesystem.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace chopstix;

namespace fs = filesystem;

static std::string library_path() {
    char raw_path[PATH_MAX];
    auto n = readlink("/proc/self/exe", raw_path, PATH_MAX);
    std::string full_path{raw_path};
    auto pos = full_path.rfind("bin/chop");
    return full_path.substr(0, pos) + "/lib/libcxtrace.so";
}

static void preload(std::string path) {
    char *env_preload = getenv("LD_PRELOAD");
    if (env_preload != NULL) {
        path += ':' + env_preload;
    }
    setenv("LD_PRELOAD", path.c_str(), 1);
}

namespace chopstix {

Tracer::Tracer(std::string trace_path, bool dryrun, TraceOptions trace_options) {
    regs = Arch::current()->create_regs();
    this->trace_path = trace_path;
    tracing_enabled = !dryrun;
    this->trace_options = trace_options;
}

Tracer::~Tracer() {
    free(regs);
}

void Tracer::start(TracerState *initial_state, int argc, char **argv) {
    init(argc, argv);

    running = true;
    set_state(initial_state);

    while(running) {
        current_state->execute(child);
    }

    log::info("Tracer captured %d traces", trace_id);
}

void Tracer::stop() {
    running = false;
}

void Tracer::set_state(TracerState *state) {
    log::verbose("State change at PC = 0x%x",
                 Arch::current()->get_pc(child.pid()));

    if (current_state != nullptr) current_state->on_state_finish(child);
    state->on_state_start(child);

    current_state = state;
}

long Tracer::read_alt_stack() {
    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "%s/_alt_stack", trace_path);
    int stack_fd = ::open(fname, O_RDONLY);
    stack_t alt_stack;
    int n = read(stack_fd, &alt_stack, sizeof(stack_t));
    log::debug("alt stack at %x-%x", (unsigned long)alt_stack.ss_sp,
               (unsigned long)alt_stack.ss_sp + alt_stack.ss_size);
    return (long)alt_stack.ss_sp + alt_stack.ss_size / 2;
}

#define PERM_R PROT_READ
#define PERM_W PROT_WRITE
#define PERM_X PROT_EXEC
#define PERM_RW (PROT_READ | PROT_WRITE)
#define PERM_RX (PROT_READ | PROT_EXEC)
#define PERM_WX (PROT_WRITE | PROT_EXEC)
#define PERM_RWX (PROT_READ | PROT_WRITE | PROT_EXEC)

const char *encode_perm(int prot) {
    switch (prot) {
        case PERM_R: return "r--";
        case PERM_W: return "-w-";
        case PERM_X: return "--x";
        case PERM_RW: return "rw-";
        case PERM_RX: return "r-x";
        case PERM_WX: return "-wx";
        case PERM_RWX: return "rwx";
        default: return "---";
    }
}

static unsigned long upper_div(unsigned long a, unsigned long b) {
    return (a + b - 1) / b;
}

struct mmap_call {
    unsigned long addr;
    unsigned long length;
    unsigned long prot;
};

void Tracer::track_mmap() {
    std::vector<mmap_call> restrict_map;
    long pagesize = sysconf(_SC_PAGESIZE);
    long args[7];
    while (1) {
        int sig;

        child.syscall();
        child.wait();
        checkx(child.stopped(), "Child did not stop");
        sig = child.stop_sig();
        if (sig == SIGUSR1) {
            break;
        }
        checkx(sig == SIGTRAP, "Expected trap/breakpoint A, found %s",
               strsignal(sig));
        Arch::current()->read_regs(child.pid(), regs);

        long syscall_nr = Arch::current()->parse_syscall(regs);
        Arch::current()->parse_args(regs, args);

        //      0   , 1     , 2   , 3    , 4  , 5
        // mmap(addr, length, prot, flags, fd, offset)
        //        0   , 1
        // munmap(addr, length)

        child.syscall();
        child.wait();
        checkx(child.stopped(), "Child did not stop");
        sig = child.stop_sig();
        checkx(sig == SIGTRAP, "Expected trap/breakpoint B, found %s",
               strsignal(sig));
        Arch::current()->read_regs(child.pid(), regs);

        long ret = Arch::current()->parse_ret(regs);

        if (syscall_nr == SYS_mmap) {
            mmap_call mmap;
            mmap.addr = ret;
            mmap.length = args[1];
            mmap.prot = args[2];
            log::debug("mmap %x-%x %s", mmap.addr, mmap.addr + mmap.length,
                       encode_perm(mmap.prot));
            restrict_map.push_back(mmap);
        } else if (syscall_nr == SYS_munmap) {
            // TODO This is a hack!
            mmap_call munmap;
            munmap.addr = args[0];
            munmap.length = args[1];
            log::debug("munmap %x-%x", munmap.addr,
                       munmap.addr + munmap.length);
            auto end = std::remove_if(restrict_map.begin(), restrict_map.end(),
                                      [&](const mmap_call &mmap) {
                                          return mmap.addr == munmap.addr &&
                                                 mmap.length == munmap.length;
                                      });
            restrict_map.erase(end);
        }
    }
    char fname[PATH_MAX];
    sfmt::format(fname, sizeof(fname), "%s/_restrict_map", trace_path);
    FILE *fp = fopen(fname, "w");
    check(fp, "Unable to open restrict_map");
    for (auto &reg : restrict_map) {
        reg.length = upper_div(reg.length, pagesize) * pagesize;
        fprintf(fp, "%lx-%lx %s 0 0:0 0 [restrict]\n", reg.addr,
                reg.addr + reg.length, encode_perm(reg.prot));
    }
    fclose(fp);
}

void Tracer::init(int argc, char **argv) {
    setenv("LD_BIND_NOW", "1", 1);
    preload(library_path());
    child.exec(argv, argc);
    child.ready();
    unsetenv("LD_PRELOAD");
    log::verbose("Tracer:: Spawned child process %d", child.pid());
    sleep(2);

    track_mmap();

    alt_stack = read_alt_stack();
}

void Tracer::capture_trace() {
    char fname[PATH_MAX];
    FILE *fp;

    if (trace_options.dump_registers) {
        //Serialize Registers
        auto registers = Arch::current()->create_regs();
        Arch::current()->read_regs(child.pid(), registers);

        sfmt::format(fname, sizeof(fname), "%s/regs.%d", trace_path, trace_id);
        fp = fopen(fname, "w");
        Arch::current()->serialize_regs(fp, registers);
        fclose(fp);
    }

    if (trace_options.dump_maps) {
        //Serialize memory mapping
        sfmt::format(fname, sizeof(fname), "/proc/%d/maps", child.pid());
        std::string from{fname};
        sfmt::format(fname, sizeof(fname), "%s/maps.%d", trace_path, trace_id);
        std::string to{fname};
        fs::copy(from, to);
    }

    if (trace_options.dump_info) {
        //Serialize Program Counter
        auto pc = Arch::current()->get_pc(child.pid());

        sfmt::format(fname, sizeof(fname), "%s/info.%d", trace_path, trace_id);
        fp = fopen(fname, "w");
        fprintf(fp, "begin_addr %016lx\n", pc);
        fclose(fp);
    }
}

void Tracer::start_trace(bool isInvocationStart) {
    if(tracing_enabled) {
        capture_trace();

        // Pass breakpoint information to tracee
        auto breakpoints = child.get_breakpoint_info();
        char fname[PATH_MAX];
        sfmt::format(fname, sizeof(fname), "%s/_breakpoints", trace_path);
        FILE *fp = fopen(fname, "wb");
        fwrite(breakpoints.data(), sizeof(BreakpointInformation),
               breakpoints.size(), fp);
        fclose(fp);

        // Invoke trace start routine
        unsigned long arg0 = isInvocationStart ? 1 : 0;
        std::vector<unsigned long> args = {arg0};
        dyn_call("chopstix_start_trace", args);
    }
}

void Tracer::stop_trace() {
    trace_id++;
    if (tracing_enabled) {
        static std::vector<unsigned long> args;
        dyn_call("chopstix_stop_trace", args);
    }
}

void Tracer::save_page() {
    log::debug("Page accessed!");
}

Location& Tracer::get_symbol(std::string name) {
    auto location = symbols.find(name);
    if(location == symbols.end()) {
        Location loc = child.find_symbol(name, library_path());
        location = symbols.emplace(name, loc).first;
    }
    return location->second;
}

void Tracer::dyn_call(std::string symbol, std::vector<unsigned long> &args) {
    child.dyn_call(get_symbol(symbol), regs, alt_stack, args);
}

bool Tracer::symbol_contains(std::string symname, long addr) {
    return get_symbol(symname).entry().contains(addr);
}

void Tracer::set_breakpoint(std::vector<long> address, bool state) {
    for (auto addr : address) {
        if(state) child.set_break(addr + module_offset.addr());
        else child.remove_break(addr + module_offset.addr());
    }
}

bool RandomizedTracer::should_trace() {
    return ((random() + 0.0) / RAND_MAX) < probability;
}

bool IndexedTracer::should_trace() {
    if (indices.size() <= current_index) return false;

    if (indices[current_index] == current_execution++) {
        current_index++;
        return true;
    } else {
        return false;
    }
}

}
