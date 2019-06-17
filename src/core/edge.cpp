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
 * NAME        : core/edge.cpp
 * DESCRIPTION : A control flow edge (i.e. branch) between two basic blocks
 ******************************************************************************/

#include "edge.h"

#include <algorithm>

#include "fmt/format.h"

#include "sql/select_edge.h"

#include "support/log.h"

#include "basicblock.h"
#include "function.h"

namespace chopstix {

template <>
Edge Record::get(int *i) {
    long rowid = get<long>(i);
    long from_id = get<long>(i);
    long to_id = get<long>(i);
    auto edge = Edge(from_id, to_id);
    edge.rowid_ = rowid;
    return edge;
}

template <>
Query &Query::bind(int *i, Edge edge) {
    bind(i, edge.from_id_);
    bind(i, edge.to_id_);
    return *this;
}

}  // namespace chopstix

using namespace chopstix;

Edge::Edge(block_ptr from, block_ptr to)
    : from_(from), to_(to), from_id_(from->rowid()), to_id_(to->rowid()) {}

const Edge::block_ptr &Edge::find_from(block_vec &blocks) {
    if (!from_) {
        auto it = std::find_if(
            blocks.begin(), blocks.end(),
            [&](const block_ptr &block) { return block->rowid() == from_id_; });
        if (it != blocks.end()) from_ = *it;
    }
    return from_;
}

const Edge::block_ptr &Edge::find_to(block_vec &blocks) {
    if (!to_) {
        auto it = std::find_if(
            blocks.begin(), blocks.end(),
            [&](const block_ptr &block) { return block->rowid() == to_id_; });
        if (it != blocks.end()) to_ = *it;
    }
    return to_;
}

void Edge::set_from(block_ptr from) {
    from_ = from;
    from_id_ = from->rowid();
}

void Edge::set_to(block_ptr to) {
    to_ = to;
    to_id_ = to->rowid();
}

bool Edge::is_backedge() const {
    if (!from_ || !to_) return false;
    return to_->addr() <= from_->addr();
}

std::string Edge::repr() const {
    if (from_ && to_) {
        return fmt::format("<Edge from @{:x} to @{:x}>", from_->addr(),
                           to_->addr());
    } else {
        return fmt::format("<Edge from #{} to #{}>", from_id_, to_id_);
    }
}

Query Edge::find_by_from(Connection &db, long from_id) {
    auto q = db.query(SQL_SELECT_EDGE "WHERE from_id = ?;");
    q.bind(1, from_id);
    return q;
}

Query Edge::find_by_to(Connection &db, long to_id) {
    auto q = db.query(SQL_SELECT_EDGE "WHERE to_id = ?;");
    q.bind(1, to_id);
    return q;
}

void Edge::link_blocks(Connection &db, block_vec &blocks) {
    for (auto block : blocks) {
        auto q = Edge::find_by_from(db, block->rowid());
        while (q.next()) {
            auto edge = q.record().get<Edge>();
            edge.set_from(block);
            edge.find_to(blocks);
            if (edge.to()) block->link_next(edge.to());
        }
    }
}

bool Edge::operator==(const Edge &other) const {
    return from_ == other.from_ && to_ == other.to_ &&
           from_id_ == other.from_id_ && to_id_ == other.to_id_;
}
