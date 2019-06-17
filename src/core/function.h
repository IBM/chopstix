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
 * NAME        : core/function.h
 * DESCRIPTION : A function is a collection of basic blocks.
 ******************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "basicblock.h"
#include "edge.h"

#include "database/connection.h"
#include "database/query.h"

#include "traits.h"

namespace chopstix {

class Module;

class Function : public std::enable_shared_from_this<Function>,
                 public trait_rowid,
                 public trait_count,
                 public trait_score {
    friend class Record;

  public:
    using shared_ptr = std::shared_ptr<Function>;
    using weak_ptr = std::weak_ptr<Function>;

    using module_ptr = std::weak_ptr<Module>;
    using block_ptr = std::shared_ptr<BasicBlock>;

    using block_vec = std::vector<block_ptr>;
    using inst_vec = std::vector<Instruction>;
    using edge_vec = std::vector<Edge>;

    using iterator = block_vec::iterator;
    using const_iterator = block_vec::const_iterator;
    using reference = block_vec::reference;
    using const_reference = block_vec::const_reference;

    template <typename... Args>
    static shared_ptr create(Args &&... args) {
        return std::make_shared<Function>(args...);
    }

    Function(std::string name) : name_(name) {}
    void build_blocks(inst_vec &insts);
    void link_blocks();

    static std::string parse_header(std::istream &is);

    block_ptr find_block(addr_type addr);

    reference front() { return blocks_.front(); }
    reference back() { return blocks_.back(); }
    const_reference front() const { return blocks_.front(); }
    const_reference back() const { return blocks_.back(); }
    iterator begin() { return blocks_.begin(); }
    iterator end() { return blocks_.end(); }
    const_iterator begin() const { return blocks_.begin(); }
    const_iterator end() const { return blocks_.end(); }
    size_t size() const { return blocks_.size(); }
    bool empty() const { return blocks_.empty(); }

    const std::string name() const { return name_; }
    addr_type addr() const { return range_.begin; }
    Range range() const { return range_; }
    const block_vec &blocks() const { return blocks_; }
    module_ptr parent() { return parent_; }
    void set_parent(module_ptr mod) { parent_ = mod; }
    long module_id();
    long num_ins();

    std::string repr() const;

    static shared_ptr find_by_name(Connection &db, std::string name);
    static shared_ptr find_by_rowid(Connection &db, long rowid);

    static Query list_by_score(Connection &db, double cutoff, long limit,
                               long size);
    static Query list_by_module(Connection &db, long module_id);
    static long compute_size(Connection &db, long id);

    void save_db(Connection &db);
    void load_db(Connection &db);

    edge_vec get_edges();
    edge_vec get_backedges();
    Edge make_loop() { return Edge(back(), front()); }

  private:
    std::string name_;
    Range range_;
    block_vec blocks_;
    module_ptr parent_;
    long module_id_ = 0;
    long num_ins_ = 0;
};
}  // namespace chopstix
