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
/******************************************************************************
 * NAME        : core/module.cpp
 * DESCRIPTION : A module is a binary or object file
 ******************************************************************************/

#include "module.h"

#include "queries.h"

#include <fstream>

#include "fmt/printf.h"
#include "support/check.h"
#include "support/log.h"
#include "support/stream.h"

#include "database/record.h"

namespace chopstix {

template <>
Module::shared_ptr Record::get(int *i) {
    auto rowid = get<long>(i);
    auto name = get<std::string>(i);
    auto arch = get<std::string>(i);
    auto module = Module::create(name, arch);
    module->rowid_ = rowid;
    module->range_ = get<Range>(i);
    module->count_ = get<long>(i);
    module->score_ = get<double>(i);
    return module;
}

template <>
Query &Query::bind(int *i, Module::shared_ptr module) {
    bind(i, module->name());
    bind(i, module->arch());
    bind(i, module->range());
    return *this;
}

}  // namespace chopstix

using namespace chopstix;

std::string Module::repr() const {
    return fmt::format("<Module {} ({})>", name_, arch_);
}

void Module::build_cfg() {
    for (auto func : funcs_) {
        func->link_blocks();
    }
}

void Module::load_obj() {
    funcs_.clear();
    std::stringstream ss;
    auto impl = Arch::get_impl(arch_);
    ss << impl->objdump(name_);
    std::string magic = name_ + ":";
    stream::skipline(ss);
    ss >> stream::expect(magic);
    stream::skipline(ss);
    checkx(!ss.fail(), "Format error in objdump");
    parse_stream(ss);
}

void Module::load_asm(const std::string &filename) {
    funcs_.clear();
    std::ifstream ifs(filename);
    parse_stream(ifs);
}

void Module::save_asm(const std::string &filename) {
    std::ofstream ofs(filename);
    for (auto &func : *this) {
        fmt::fprintf(ofs, "{:x}: <{}>:\n", func->addr(), func->name());
        for (auto &bb : func->blocks()) {
            for (auto &inst : bb->insts()) {
                fmt::fprintf(ofs, "  {:x}: {} {}\n", inst.addr, inst.raw,
                             inst.text);
            }
        }
        fmt::fprintf(ofs, "\n");
    }
}

std::istream &Module::parse_stream(std::istream &is) {
    using inst_vec = std::vector<Instruction>;
    inst_vec insts;
    func_ptr func;
    std::string buffer;
    auto impl = Arch::get_impl(arch_);
    while (std::getline(is, buffer)) {
        if (buffer.size() == 0) continue;
        if (buffer.front() == 'D') continue;
        std::stringstream line(buffer);
        if (std::isalnum(buffer.front())) {
            if (func) {
                func->build_blocks(insts);
                insts.clear();
                funcs_.push_back(func);
            }
            auto name = Function::parse_header(line);
            func = Function::create(name);
        } else {
            Instruction inst;
            inst.parse_line(line);
            if (inst.text == "") continue;
            impl->parse_inst(inst);
            insts.push_back(inst);
        }
    }

    if (func) {
        func->build_blocks(insts);
        funcs_.push_back(func);
    }

    range_ = {front()->range().begin, back()->range().end};

    for (auto func : funcs_) func->set_parent(shared_from_this());

    return is;
}

Module::shared_ptr Module::find_by_name(Connection &db, std::string name) {
    auto q = db.query(SQL_SELECT_MODULE "WHERE name LIKE ?;");
    if (name[0] != '%') name = "%" + name;
    q.bind(1, name);
    if (!q.next()) return nullptr;
    auto ret = q.record().get<shared_ptr>();
    if (q.next()) log::warn("Multiple modules with name '%s'", name);
    return ret;
}

Module::shared_ptr Module::find_by_value(Connection &db, std::string name,
                                         std::string arch) {
    Query q = db.query(SQL_SELECT_MODULE
                       "WHERE name = ?\n"
                       "AND arch = ?;\n");

    q.bind(1, name);
    q.bind(2, arch);
    return q.next() ? q.record().get<shared_ptr>(0) : nullptr;
}

Module::shared_ptr Module::find_by_rowid(Connection &db, long rowid) {
    Query q = db.query(SQL_SELECT_MODULE SQL_FIND_BY_ROWID);
    q.bind(1, rowid);
    return q.next() ? q.record().get<shared_ptr>(0) : nullptr;
}

Query Module::list_by_score(Connection &db, double cutoff, long limit) {
    auto q = db.query(SQL_SELECT_MODULE SQL_LIST_BY_SCORE);
    q.bind(1, cutoff);
    q.bind(2, limit);
    return q;
}

void Module::save_db(Connection &db) {
    log::verbose("Saving %s", repr());

    auto q = db.query(SQL_INSERT_MODULE);

    q.bind(1, shared_from_this());
    q.finish();
    rowid_ = db.last_rowid();

    for (auto func : funcs_) {
        func->save_db(db);
    }
}

void Module::load_db(Connection &db) {
    checkx(has_rowid(), "Unable to load module without rowid");

    auto q = Function::list_by_module(db, rowid_);

    funcs_.clear();
    while (q.next()) {
        auto func = q.record().get<func_ptr>();
        func->set_parent(shared_from_this());
        func->load_db(db);
        funcs_.push_back(func);
    }

    log::verbose("Loaded %s", repr());
}
