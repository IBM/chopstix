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
 * NAME        : core/function.cpp
 * DESCRIPTION : A function is a collection of basic blocks.
 ******************************************************************************/

#include "function.h"

#include "queries.h"

#include <algorithm>

#include "fmt/format.h"
#include "module.h"
#include "support/check.h"
#include "support/log.h"
#include "support/stream.h"

using namespace chopstix;

std::string Function::repr() const {
    return fmt::format("<Function {}>", name_);
}

void Function::build_blocks(inst_vec &insts) {
    using mark_vec = std::vector<inst_vec::iterator>;

    blocks_.clear();
    range_ = {insts.front().addr, insts.back().addr};
    mark_vec marks;

    // Mark branch targets and insts. after branches
    for (inst_vec::iterator inst = insts.begin(); inst < insts.end(); ++inst) {
        auto branch = inst->branch;
        if (!branch) continue;

        marks.push_back(inst + 1);
        if (!branch->reg() && range_.contains(branch->target)) {
            auto addr = branch->target;
            auto target = std::find_if(
                insts.begin(), insts.end(),
                [=](const Instruction &inst) { return inst.addr == addr; });
            if (target == insts.end()) {
                log::warn(
                    "Branch from %x: Unable to find instruction with address "
                    "%x",
                    branch->source, addr);
            } else {
                marks.push_back(target);
            }
        }
    }

    // Sort and remove duplicate marks
    std::sort(marks.begin(), marks.end());
    marks.push_back(insts.end());
    marks.erase(std::unique(marks.begin(), marks.end()), marks.end());

    // Build basic blocks by traversing marks
    auto head = insts.begin();
    auto mark = marks.begin();
    while (*mark < insts.end()) {
        if (*mark == head) {
            ++mark;
        } else {
            blocks_.push_back(BasicBlock::create(head, *mark));
            head = *(mark++);
        }
    }

    // Insert last block (if any)
    if (head != insts.end()) {
        blocks_.push_back(BasicBlock::create(head, insts.end()));
    }

    for (auto block : blocks_) block->set_parent(shared_from_this());
}

void Function::link_blocks() {
    auto cur_bb = blocks_.begin();

    for (auto next_bb = cur_bb + 1; next_bb < blocks_.end();
         cur_bb = next_bb++) {
        auto block = *cur_bb;
        auto branch = block->back().branch;
        if (branch) {  // Mark b/c of branch here
            // Also link with next bb?
            if (branch->cond() || branch->link()) {
                block->link_next(*next_bb);
            }

            if (!branch->reg()) {
                auto target = branch->target;
                if (range_.contains(target)) {
                    // Branch target in text
                    auto target_bb = find_if(
                        blocks_.begin(), blocks_.end(),
                        [=](block_ptr bb) { return bb->addr() == target; });
                    if (target_bb != blocks_.end()) {
                        block->link_next(*target_bb);
                    }
                }
            }
        } else {  // Mark b/c of branch target
            block->link_next(*next_bb);
        }
    }

    // Sometimes the last instructions in a function are padding
    if (blocks_.size() != 1 && blocks_.back()->prev().empty()) {
        blocks_.pop_back();
    }
}

std::string Function::parse_header(std::istream &is) {
    std::string name;
    is >> std::hex >> stream::skip<long>;
    is >> std::ws >> stream::expect('<') >> name;
    if (is.fail()) return "";
    if (name.back() != ':') {
        is.setstate(std::ios::failbit);
        return "";
    }
    name.pop_back();
    if (name.back() != '>') {
        is.setstate(std::ios::failbit);
        return "";
    }
    name.pop_back();
    return name;
}

Function::block_ptr Function::find_block(addr_type addr) {
    for (auto bb : *this) {
        if (bb->range().contains(addr)) {
            return bb;
        }
    }
    return nullptr;
}

