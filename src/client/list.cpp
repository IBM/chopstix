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
 * NAME        : client/list.cpp
 * DESCRIPTION : Implementation of the `list` command.
 *               Lists different database statistics
 *               depending on input.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <future>

#include "support/cache.h"
#include "support/check.h"
#include "support/filesystem.h"
#include "support/string.h"

#include "core/module.h"
#include "core/path.h"

#include "database/connection.h"

using namespace chopstix;
namespace fs = filesystem;

namespace {

void list_sessions() {
    auto db = Connection::get_default(true);

    checkx(db.has_tables({"session", "sample"}),
           "No sampling information. Try 'chop sample'");

    auto find_sessions = db.query(
        "SELECT pid, cmd\n"
        "FROM session;\n");

    auto count_samples = db.query(
        "SELECT count(*)\n"
        "FROM sample\n"
        "WHERE pid = ?;\n");

    checkx(find_sessions.next(), "No sessions!");

    fmt::print("PID\tSAMPLES\tCOMMAND\n");

    do {
        auto rec = find_sessions.record();
        auto pid = rec.get<long>(0);
        auto cmd = rec.get<std::string>(1);

        count_samples.bind(1, pid);
        count_samples.next();
        auto count = count_samples.record().get<long>();
        count_samples.finish();

        fmt::print("{}\t{}\tchop sample {}\n", pid, count, cmd);
    } while (find_sessions.next());
}

template <typename T>
void format_scored(Query &q, const std::string &header,
                   std::function<void(std::shared_ptr<T>)> print_row) {
    auto get_ptr = [&]() { return q.record().get<std::shared_ptr<T>>(); };
    bool print_score = get_ptr()->score() > 0;
    float accum = 0;
    float accum_score = 0;
    int results = 0;
    bool summary = false;
    int limit = 0;

    accum = Option::get("accum").as_float();
    summary = Option::get("summary").as_bool();
    limit = Option::get("limit").as_int();

    if (!summary) {
        fmt::print("ID\t{}", header);
    }

    if (print_score) {
        if (!summary) {
            fmt::print("\tSCORE\tCOUNT");
        }
    }

    if (!summary) {
        fmt::print("\n");
    }

    do {
        auto ptr = get_ptr();
        if (!summary) {
            fmt::print("{}\t", ptr->rowid());
            print_row(ptr);
        }

        if (print_score) {
            if (!summary) {
                fmt::print("\t{:3.1f}%\t{}\t", ptr->score() * 100,
                           ptr->count());
            }
            accum_score += ptr->score() * 100;
            results++;
            if ((accum > 0 and accum_score > accum) or
                (limit > 0 and results >= limit)) {
                if (!summary) {
                    fmt::print("\n");
                }
                break;
            }
        }
        if (!summary) {
            fmt::print("\n");
        }
    } while (q.next());

    if (summary) {
        fmt::print("Results: {} (max results: {:d})\n", results,
                   Option::get("limit").as_int());
        fmt::print(
            "Score: {:3.1f}% (min score: {:3.1f}%, min accum score {:3.1f} "
            "%)\n",
            accum_score, Option::get("min").as_float(), accum);
    }
}

int list_modules() {
    auto db = Connection::get_default(true);
    checkx(db.has_tables({"module"}), "No modules. Try 'chop disasm'");

    auto q = Module::list_by_score(
        db, Option::get("min").as_float() / 100,
        Option::get("limit").as_int(Connection::max_rows()));

    checkx(q.next(), "No modules. Relax -min parameter.");

    format_scored<Module>(q, "NAME\tARCH", [](Module::shared_ptr m) {
        fmt::print("{}\t{}", fs::basename(m->name()), m->arch());
    });

    return 0;
}

int list_functions() {
    auto db = Connection::get_default(true);

    checkx(db.has_tables({"func"}), "No functions. Try 'chop disasm'");

    auto q = Function::list_by_score(db, Option::get("min").as_float() / 100,
                                     Connection::max_rows(),
                                     Option::get("minsize").as_float(0));

    checkx(q.next(), "No functions. Relax -min or -minsize parameters.");

    Cache<long, Module::shared_ptr> modules(
        [&](long module_id) { return Module::find_by_rowid(db, module_id); });

    auto demangler = Option::get("demangle").as_bool()
                         ? &string::demangle
                         : [](const std::string &s) { return s; };

    format_scored<Function>(
        q, "NAME\tSIZE\tMODULE", [&](Function::shared_ptr func) {
            auto module = modules.cache(func->module_id());
            fmt::print("{}\t{}\t{}", demangler(func->name()),
                       Function::compute_size(db, func->rowid()),
                       fs::basename(module->name()));
        });

    return 0;
}

void list_blocks() {
    auto db = Connection::get_default(true);
    checkx(db.has_tables({"block"}), "No blocks. Try 'chop disasm'");

    auto q = BasicBlock::list_by_score(
        db, Option::get("min").as_float() / 100,
        Option::get("limit").as_int(Connection::max_rows()));

    checkx(q.next(), "No basic blocks. Relax -min parameter.");

    Cache<long, Function::shared_ptr> funcs(
        [&](long func_id) { return Function::find_by_rowid(db, func_id); });

    Cache<long, Module::shared_ptr> modules(
        [&](long module_id) { return Module::find_by_rowid(db, module_id); });

    auto demangler = Option::get("demangle").as_bool()
                         ? &string::demangle
                         : [](const std::string &s) { return s; };

    format_scored<BasicBlock>(
        q, "ADDR\tFUNCTION\tMODULE", [&](BasicBlock::shared_ptr block) {
            auto func = funcs.cache(block->func_id());
            auto module = modules.cache(func->module_id());
            fmt::print("{:x}\t{}\t{}", block->addr(), demangler(func->name()),
                       fs::basename(module->name()));
        });
}

void list_paths() {
    auto db = Connection::get_default(true);

    checkx(db.has_tables({"path", "path_node"}),
           "No paths have been generated. Try 'chop search'");

    auto q = Path::list_by_score(
        db, Option::get("min").as_float() / 100,
        Option::get("limit").as_int(Connection::max_rows()));

    checkx(q.next(), "No paths. Relax -min parameter.");

    fmt::print("ID\tSCORE\tBLOCKS\n");

    do {
        auto path = q.record().get<Path>();
        path.load_db(db);
        fmt::print("{}\t{:3.1f}%", path.rowid(), path.score() * 100);
        char sep = '\t';
        for (auto &node : path.nodes()) {
            fmt::print("{}{}", sep, node->rowid());
            sep = ',';
        }
        fmt::print("\n");
    } while (q.next());
}

void list_samples() {
    Connection db(getopt("data").as_string());

    checkx(db.has_tables({"sample"}),
           "No sampling information. Try 'chop sample'");
    Query qu;
    if (getopt("pid")) {
        qu = db.query(R"SQL(
                select * from sample
                where pid = ?;
            )SQL");
        qu.bind(1, getopt("pid").as_int());
    } else {
        qu = db.query("select * from sample;");
    }

    auto columns = db.columns("sample");
    for (auto &col : columns) string::toupper(col);
    fmt::print("{}\n", string::join(columns, "\t"));

    while (qu.next()) {
        auto rec = qu.record();
        fmt::print("{:x}", rec.get<long>(0));
        for (int i = 1; i < rec.size(); ++i) {
            fmt::print("\t{}", rec.get<long>(i));
        }
        fmt::print("\n");
    };
}
}  // namespace

int run_list(int argc, char **argv) {
    auto opt_table = Option::arg(argc, argv);
    CHECK_USAGE(list, opt_table, "No statistic provided.");
    auto table = opt_table.as_string();

    PARSE_OPTIONS(list, argc, argv);

#define IF_LIST(Table)   \
    if (table == #Table) \
        list_##Table();  \
    else

    IF_LIST(sessions)
    IF_LIST(modules)
    IF_LIST(functions)
    IF_LIST(blocks)
    IF_LIST(paths)
    IF_LIST(samples)
    CHECK_USAGE(list, 0, "Unable to list '{}'", table);

    return 0;
}
