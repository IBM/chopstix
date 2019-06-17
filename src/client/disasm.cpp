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
 * NAME        : client/disasm.cpp
 * DESCRIPTION : Implementation of the `disasm` command.
 *               Creates and disassembles module objects
 *               and updates the database.
 ***********************************************************/

#include "client.h"
#include "queries.h"
#include "usage.h"

#include <algorithm>

#include "support/check.h"
#include "support/filesystem.h"
#include "support/options.h"
#include "support/progress.h"

#include "database/connection.h"
#include "database/utils.h"

#include "fmt/format.h"
#include "fmt/printf.h"

#include "core/module.h"

using namespace chopstix;
namespace fs = filesystem;

namespace {

std::vector<std::string> find_modules(Connection &db) {
    std::string query_str =
        "SELECT path FROM map\n"
        "WHERE perm = 'r-xp'\n"
        "AND path NOT LIKE '[%]'\n";

    auto ignore = Option::get("ignore").as_string_vec();
    for (auto &pattern : ignore) {
        database::sanitize(pattern);
        query_str += fmt::format("AND path NOT LIKE '{}'", pattern);
    }

    query_str += ";";

    auto query = db.query(query_str);

    std::vector<std::string> names;

    while (query.next()) {
        auto rec = query.record();
        names.push_back(rec.get<std::string>());
    }

    return names;
}

}  // namespace

int run_disasm(int argc, char **argv) {
    PARSE_OPTIONS(disasm, argc, argv);

    auto db = Connection::get_default(false);

    std::vector<std::string> module_names;

    Progress prog(1);

    if (argc == 0) {
        CHECK_USAGE(disasm, db.has_tables({"map"}),
                    "Unable to find modules (no sampling information).");
        fmt::print("{} Finding modules\n", prog);
        module_names = find_modules(db);
    } else {
        module_names.reserve(argc);
        for (int i = 0; i < argc; ++i) {
            checkx(fs::exists(argv[i]), "File not found: %s", argv[i]);
            module_names.push_back(fs::realpath(argv[i]));
        }
    }

    module_names.erase(std::unique(module_names.begin(), module_names.end()),
                       module_names.end());

    auto arch = Option::get("arch").as_string(Arch::get_machine());

    db.exec(SQL_CREATE_CFG);

    prog.set_max(module_names.size() * 2);

    for (auto name : module_names) {
        constexpr int max_width = 40;
        auto short_name = fs::basename(name);

        auto module = Module::find_by_value(db, name, arch);

        if (module) {
            prog.next(2);
            fmt::print("{} Cached module {}\n", prog, short_name);
        } else {
            fmt::print("{} Parsing module {}\n", prog, short_name);
            module = Module::create(name, arch);
            fmt::print("       Loading module {}\n", short_name);
            module->load_obj();
            fmt::print("       Building CFG {}\n", short_name);
            module->build_cfg();
            prog.next();
            fmt::print("{} Saving module {}\n", prog, short_name);
            module->save_db(db);
            prog.next();
            fmt::print("{} Built module {} (#{})\n", prog, short_name,
                       module->rowid());
        }
    }

    return 0;
}
