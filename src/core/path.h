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
 * NAME        : core/path.h
 * DESCRIPTION : Path along basic blocks, i.e. snippet
 ******************************************************************************/

#pragma once

#include "traits.h"

#include "basicblock.h"

#include "database/connection.h"

#include "support/check.h"
#include "support/ringbuffer.h"

#include <memory>
#include <string>
#include <vector>

namespace chopstix {

class Path : public trait_rowid, public trait_score {
    friend class Record;

  public:
    using node_type = std::shared_ptr<BasicBlock>;
    using node_vec = std::vector<node_type>;
    using unique_ptr = std::unique_ptr<Path>;

    using iterator = node_vec::iterator;
    using const_iterator = node_vec::const_iterator;
    using reference = node_vec::reference;
    using const_reference = node_vec::const_reference;

    Path(const Path &other) = default;
    Path(node_type target = nullptr) : target_(target) {}
    Path(node_type target, node_type source) : target_(target) { add(source); }

    const node_vec &nodes() const { return nodes_; }
    const node_type &target() const { return target_; }

    reference front() { return nodes_.front(); }
    reference back() { return nodes_.back(); }
    const_reference front() const { return nodes_.front(); }
    const_reference back() const { return nodes_.back(); }
    iterator begin() { return nodes_.begin(); }
    iterator end() { return nodes_.end(); }
    const_iterator begin() const { return nodes_.begin(); }
    const_iterator end() const { return nodes_.end(); }
    bool empty() const { return nodes_.empty(); }
    size_t size() const { return nodes_.size(); }

    size_t num_ins() const { return num_ins_; }

    void add(node_type node);
    bool contains(const node_type &node) const;
    double trend(size_t nins = 60);  // TODO mark as const

    bool operator<(const Path &other) const { return score() < other.score(); }
    bool operator>(const Path &other) const { return score() > other.score(); }

    bool operator==(const Path &other) const;
    bool operator!=(const Path &other) const { return !(*this == other); }

    std::string repr() const;

    static unique_ptr find_by_rowid(Connection &db, long rowid);

    static Query list_by_score(Connection &db, double cutoff, long limit);
    static Query list_by_hash(Connection &db, long hash);

    bool find_by_hash(Connection &db);
    void save_db(Connection &db);
    void load_db(Connection &db);

    void update_score(double score);

    long hash() const { return hash_; }

  private:
    constexpr static size_t history_size_ = 20;
    node_type target_;
    node_vec nodes_;
    size_t num_ins_ = 0;
    long hash_ = 0;
    RingBuffer<double> d_score_ = RingBuffer<double>(history_size_);
    RingBuffer<size_t> d_ins_ = RingBuffer<size_t>(history_size_);
};

}  // namespace chopstix
