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
 * NAME        : core/edge.h
 * DESCRIPTION : A control flow edge (i.e. branch) between two basic blocks
 ******************************************************************************/

#pragma once

#include "database/connection.h"

#include <memory>
#include <string>
#include <vector>

#include "traits.h"

namespace chopstix {

class Function;
class BasicBlock;

class Edge : public trait_rowid {
    friend class Query;
    friend class Record;

  public:
    using block_ptr = std::shared_ptr<BasicBlock>;
    using block_vec = std::vector<block_ptr>;
    bool is_backedge() const;

    Edge(long from, long to)
        : from_(nullptr), to_(nullptr), from_id_(from), to_id_(to) {}
    Edge(block_ptr from, block_ptr to);

    const block_ptr &find_from(block_vec &blocks);
    const block_ptr &find_to(block_vec &blocks);
    void find_blocks(block_vec &blocks) {
        find_from(blocks);
        find_to(blocks);
    }

    const block_ptr &from() const { return from_; }
    const block_ptr &to() const { return to_; }

    bool operator==(const Edge &other) const;
    bool operator!=(const Edge &other) const { return !(*this == other); }

    void set_from(block_ptr from);
    void set_to(block_ptr from);

    std::string repr() const;

    static Query find_by_from(Connection &db, long from_id);
    static Query find_by_to(Connection &db, long to_id);

    static void link_blocks(Connection &db, block_vec &blocks);

  private:
    block_ptr from_ = nullptr;
    block_ptr to_ = nullptr;
    long from_id_ = 0;
    long to_id_ = 0;
};

}  // namespace chopstix
