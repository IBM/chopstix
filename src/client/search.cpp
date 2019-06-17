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
 * NAME        : client/search.cpp
 * DESCRIPTION : Implementation of the `search` command.
 *               Creates a Search object based on input
 *               parameters.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <chrono>
#include <fstream>

#include "queries.h"

#include "core/function.h"
#include "core/module.h"
#include "core/path.h"
#include "core/search.h"

#include "database/connection.h"

#include "support/check.h"
#include "support/options.h"

using namespace chopstix;

using func_vec = Module::func_vec;

namespace {

void search_module(Search &search, func_vec &funcs) {
    auto opt_id = Option::get("id");
    auto opt_name = Option::get("name");

    CHECK_USAGE(search, opt_name.is_set() || opt_id.is_set(),
                "No id or name specified");

    auto db = Connection::get_default(true);

    auto module = opt_id.is_set()
                      ? Module::find_by_rowid(db, opt_id.as_int())
                      : Module::find_by_name(db, opt_name.as_string());
    checkx(module != nullptr, "Unable to find module");

    auto q = db.query(SQL_SELECT_FUNC
                      "WHERE module_id = ?\n"
                      "AND the_score >= ?\n"
                      "ORDER BY the_score DESC\n"
                      "LIMIT ?;\n");
    q.bind(1, module->rowid());
    q.bind(2, Option::get("min").as_float());
    q.bind(3, Option::get("limit").as_int(Connection::max_rows()));

    while (q.next()) {
        auto func = q.record().get<Function::shared_ptr>();
        fmt::format("{}\n", func->repr());
        funcs.push_back(func);
    }
}

void search_function(Search &search, func_vec &funcs) {
    auto opt_id = Option::get("id");
    auto opt_name = Option::get("name");

    CHECK_USAGE(search, opt_name.is_set() || opt_id.is_set(),
                "No id or name specified");

    auto db = Connection::get_default(true);

    auto func = opt_id.is_set()
                    ? Function::find_by_rowid(db, opt_id.as_int())
                    : Function::find_by_name(db, opt_name.as_string());
    checkx(func != nullptr, "Unable to find function");

    funcs.push_back(func);
}

void search_global(Search &search, func_vec &funcs) {
    auto db = Connection::get_default(true);

    auto q = Function::list_by_score(
        db, Option::get("min").as_float(),
        Option::get("limit").as_int(Connection::max_rows()), 0);

    checkx(q.next(), "No functions");

    do {
        auto func = q.record().get<Function::shared_ptr>();
        funcs.push_back(func);
    } while (q.next());
}

}  // namespace

int run_search(int argc, char **argv) {
    auto type = Option::arg(argc, argv);
    PARSE_OPTIONS(search, argc, argv);

    func_vec funcs;
    Search search;

    if (!type) {
        search_global(search, funcs);
    } else if (type.as_string() == "module") {
        search_module(search, funcs);
    } else if (type.as_string() == "function") {
        search_function(search, funcs);
    } else {
        CHECK_USAGE(search, 0, "Unknown type '{}'.", type.as_string());
    }

    auto db = Connection::get_default(true);
    auto make_loop = Option::get("fullfunc").as_bool();

    for (auto &func : funcs) {
        func->load_db(db);
        auto bes = func->get_backedges();
        search.add_backedges(bes);
        if (bes.empty() || make_loop) search.add_backedges({func->make_loop()});
    }

    auto target_count = Option::get("target-count");
    auto target_coverage = Option::get("target-coverage");
    auto heur_reps = Option::get("reps");
    auto heur_ins = Option::get("ins");
    auto opt_cutoff = Option::get("cutoff");
    auto opt_timeout = Option::get("timeout");

    if (!(target_count.is_set() || target_coverage.is_set() ||
          opt_timeout.is_set())) {
        log::warn(
            "You did not provide a termination condition:\n"
            "        -timeout <time>\n"
            "        -target-coverage <score>\n"
            "        -target-count <num>\n"
            "      This may take very long!");
    }

    if (target_count) search.target_count(target_count.as_int());
    if (target_coverage) search.target_coverage(target_coverage.as_float());
    if (heur_reps) search.heur_reps(heur_reps.as_int());
    if (heur_ins) search.heur_ins(heur_ins.as_int());
    if (opt_cutoff) search.set_cutoff(opt_cutoff.as_float());

    auto timeout = Progress::infty();
    if (opt_timeout) timeout.set_max(opt_timeout.as_time());

    std::ofstream log_file;
    auto opt_log = Option::get("log");
    if (opt_log) {
        log_file.open(opt_log.as_string());
        fmt::print(log_file, "#time coverage paths blocks\n");
        std::flush(log_file);
    }

    using the_clock = std::chrono::high_resolution_clock;
    using time_sec = std::chrono::duration<double>;
    auto tstamp = the_clock::now();
    auto tstart = the_clock::now();
    Progress info;

    double cov = 0.01;

    while (!timeout.complete() && search.next()) {
        auto now = the_clock::now();
        time_sec dt = now - tstamp;
        tstamp = now;

        timeout += dt.count();
        info += dt.count();

        if (opt_log) {
            if (search.coverage() >= cov) {
                cov += 0.01;
                time_sec rt = now - tstart;
                fmt::print(log_file, "{:.6f} {:f} {} {}\n", rt.count(),
                           search.coverage(), search.count(),
                           search.num_blocks());
                std::flush(log_file);
            }
        }

        if ((bool)info) {
            info.reset();
            fmt::print("{} Found {} paths ({:3.1f}\% coverage)\n",
                       search.progress(), search.count(),
                       search.coverage() * 100);
            if (opt_log) {
                time_sec rt = now - tstart;
                fmt::print(log_file, "{:.6f} {:f} {} {}\n", rt.count(),
                           search.coverage(), search.count(),
                           search.num_blocks());
                std::flush(log_file);
            }
        }
    }

    fmt::print("{} Found {} paths ({:3.1f}\% coverage)\n", search.progress(),
               search.count(), search.coverage() * 100);

    db.exec(SQL_CREATE_PATH);

    auto paths = search.paths();
    for (auto path : paths) {
        if (!path.find_by_hash(db)) path.save_db(db);
    }

    return 0;
}
