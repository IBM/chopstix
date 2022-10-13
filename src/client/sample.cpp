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
/***********************************************************
 * NAME        : client/sample.cpp
 * DESCRIPTION : Implementation of the `sample` command.
 *               Opens perf_events and updates database.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <sstream>
#include <thread>

#include <signal.h>

#include "support/check.h"

#include "core/event.h"
#include "core/maps.h"
#include "core/process.h"
#include "database/connection.h"

#include "queries.h"

using namespace chopstix;

using event_list = std::vector<Event>;

namespace {

static void setup_database(Connection &db, event_list &events) {
    db.exec(SQL_CREATE_SAMPLE);
    db.exec(SQL_CREATE_SESSION);
    db.exec(SQL_CREATE_MAP);
    auto cols = db.columns("sample");
    for (auto &evt : events) {
        auto name = evt.name();
        if (std::find(cols.begin(), cols.end(), name) == cols.end()) {
            db.exec(fmt::format(SQL_ADD_EVENT, name));
        }
    }
}

static Query prepare_insert_sample(Connection &db, event_list &events) {
    std::stringstream header;
    std::stringstream body;
    for (auto &evt : events) {
        header << " ,[" << evt.name() << "]";
        body << " ,?";
    }
    return db.query(fmt::format(SQL_INSERT_SAMPLE, header.str(), body.str()));
}

static void setup_events(event_list &events, long pid) {
    auto &leader = events.front();
    leader.setup();

    if (getopt("period")) leader.set_period(getopt("period").as_int());
    if (getopt("freq")) leader.set_freq(getopt("freq").as_int());

    for (auto &evt : events) {
        log::debug("Opening event '%s'", evt.name());
        evt.open(pid, -1, leader.fd(), 0);
    }

    leader.enable();
    leader.start_buffering();
}

static void insert_maps(Connection &db, long pid) {
    auto maps = parse_maps(pid);
    auto query = db.query(SQL_INSERT_MAP);
    query.bind(1, pid);
    for (auto &entry : maps) {
        query.bind(2, entry.addr[0])
            .bind(3, entry.addr[1])
            .bind(4, std::string(entry.perm))
            .bind(5, entry.path)
            .finish();
    }
}

void insert_session(Connection &db, long pid, char **argv) {
    std::stringstream cmd;
    std::string sep = "";
    while (*argv) {
        cmd << sep << *argv;
        sep = " ";
        ++argv;
    }
    auto query = db.query("insert into session (pid, cmd) values (?, ?)");
    query.bind(1, pid);
    query.bind(2, cmd.str());
    query.finish();
}

static void terminate_children() { kill(0, SIGKILL); }
}  // namespace

int run_sample(int argc, char **argv) {
    PARSE_OPTIONS(sample, argc, argv);

    /* Parse and check command line options */

    auto opt_db = getopt("data");
    auto opt_pid = getopt("pid");
    auto opt_events = getopt("events");
    auto opt_timeout = getopt("timeout");
    auto opt_cpu = getopt("cpu");

    checkx(opt_db.is_set(), "Database not set");
    checkx(opt_events.is_set(), "Events not set");

    checkx(!opt_pid.is_set() || opt_timeout.is_set(),
           "Option -pid requires -timeout");
    checkx(argc > 0 || opt_pid.is_set(), "No <command> or <pid> provided");

    /* Create/attach to process */
    Process child;
    std::atomic<bool> running(true);

    /* atexit(terminate_children); */

    /* Setup database and events */
    Connection db(opt_db.as_string());
    event_list events = Event::parse_all(opt_events.as_string());

    setup_database(db, events);
    auto query = prepare_insert_sample(db, events);
    std::thread stop_onexit;

    if (opt_cpu.as_int() != -1) {
        /* Pin to a particular CPU */
        cpu_set_t mask;
        CPU_ZERO(&mask);
        int ret = sched_getaffinity(0, sizeof(cpu_set_t), &mask);
        if (ret != 0) { perror("ERROR: while getting affinity"); exit(EXIT_FAILURE);};

        if (!CPU_ISSET(opt_cpu.as_int(), &mask)) {
            perror("ERROR: CPU specified is not allowed"); exit(EXIT_FAILURE);
        }

        log::debug("chop sample: pinning to CPU: %d", opt_cpu.as_int());
        CPU_ZERO(&mask);
        CPU_SET(opt_cpu.as_int(), &mask);
        ret = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
        if (ret != 0) { perror("ERROR: while setting affinity"); exit(EXIT_FAILURE);};
    }

    if (opt_pid.is_set()) {
        child.copy(opt_pid.as_int());
    } else {
        log::debug("chop sample: executing command provided");
        setenv("LD_BIND_NOW", "1", 1);
        child.exec_wait(argv, argc);
    }

    if (!opt_pid.is_set()) {
        log::debug("chop sample: waiting for ready");
        child.ready();
        log::debug("chop sample: process ready");

        child.step_to_main_module();
        log::info("chop sample: process in main module");
        log::debug("chop sample: process in main");

        insert_session(db, child.pid(), argv);
        insert_maps(db, child.pid());

        log::debug("chop sample: setting events");
        setup_events(events, child.pid());

        child.cont();

        stop_onexit = std::thread([&]() {
            child.wait(0);
            running = false;
            log::debug("chop sample: stop_onexit called");
        });
        stop_onexit.detach();
    } else {
        log::debug("chop sample: setting events");
        setup_events(events, child.pid());
        insert_session(db, child.pid(), argv);
        insert_maps(db, child.pid());
    }

    if (opt_timeout.is_set()) {
        log::debug("chop sample: setting timeout");
        std::thread stop_ontimeout([&]() {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(opt_timeout.as_time()));
            running = false;
            log::debug("chop sample: process timed out");
        });
        stop_ontimeout.detach();
    }

    auto &prof = events.front();
    Sample::value_list last(events.size(), 0);

    while (running) {

        if (prof.poll()) {
            auto samples = prof.sample();

            db.transact([&]() {
                for (auto &smp : samples) {
                    query.bind(1, smp.ip)
                        .bind(2, smp.pid)
                        .bind(3, smp.tid)
                        .bind(4, smp.time);
                    for (unsigned i = 0; i < smp.data.size(); ++i) {
                        query.bind(i + 5, smp.data[i] - last[i]);
                    }
                    query.finish();
                    query.clear();
                    last = smp.data;
                }
            });
        }
    }

    if (opt_pid.is_set()) {
        child.abandon();
    } else {
        if (child.active()) child.send(SIGKILL);
    }

    if (prof.num_lost()) {
        printf("WARNING: Records read: %lu\n", prof.num_samples());
        printf("WARNING: Records lost: %lu\n", prof.num_lost());
    }

    return 0;
}
