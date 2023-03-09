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
 * NAME        : client/view.cpp
 * DESCRIPTION : Implementation of the `view` command.
 *               Displays assembly instructions in a given
 *               view format.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "support/check.h"
#include "support/options.h"

#include "database/connection.h"

#include "core/function.h"
#include "core/path.h"

using namespace chopstix;

namespace {

using block_iter = Function::iterator;
using formatter =
    std::function<std::ostream &(std::ostream &, block_iter, block_iter)>;

std::unique_ptr<std::ostream> get_output() {
    using out_stream = std::unique_ptr<std::ostream>;
    auto opt = Option::get("out");
    return opt ? out_stream(new std::ofstream(opt.as_string()))
               : out_stream(new std::ostream(std::cout.rdbuf()));
}

std::string get_color(double score) {
    if (score > 0.2) return "red";
    if (score > 0.1) return "orange";
    if (score > 0.01) return "yellow";
    if (score > 0) return "white";
    return "grey";
}

std::ostream &render_compact(std::ostream &os, block_iter begin,
                             block_iter end) {
    fmt::print(os, "digraph path_{:x} {{\n", begin->get()->addr());
    fmt::print(os, "  node [shape=record];\n\n");
    // Draw basic blocks
    for (auto bb = begin; bb != end; ++bb) {
        auto ptr = bb->get();
        fmt::print(os,
                   "  bb_{0}[label=\"{{[{0}] 0x{1:x}}}|{{{2:3.2f}%|{3} "
                   "ins}}\", style=filled, fillcolor=\"{4}\"]\n",
                   ptr->rowid(), ptr->addr(), ptr->score() * 100, ptr->size(),
                   get_color(ptr->score()));
    }
    // Draw edges
    for (auto bb = begin; bb != end; ++bb) {
        auto ptr = bb->get();
        for (auto &ln : ptr->next()) {
            auto ptr2 = ln.lock();
            if (ptr2 != nullptr) {
                fmt::print(os, "  bb_{} -> bb_{};\n", ptr->rowid(),
                           ptr2->rowid());
            }
        }
    }
    fmt::print(os, "}}\n");
    return os;
}

std::ostream &render_detail(std::ostream &os, block_iter begin,
                            block_iter end) {
    fmt::print(os, "digraph path_{:x} {{\n", begin->get()->addr());
    fmt::print(os, "  node [shape=record];\n\n");
    // Draw basic blocks
    for (auto bb = begin; bb != end; ++bb) {
        auto ptr = bb->get();
        fmt::print(os, "  bb_{0}[label=\"{{[{0}] 0x{1:x}", ptr->rowid(),
                   ptr->addr());
        std::string sep = "|";
        std::stringstream ss;
        fmt::print(ss, "{:3.2f}%", ptr->score() * 100);
        for (auto &in : *ptr) {
            fmt::print(os, "{}\n    {}", sep, in.text);
            fmt::print(ss, "{}\n    {:3.2f}%", sep, in.score() * 100);
            sep = "\\n";
        }
        fmt::print("\n  }}|{{{}", ss.str());
        fmt::print("\n  }}\", style=fill, color=\"{}\"];\n\n",
                   get_color(ptr->score()));
    }
    // Draw edges
    for (auto bb = begin; bb != end; ++bb) {
        auto ptr = bb->get();
        for (auto &ln : ptr->next()) {
            auto ptr2 = ln.lock();
            if (ptr2 != nullptr) {
                fmt::print(os, "  bb_{} -> bb_{};\n", ptr->rowid(),
                           ptr2->rowid());
            }
        }
    }
    fmt::print(os, "}}\n");
    return os;
}

formatter get_format() {
    auto fmt = Option::get("fmt").as_string();

#define FMT_CASE(Name)        \
    if (fmt == #Name) {       \
        return render_##Name; \
    } else

    FMT_CASE(compact)
    FMT_CASE(detail)
    CHECK_USAGE(view, 0, "Unknown format '{}'", fmt);
    exit(1); // Tell compiler to shut up
}

void view_function() {
    auto opt_name = Option::get("name");
    auto opt_id = Option::get("id");
    CHECK_USAGE(view, opt_id.is_set() || opt_name.is_set(),
                "No id or name specified");

    auto db = Connection::get_default(true);
    auto func = opt_id ? Function::find_by_rowid(db, opt_id.as_int())
                       : Function::find_by_name(db, opt_name.as_string());
    checkx(func != nullptr, "Unable to find function");
    func->load_db(db);

    auto out = get_output();
    auto fmt = get_format();
    fmt(*out, func->begin(), func->end());
}

void view_path() {
    auto opt_id = Option::get("id");
    CHECK_USAGE(view, opt_id.is_set(), "No path id specified");

    auto db = Connection::get_default(true);
    auto path = Path::find_by_rowid(db, opt_id.as_int());
    checkx(path != nullptr, "Unable to find path");

    path->load_db(db);

    auto out = get_output();
    auto fmt = get_format();
    fmt(*out, path->begin(), path->end());
}

}  // namespace

int run_view(int argc, char **argv) {
    auto opt_type = Option::arg(argc, argv);
    CHECK_USAGE(view, opt_type.is_set(), "View what?");
    auto type = opt_type.as_string();

    PARSE_OPTIONS(view, argc, argv);

#define VIEW_CASE(Name)  \
    if (type == #Name) { \
        view_##Name();   \
    } else

    VIEW_CASE(function)
    VIEW_CASE(path)
    CHECK_USAGE(view, 0, "Unable to view CFG for '{}'", type);

    return 0;
}