long Function::module_id() {
    if (module_id_) return module_id_;
    if (auto module = parent_.lock()) module_id_ = module->rowid();
    return module_id_;
}

long Function::num_ins() {
    if (num_ins_) return num_ins_;
    for (auto block : blocks_) {
        num_ins_ += block->size();
    }
    return num_ins_;
}

namespace chopstix {

template <>
Query &Query::bind(int *i, Function::shared_ptr func) {
    bind(i, func->name());
    bind(i, func->range());
    bind(i, func->module_id());
    return *this;
}

template <>
Function::shared_ptr Record::get(int *i) {
    auto rowid = get<long>(i);
    auto name = get<std::string>(i);
    auto func = Function::create(name);
    func->rowid_ = rowid;
    func->range_ = get<Range>(i);
    func->count_ = get<long>(i);
    func->score_ = get<double>(i);
    func->module_id_ = get<long>(i);
    return func;
}

}  // namespace chopstix

Function::shared_ptr Function::find_by_name(Connection &db, std::string name) {
    auto q = db.query(SQL_SELECT_FUNC "WHERE name = ?\n");
    q.bind(1, name);
    if (!q.next()) return nullptr;
    auto ret = q.record().get<shared_ptr>();
    if (q.next()) log::warn("Multiple functions with name '%s'", name);
    return ret;
}

Function::shared_ptr Function::find_by_rowid(Connection &db, long rowid) {
    auto q = db.query(SQL_SELECT_FUNC SQL_FIND_BY_ROWID);
    q.bind(1, rowid);
    return q.next() ? q.record().get<shared_ptr>() : nullptr;
}

Query Function::list_by_score(Connection &db, double cutoff, long limit,
                              long size) {
    auto q = db.query(SQL_SELECT_FUNC_BY_SCORE_AND_SIZE);
    q.bind(1, cutoff);
    q.bind(2, size);
    q.bind(3, limit);
    return q;
}

Query Function::list_by_module(Connection &db, long module_id) {
    auto q = db.query(SQL_SELECT_FUNC "WHERE module_id = ?;");
    q.bind(1, module_id);
    return q;
}

long Function::compute_size(Connection &db, long id) {
    auto q = db.query(SQL_COUNT_FUNC_SIZE);
    q.bind(1, id);
    checkx(q.next(), "No function with id {}", id);
    return q.record().get<long>(0);
}

void Function::save_db(Connection &db) {
    log::verbose("Saving %s", repr());
    auto q = db.query(SQL_INSERT_FUNC);
    q.bind(1, shared_from_this());
    q.finish();
    rowid_ = db.last_rowid();

    for (auto block : blocks_) {
        block->save_db(db);
    }

    auto q2 = db.query(SQL_INSERT_EDGE);
    auto edges = get_edges();
    db.transact([&]() {
        for (auto edge : edges) {
            log::verbose("Saving %s", edge.repr());
            q2.bind(1, edge);
            q2.finish();
        }
    });
}

void Function::load_db(Connection &db) {
    checkx(has_rowid(), "Unable to load function without rowid");

    auto q = BasicBlock::list_by_func(db, rowid_);

    blocks_.clear();
    while (q.next()) {
        auto block = q.record().get<block_ptr>();
        block->set_parent(shared_from_this());
        block->load_db(db);
        blocks_.push_back(block);
    }

    Edge::link_blocks(db, blocks_);

    log::verbose("Loaded %s", repr());
}

Function::edge_vec Function::get_edges() {
    edge_vec edges;
    edges.reserve(size());
    for (auto block : blocks_) {
        auto add = block->get_edges();
        edges.insert(edges.end(), add.begin(), add.end());
    }
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    return edges;
}

Function::edge_vec Function::get_backedges() {
    auto edges = get_edges();
    auto it = std::remove_if(edges.begin(), edges.end(), [](const Edge &edge) {
        return !edge.is_backedge();
    });
    edges.erase(it, edges.end());
    return edges;
}
