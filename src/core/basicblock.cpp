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
 * NAME        : core/basicblock.cpp
 * DESCRIPTION : A basic block is an uninterupted sequence of instructions.
 ******************************************************************************/

#include "basicblock.h"

#include "queries.h"

#include "function.h"

#include "support/check.h"
#include "support/log.h"

#include "fmt/format.h"

using namespace chopstix;

BasicBlock::BasicBlock(iterator begin, iterator end) : insts_(begin, end) {
    checkx(begin != end, "Attempting to create empty basic block");
    range_ = {begin->addr, end->addr};
}

std::string BasicBlock::repr() const {
    return fmt::format("<BasicBlock {:x}>", addr());
}

void BasicBlock::link_next(shared_ptr next) {
    next_.push_back(next);
    next->prev_.push_back(shared_from_this());
}

long BasicBlock::func_id() {
    if (func_id_) return func_id_;
    if (auto func = parent_.lock()) func_id_ = func->rowid();
    return func_id_;
}

long BasicBlock::module_id() {
    if (module_id_) return module_id_;
    if (auto func = parent_.lock()) module_id_ = func->module_id();
    return module_id_;
}

namespace chopstix {

template <>
BasicBlock::shared_ptr Record::get(int *i) {
    long rowid = get<long>(i);
    Range range = get<Range>(i);
    auto block = BasicBlock::create(range);
    block->rowid_ = rowid;
    block->count_ = get<long>(i);
    block->score_ = get<double>(i);
    block->func_id_ = get<long>(i);
    block->module_id_ = get<long>(i);
    return block;
}

template <>
Query &Query::bind(int *i, BasicBlock::shared_ptr block) {
    bind(i, block->range());
    bind(i, block->func_id());
    bind(i, block->module_id());
    return *this;
}

}  // namespace chopstix

BasicBlock::shared_ptr BasicBlock::find_by_rowid(Connection &db, long rowid) {
    auto q = db.query(SQL_SELECT_BLOCK SQL_FIND_BY_ROWID);
    q.bind(1, rowid);
    return q.next() ? q.record().get<shared_ptr>(0) : nullptr;
}

Query BasicBlock::list_by_score(Connection &db, double cutoff, long limit) {
    auto q = db.query(SQL_SELECT_BLOCK SQL_LIST_BY_SCORE);
    q.bind(1, cutoff);
    q.bind(2, limit);
    return q;
}

Query BasicBlock::list_by_func(Connection &db, long func_id) {
    auto q = db.query(SQL_SELECT_BLOCK "WHERE func_id = ?;");
    q.bind(1, func_id);
    return q;
}

void BasicBlock::save_db(Connection &db) {
    auto q = db.query(SQL_INSERT_BLOCK);
    q.bind(1, shared_from_this());
    q.finish();
    rowid_ = db.last_rowid();

    auto q2 = db.query(SQL_INSERT_INST);
    q2.bind(4, rowid_);
    q2.bind(5, func_id());
    q2.bind(6, module_id());

    db.transact([&]() {
        for (auto inst : insts_) {
            q2.bind(1, inst).finish();
        }
    });
}

void BasicBlock::load_db(Connection &db) {
    checkx(has_rowid(), "Unable to load basic block without rowid");

    auto q = Instruction::list_by_block(db, rowid_);

    insts_.clear();
    while (q.next()) {
        auto inst = q.record().get<Instruction>();
        insts_.push_back(inst);
    }
}

BasicBlock::edge_vec BasicBlock::get_edges() {
    edge_vec edges;
    edges.reserve(next_.size());
    for (auto &weakblock : next()) {
        if (auto block = weakblock.lock()) {
            edges.push_back(Edge(shared_from_this(), block));
        }
    }
    return edges;
}
