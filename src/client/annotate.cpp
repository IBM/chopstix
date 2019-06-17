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
 * NAME        : client/annotate.cpp
 * DESCRIPTION : Implementation of the `annotate` command.
 *               Simply executes the necessary SQL commands
 *               to update the local database.
 ***********************************************************/

#include "client.h"
#include "queries.h"
#include "usage.h"

#include "database/connection.h"
#include "support/options.h"
#include "support/progress.h"

#include "fmt/printf.h"

using namespace chopstix;

int run_annotate(int argc, char **argv) {
    PARSE_OPTIONS(annotate, argc, argv);
    auto db = Connection::get_default(true);

    auto normalize = Option::get("normalize").as_bool();

    Progress prog(4);

    fmt::print("{} Scoring instructions\n", prog);
    db.exec(SQL_SCORE_INSTS);
    prog.next();

    fmt::print("{} Scoring basic blocks\n", prog);
    db.exec(SQL_SCORE_BLOCKS);
    if (normalize) db.exec(SQL_NORM_BLOCKS);
    prog.next();

    fmt::print("{} Scoring functions\n", prog);
    db.exec(SQL_SCORE_FUNCS);
    prog.next();

    fmt::print("{} Scoring modules\n", prog);
    db.exec(SQL_SCORE_MODULES);
    prog.next();

    fmt::print("{} Finished\n", prog);
    return 0;
}
