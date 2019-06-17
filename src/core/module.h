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
 * NAME        : core/module.h
 * DESCRIPTION : A module is a binary or object file
 ******************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "function.h"
#include "types.h"

#include "arch.h"

#include "traits.h"

#include "database/connection.h"

namespace chopstix {
class Module : public std::enable_shared_from_this<Module>,
               public trait_rowid,
               public trait_count,
               public trait_score {
    friend class Record;

  public:
    using shared_ptr = std::shared_ptr<Module>;
    using weak_ptr = std::weak_ptr<Module>;

    using func_ptr = std::shared_ptr<Function>;
    using func_vec = std::vector<func_ptr>;
    using block_ptr = Function::block_ptr;

    using iterator = func_vec::iterator;
    using const_iterator = func_vec::const_iterator;
    using reference = func_vec::reference;
    using const_reference = func_vec::const_reference;

    template <typename... Args>
    static shared_ptr create(Args &&... args) {
        return std::make_shared<Module>(args...);
    }

    Module(std::string name, std::string arch = Arch::get_machine())
        : name_(name), arch_(arch) {}
    void build_cfg();

    void load_obj();
    void load_asm(const std::string &filename);
    void save_asm(const std::string &filename);

    block_ptr find_block(addr_type addr);
    func_ptr find_func(addr_type addr);
    func_ptr find_func(const std::string &name);

    reference front() { return funcs_.front(); }
    reference back() { return funcs_.back(); }
    const_reference front() const { return funcs_.front(); }
    const_reference back() const { return funcs_.back(); }
    iterator begin() { return funcs_.begin(); }
    iterator end() { return funcs_.end(); }
    const_iterator begin() const { return funcs_.begin(); }
    const_iterator end() const { return funcs_.end(); }
    size_t size() const { return funcs_.size(); }
    bool empty() const { return funcs_.empty(); }

    const std::string &name() const { return name_; }
    const std::string &arch() const { return arch_; }
    addr_type addr() const { return range_.begin; }
    Range range() const { return range_; }
    const func_vec &funcs() const { return funcs_; }

    std::string repr() const;

    static shared_ptr find_by_name(Connection &db, std::string name);
    static shared_ptr find_by_value(Connection &db, std::string name,
                                    std::string arch);
    static shared_ptr find_by_rowid(Connection &db, long rowid);
    static Query list_by_score(Connection &db, double cutoff, long limit);

    void save_db(Connection &db);
    void load_db(Connection &db);

  private:
    std::string name_;
    std::string arch_;
    Range range_;
    func_vec funcs_;

    std::istream &parse_stream(std::istream &);
};
}  // namespace chopstix
