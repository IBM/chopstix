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
#include "usage.h"

#include "core/tracer/tracer.h"
#include "core/tracer/preamble.h"
#include "core/tracer/prologue.h"
#include "core/tracer/regionofinterest.h"
#include "support/check.h"
#include "support/filesystem.h"
#include "support/log.h"
#include "support/options.h"

#include <climits>

using namespace chopstix;

namespace fs = filesystem;

int run_trace(int argc, char **argv) {
    PARSE_OPTIONS(trace, argc, argv);

    log::verbose("run_trace:: Starting client setup");

    CHECK_USAGE(trace, argc > 0, "No command given.");

    CHECK_USAGE(trace,
                (getopt("begin").is_set() && getopt("end").is_set()) ||
                    (getopt("interval").is_set() && getopt("active").is_set()),
                "No tracing parameters (Region of Interest or Temporal-based "
                "sampling)");

    bool save_regs = getopt("save").as_bool();
    bool drytrace = getopt("access-only").as_bool();
    TraceOptions trace_options;
    trace_options.dump_registers = getopt("registers").as_bool();
    trace_options.dump_maps = getopt("maps").as_bool();
    trace_options.dump_info = getopt("info").as_bool();
    trace_options.max_traces = getopt("max-traces").as_int();
    std::string trace_path = getopt("trace-dir").as_string();
    double sample_freq = getopt("prob").as_float();
    bool notrace = !getopt("trace").as_bool();
    double tidle = getopt("interval").as_time();
    double tsample = getopt("active").as_time();
    int max_pages = getopt("max-pages").as_int();
    int group_iter = getopt("group").as_int();
    auto addr_begin = getopt("begin").as_hex_vec();
    auto addr_end = getopt("end").as_hex_vec();
    auto indices = getopt("indices").as_int_vec();
    bool with_region = addr_begin.size() > 0;

    checkx(!fs::exists(trace_path), "Output trace directory path '%s' already exists!", trace_path);
    fs::mkdir(trace_path);

    Tracer *tracer;
    if (getopt("prob").is_set()) {
        log::info("Performing randomized tracing");
        tracer = new RandomizedTracer(trace_path, notrace, trace_options,
                                      sample_freq);
    } else if(getopt("indices").is_set()) {
        log::info("Performing index tracing");
        std::vector<unsigned int> vec(begin(indices), end(indices));
        std::sort(vec.begin(), vec.end());
        tracer = new IndexedTracer(trace_path, notrace, trace_options, vec);
    } else {
        log::info("Tracing all invocations");
        tracer = new Tracer(trace_path, notrace, trace_options);
    }

    TracerState *preamble, *roi, *prologue;
    if (with_region) {
        preamble = new TracerRangedPreambleState(tracer, addr_begin, addr_end);
        roi = new TracerRangedRegionOfInterestState(tracer, addr_end);
        prologue = new TracerPrologueState(tracer);
    } else {
        preamble = new TracerTimedPreambleState(tracer, tidle);
        roi = new TracerTimedRegionOfInterestState(tracer, tsample);
        prologue = new TracerPrologueState(tracer);
    }

    preamble->set_next_state(roi);
    roi->set_next_state(prologue);
    prologue->set_next_state(preamble);

    tracer->start(preamble, argc, argv);

    if (getopt("gzip").as_bool()) {
        log::info("run_trace:: compressing trace directory");
        char cmd[PATH_MAX];
        snprintf(cmd, sizeof(cmd), "gzip -rf9 %s", trace_path.c_str());
        int ret = system(cmd);
        check(ret == 0, "'%s' command failed", cmd);
    }

    return 0;
}
