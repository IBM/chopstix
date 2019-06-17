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
 * NAME        : database/query.h
 * DESCRIPTION : Wrapper for SQL query, i.e. statement
 ******************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "record.h"

namespace chopstix {

class Query {
  public:
    typedef sqlite3_stmt *handle_type;
    using ptr = std::unique_ptr<Query>;

    Query(handle_type qu = nullptr) : h_(qu) {}
    ~Query();

    Query(const Query &) = delete;
    Query &operator=(const Query &) = delete;
    Query(Query &&);
    Query &operator=(Query &&);

    bool is_good() const { return h_ != nullptr; }

    template <typename T>
    Query &bind(int *i, T v);
    template <typename T>
    Query &bind(int i, T v) {
        return bind<T>(&i, v);
    }
    Query &bind_null(int i);
    // void bind_blob(int i, void *v, size_t s);
    // template<typename T>
    // void bind_blob(int i, T *v) { bind_blob(i, v, sizeof(T)); }

    template <typename T>
    Query &bind(const std::string &k, T v) {
        return bind(index(k), v);
    }
    Query &bind_null(const std::string &k) { return bind_null(index(k)); }
    // void bind_blob(const std::string &k, void *v, size_t s) {
    // bind_blob(index(k), v, s); }
    // template<typename T>
    // void bind_blob(const std::string &k, T *v) { bind_blob(k, v, sizeof(T));
    // }

    int index(const std::string &k);

    void clear();
    int step();
    int reset();
    void finish();

    bool next();
    Record record() { return Record(h_); }

    // // TODO Might not have undefined behavior
    // template <typename... Ts>
    // std::tuple<Ts...> tuple() {
    //     auto rec = record();
    //     int pos = 0;
    //     return std::tuple<Ts...>{next_val<Ts>(rec, pos++)...};
    // }
    //
    // template <typename... Ts>
    // std::vector<std::tuple<Ts...>> getall() {
    //     std::vector<std::tuple<Ts...>> res;
    //     while (next()) {
    //         res.push_back(tuple<Ts...>());
    //     }
    //     reset();
    //     return res;
    // }
    //
    // template <typename... Ts>
    // std::tuple<Ts...> get() {
    //     return getall<Ts...>()[0];
    // }

    std::string errmsg();

  private:
    handle_type h_ = nullptr;

    template <typename T>
    static T next_val(Record &rec, int pos) {
        return rec.get<T>(pos);
    }
};

}  // namespace chopstix
