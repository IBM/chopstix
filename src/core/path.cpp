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
 * NAME        : core/path.cpp
 * DESCRIPTION : Path along basic blocks, i.e. snippet
 ******************************************************************************/

#include "path.h"

#include "queries.h"

#include "support/log.h"

#include "fmt/format.h"

#include <algorithm>
#include <sstream>

namespace chopstix {

template <>
Query &Query::bind(int *i, Path path) {
    bind(i, path.hash());
    bind(i, path.score());
    return *this;
}

template <>
Path Record::get(int *i) {
    Path path;
    path.rowid_ = get<long>(i);
    path.hash_ = get<long>(i);
    path.score_ = get<double>(i);
    return path;
}

}  // namespace chopstix

using namespace chopstix;

void Path::add(node_type node) {
    nodes_.push_back(node);
    num_ins_ += node->size();
    hash_ ^= node->rowid();
    d_ins_.push_back(node->size());
}

bool Path::contains(const node_type &node) const {
    auto it = std::find_if(
        nodes_.begin(), nodes_.end(),
        [&](const node_type &test) { return test->rowid() == node->rowid(); });
    return it != nodes_.end();
}

void Path::update_score(double score) {
    d_score_.push_back(score - score_);
    score_ = score;
}

double Path::trend(size_t nins) {
    double t_score = 0;
    size_t t_ins = 0;
    for (auto d : d_score_) t_score += d;
    for (auto d : d_ins_) t_ins += d;
    if (t_ins < nins) return 1;
    return t_score / t_ins;
}

bool Path::operator==(const Path &other) const {
    if (hash() != other.hash()) return false;
    if (size() != other.size()) return false;
    for (auto &node : nodes_) {
        if (!other.contains(node)) return false;
    }
    return true;
}

std::string Path::repr() const {
    if (empty()) return "<Path (empty)>";

    auto front = nodes_.front();
    auto back = nodes_.back();

    // char sep = '(';
    // std::stringstream ss;
    // for (auto &node : nodes_) {
    //     fmt::print(ss, "{}{}", sep, node->rowid());
    //     sep = ',';
    // }
    // return fmt::format("<Path from [{}]{:x} to [{}]{:x} {})>",
    // front->rowid(),
    //                    front->addr(), back->rowid(), back->addr(), ss.str());

    return fmt::format("<Path from [{}]{:x} to [{}]{:x}>", front->rowid(),
                       front->addr(), back->rowid(), back->addr());
}

Path::unique_ptr Path::find_by_rowid(Connection &db, long rowid) {
    auto q = db.query(SQL_SELECT_PATH " WHERE rowid = ?;");
    q.bind(1, rowid);
    if (!q.next()) return nullptr;
    auto path = q.record().get<Path>();
    return unique_ptr(new Path(path));
}

Query Path::list_by_score(Connection &db, double cutoff, long limit) {
    auto q = db.query(SQL_SELECT_PATH SQL_LIST_BY_SCORE);
    q.bind(1, cutoff);
    q.bind(2, limit);
    return q;
}

Query Path::list_by_hash(Connection &db, long hash) {
    auto q = db.query(SQL_SELECT_PATH " WHERE hash = ?;");
    q.bind(1, hash);
    return q;
}

bool Path::find_by_hash(Connection &db) {
    log::verbose("Hashing %s", repr());
    auto q = Path::list_by_hash(db, hash());
    while (q.next()) {
        auto other = q.record().get<Path>();
        other.load_db(db);
        log::verbose("Compare %s", other.repr());
        if (*this == other) return true;
    }
    return false;
}

void Path::save_db(Connection &db) {
    log::verbose("Saving %s", repr());
    auto q1 = db.query(SQL_INSERT_PATH);
    q1.bind(1, *this);
    q1.finish();
    rowid_ = db.last_rowid();

    auto q2 = db.query(SQL_INSERT_PATH_NODE);
    q2.bind(1, rowid_);

    int rank = 1;

    db.transact([&]() {
        for (auto block : nodes_) {
            q2.bind(2, block->rowid());
            q2.bind(3, rank);
            q2.finish();
            rank += 1;
        }
    });
}

void Path::load_db(Connection &db) {
    checkx(has_rowid(), "Unable to load path without id");
    auto q =
        db.query(SQL_SELECT_PATH_NODE " WHERE path_id = ? ORDER BY rank ASC;");
    q.bind(1, rowid_);

    while (q.next()) {
        auto block = q.record().get<BasicBlock::shared_ptr>();
        nodes_.push_back(block);
    }

    for (auto block : nodes_) {
        block->load_db(db);
    }

    Edge::link_blocks(db, nodes_);
    log::verbose("Loaded %s", repr());
}
