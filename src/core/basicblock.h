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
 * NAME        : core/basicblock.h
 * DESCRIPTION : A basic block is an uninterupted sequence of instructions.
 ******************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "edge.h"
#include "instruction.h"
#include "range.h"
#include "traits.h"
#include "types.h"

#include "database/connection.h"

namespace chopstix {

class Function;

class BasicBlock : public std::enable_shared_from_this<BasicBlock>,
                   public trait_rowid,
                   public trait_count,
                   public trait_score {
    friend class Record;

  public:
    using shared_ptr = std::shared_ptr<BasicBlock>;
    using weak_ptr = std::weak_ptr<BasicBlock>;

    using inst_vec = std::vector<Instruction>;
    using iterator = inst_vec::iterator;
    using const_iterator = inst_vec::const_iterator;
    using reference = inst_vec::reference;
    using const_reference = inst_vec::const_reference;

    using func_ptr = std::weak_ptr<Function>;

    using link_vec = std::vector<weak_ptr>;
    using edge_vec = std::vector<Edge>;

    template <typename... Args>
    static shared_ptr create(Args &&... args) {
        return std::make_shared<BasicBlock>(args...);
    }

    BasicBlock(Range range) : range_(range) {}
    BasicBlock(iterator begin, iterator end);

    void link_next(shared_ptr next);

    reference front() { return insts_.front(); }
    reference back() { return insts_.back(); }
    const_reference front() const { return insts_.front(); }
    const_reference back() const { return insts_.back(); }
    iterator begin() { return insts_.begin(); }
    iterator end() { return insts_.end(); }
    const_iterator begin() const { return insts_.begin(); }
    const_iterator end() const { return insts_.end(); }
    size_t size() const { return insts_.size(); }
    bool empty() const { return insts_.empty(); }

    addr_type addr() const { return range_.begin; }
    Range range() const { return range_; }
    const inst_vec &insts() const { return insts_; }
    func_ptr parent() { return parent_; }
    void set_parent(func_ptr ptr) { parent_ = ptr; }
    const link_vec &next() const { return next_; }
    const link_vec &prev() const { return prev_; }
    long func_id();
    long module_id();

    std::string repr() const;

    static shared_ptr find_by_rowid(Connection &db, long rowid);

    static Query list_by_score(Connection &db, double cutoff, long limit);
    static Query list_by_func(Connection &db, long func_id);

    void save_db(Connection &db);
    void load_db(Connection &db);

    edge_vec get_edges();

  private:
    Range range_;
    inst_vec insts_;
    func_ptr parent_;
    link_vec next_;
    link_vec prev_;
    long func_id_ = 0;
    long module_id_ = 0;
};

}  // namespace chopstix
