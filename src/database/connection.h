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
 * NAME        : database/connection.h
 * DESCRIPTION : Wrapper around connection to SQL database
 ******************************************************************************/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "query.h"

namespace chopstix {

class Connection {
  public:
    typedef sqlite3 *handle_type;

    Connection() {}
    Connection(const std::string &filename);
    ~Connection();

    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;
    Connection(Connection &&);
    Connection &operator=(Connection &&);

    bool is_open() const { return h_ != nullptr; }
    void open(const std::string &filename);
    void close();

    Query query(const std::string &q);
    void exec(const std::string &q);
    std::string exec_safe(const std::string &);
    void transact(const std::function<void(void)> &action);

    handle_type handle() { return h_; }

    std::string errmsg();

    long last_rowid();

    std::vector<std::string> columns(const std::string &table);
    std::vector<std::string> tables();
    bool has_tables(const std::vector<std::string> &names);
    long num_records(const std::string &table);

    static Connection get_default(bool required = false);

    static long max_rows();

  private:
    handle_type h_ = nullptr;
    int _exec(const std::string &q, bool safe = false);
};

}  // namespace chopstix
