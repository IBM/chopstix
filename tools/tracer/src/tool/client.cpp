/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#
*/
#include "client.h"

#include <cctype>
#include <cstring>
#include <linux/limits.h>
#include <unistd.h>

#include <algorithm>
#include <string>

#include "common/check.h"
#include "common/param.h"

namespace cxtrace {
std::string library_path() {
    char raw_path[PATH_MAX];
    auto n = readlink("/proc/self/exe", raw_path, PATH_MAX);
    std::string full_path = raw_path;
    auto pos = full_path.rfind("bin/cxtrace");
    return full_path.substr(0, pos) + "lib/libcxtrace.so";
}

void preload(std::string path) {
    char *env_preload = getenv("LD_PRELOAD");
    if (env_preload != NULL) {
        path += ':' + env_preload;
    }
    setenv("LD_PRELOAD", path.c_str(), 1);
}

void usage() {
    printf("Usage: cxtrace [<opts>..] [--] <cmd> [<args>..]\n"
            "Options:\n"
            "  -save              Save page contents\n"
            "  -trace <dir>       Path to trace directory\n"
            "  -logfile <path>    Base path for logfiles\n"
            "  -loglevel <lvl>    Logging level (debug,verbose,info,warn,error)\n"
            "  -begin <addr>      Begin addresses\n"
            "  -end <addr>        End addresses\n"
            "  -module <name>     Module to sample (only needed for Z)\n"
            "  -perm <perm>       Filter pages based on permissions\n"
            "                        r--   Read-only\n"
            "                        r*-   Read/Write\n"
            "                        r-x   Executable (i.e. text)\n"
            "  -drytrace          Do not dump page contents. Produce compressed output.\n"
            "                     Cannot be enabled together with -save\n"
            "  -group-iter <num>  Group <num> invocations together\n"
            "  -freq <pct>        Frequency with which invocations are traced\n"
            "  -max-pages <num>   Tracing stops after <num> pages\n"
            "  -max-traces <num>  Tracing stops after <num> traces\n"
            "  -tidle <time>      Idle time between traces\n"
            "  -tsample <time>    Time tracing is active\n"
            "  -nolib             Disable library injection (for debugging only)\n"
            );
}

void set_defaults() { setparam("trace", "/tmp/cxtrace", false); }

int parse_args(int argc, char **argv) {
    using option = std::pair<std::string, bool>;
    static option options[]{
        {"save", false},    // Save page contents
        {"trace", true},    // Path to trace
        {"log", true},      // Path to log file
        {"begin", true},    // Begin address
        {"end", true},      // End address
        {"module", true},   // Sampled module
        {"perm", true},     // Protect pages with permission
        {"seed", true}, 
        {"nolib", false},
        {"freq", true},     // Sampling frequency
        {"max-pages", true}, // Maximum number of pages
        {"max-traces", true},   // Finish after n samples
        {"tidle", true},    // Time idle between sampling
        {"tsample", true},  // Time waited while sampling
        {"loglevel", true}, // Set log level
        {"logfile", true},  // Set log output file
        {"drytrace", false}, // Dry tracing (no save contents)
        {"group-iter", true}, // Group n iterations
    };

    int parsed = 0;
    while (parsed < argc) {
        char *arg = argv[parsed];
        if (arg[0] != '-') break;
        while (*arg == '-') ++arg;
        std::string flag{arg};
        if (flag == "help") {
            usage();
            exit(0);
        }
        auto it =
            std::find_if(std::begin(options), std::end(options),
                         [&](const option &opt) { return opt.first == flag; });
        checkx(it != std::end(options), "Unknown argument '%s'", argv[parsed]);
        ++parsed;
        if (it->second) {
            checkx(parsed < argc, "Expected argument");
            checkx(argv[parsed][0] != '-', "Expected argument");
            setparam(arg, argv[parsed]);
            ++parsed;
        } else {
            setparam(arg, "1");
        }
    }
    return parsed;
}

double next_rand() {
    return (random() + 0.0) / RAND_MAX;
}

}
