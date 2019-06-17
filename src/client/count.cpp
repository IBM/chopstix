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
 * NAME        : client/count.cpp
 * DESCRIPTION : Implementation of the `count` command.
 *               Simply executes the necessary SQL commands
 *               to update the local database.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include "queries.h"

#include "support/check.h"
#include "support/options.h"
#include "support/progress.h"

#include "database/connection.h"

#include "fmt/ostream.h"
#include "fmt/printf.h"

using namespace chopstix;

int run_count(int argc, char **argv) {
    PARSE_OPTIONS(count, argc, argv);

    auto db = Connection::get_default(true);

    Progress prog(5);

    fmt::print("{} Grouping samples\n", prog);
    db.exec(SQL_GROUP_SAMPLES);
    prog.next();

    fmt::print("{} Counting instructions\n", prog);
    db.exec(SQL_COUNT_INSTS_PRE);

    auto ids = db.query("SELECT rowid, name FROM module");
    checkx(ids.next(), "No modules. Check chop disasm command");
    do {
        auto rec = ids.record();
        auto id = rec.get<long>(0);
        auto name = rec.get<std::string>(1);
        fmt::print("       Counting instructions for module id: {} name: {}\n",
                   id, name);
        auto q = db.query(SQL_COUNT_INSTS);
        q.bind(1, id).finish();
    } while (ids.next());

    prog.next();

    fmt::print("{} Counting basic blocks\n", prog);
    db.exec(SQL_COUNT_BLOCKS);
    prog.next();

    fmt::print("{} Counting functions\n", prog);
    db.exec(SQL_COUNT_FUNCS);
    prog.next();

    fmt::print("{} Counting modules\n", prog);
    db.exec(SQL_COUNT_MODULES);
    prog.next();

    fmt::print("{} Finished\n", prog);
    return 0;
}
