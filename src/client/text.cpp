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
 * NAME        : client/text.cpp
 * DESCRIPTION : Implementation of the `text` command.
 *               Displays assembly instructions in a given
 *               text format.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <fstream>
#include <iostream>

#include "text_format.h"

#include "support/check.h"
#include "support/options.h"

#include "database/connection.h"

#include "core/module.h"

using namespace chopstix;

namespace {

std::unique_ptr<TextFormat> get_format() {
    using format = std::unique_ptr<TextFormat>;
    auto opt = Option::get("fmt").as_string("text");
    if (opt == "text") return format(new TextFormat());
    if (opt == "annotate") return format(new AnnotFormat());
    if (opt == "mpt") {
        auto opt_arch = Option::get("arch");
        CHECK_USAGE(text, opt_arch.is_set(), "No architecture specified");
        auto arch = Arch::get_impl(opt_arch.as_string());
        return format(new MptFormat(arch->get_endianess()));
    }
    CHECK_USAGE(text, 0, "Unknown format '{}'", opt);
    exit(1); // Tell compiler to shut up
}

std::unique_ptr<std::ostream> get_output() {
    using out_stream = std::unique_ptr<std::ostream>;
    auto opt = Option::get("out");
    return opt ? out_stream(new std::ofstream(opt.as_string()))
               : out_stream(new std::ostream(std::cout.rdbuf()));
}

void text_module() {
    auto opt_name = Option::get("name");
    auto opt_id = Option::get("id");
    CHECK_USAGE(text, opt_id.is_set() || opt_name.is_set(),
                "No id or name specified");

    auto db = Connection::get_default(true);
    checkx(db.has_tables({"module"}), "No modules. Try 'chop disasm'");
    auto module = opt_id ? Module::find_by_rowid(db, opt_id.as_int())
                         : Module::find_by_name(db, opt_name.as_string());
    checkx(module != nullptr, "Unable to find module");
    module->load_db(db);

    auto format = get_format();
    auto out = get_output();

    format->header(*out);
    format->format(*out, *module);
}

void text_function() {
    auto opt_name = Option::get("name");
    auto opt_id = Option::get("id");
    CHECK_USAGE(text, opt_id.is_set() || opt_name.is_set(),
                "No id or name specified");

    auto db = Connection::get_default(true);
    checkx(db.has_tables({"func"}), "No functions. Try 'chop disasm'");
    auto func = opt_id ? Function::find_by_rowid(db, opt_id.as_int())
                       : Function::find_by_name(db, opt_name.as_string());
    checkx(func != nullptr, "Unable to find function");
    func->load_db(db);

    auto format = get_format();
    auto out = get_output();

    format->header(*out);
    format->format(*out, *func);
}

void text_block() {
    auto opt_id = Option::get("id");
    CHECK_USAGE(text, opt_id.is_set(), "No block id specified");

    auto db = Connection::get_default(true);
    checkx(db.has_tables({"block"}), "No basic blocks. Try 'chop disasm'");
    auto block = BasicBlock::find_by_rowid(db, opt_id.as_int());
    checkx(block != nullptr, "Unable to find basic block");

    block->load_db(db);

    auto format = get_format();
    auto out = get_output();

    format->header(*out);
    format->format(*out, *block);
}

void text_path() {
    auto opt_id = Option::get("id");
    CHECK_USAGE(text, opt_id.is_set(), "No path id specified");

    auto db = Connection::get_default(true);
    checkx(db.has_tables({"path"}), "No paths. Try 'chop search'");
    auto path = Path::find_by_rowid(db, opt_id.as_int());
    checkx(path != nullptr, "Unable to find path");

    path->load_db(db);

    auto format = get_format();
    auto out = get_output();

    format->header(*out);
    format->format(*out, *path);
}

}  // namespace

int run_text(int argc, char **argv) {
    auto opt_type = Option::arg(argc, argv);
    CHECK_USAGE(text, opt_type.is_set(), "No type information.");
    auto type = opt_type.as_string();

    PARSE_OPTIONS(text, argc, argv);

#define TEXT_CASE(Name)  \
    if (type == #Name) { \
        text_##Name();   \
    } else

    TEXT_CASE(module)
    TEXT_CASE(function)
    TEXT_CASE(block)
    TEXT_CASE(path)
    CHECK_USAGE(text, 0, "Unable to show text for '{}'", type);

    return 0;
}
