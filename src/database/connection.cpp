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
 * NAME        : database/connection.cpp
 * DESCRIPTION : Wrapper around connection to SQL database
 ******************************************************************************/

#include "sql/common_pragmas.h"

#include "support/check.h"
#include "support/filesystem.h"
#include "support/options.h"

#include "connection.h"

#include <algorithm>

#include "fmt/format.h"

using namespace chopstix;

namespace fs = filesystem;

Connection::Connection(const std::string &filename) { open(filename); }
Connection::~Connection() { close(); }

Connection::Connection(Connection &&con) : h_(con.h_) { con.h_ = nullptr; }

Connection &Connection::operator=(Connection &&con) {
    if (&con != this) {
        h_ = con.h_;
        con.h_ = nullptr;
    }
    return *this;
}

void Connection::open(const std::string &filename) {
    if (is_open()) {
        close();
    }
    int ret = sqlite3_open(filename.c_str(), &h_);
    checkx(ret == SQLITE_OK, "Unable to open db: %s", errmsg());
}

void Connection::close() {
    sqlite3_close(h_);
    h_ = nullptr;
}

Query Connection::query(const std::string &q) {
    checkx(is_open(), "Database not open");
    Query::handle_type h;
    int ret = sqlite3_prepare_v2(h_, q.c_str(), q.size() + 1, &h, nullptr);
    checkx(ret == SQLITE_OK, "Unable to execute query: %s\n%s", errmsg(), q);
    Query query(h);
    return query;
}

void Connection::exec(const std::string &q) {
    int ret = _exec(q, false);
    checkx(ret == SQLITE_OK, "Error executing query: %s\n", errmsg());
}

std::string Connection::exec_safe(const std::string &q) {
    int ret = _exec(q, true);
    return errmsg();
}

int Connection::_exec(const std::string &q, bool safe) {
    checkx(is_open(), " not open");
    return sqlite3_exec(h_, q.c_str(), nullptr, nullptr, nullptr);
}

void Connection::transact(const std::function<void(void)> &action) {
    exec("begin transaction;");
    action();
    exec("end transaction;");
}

std::string Connection::errmsg() {
    return sqlite3_errmsg(h_);
}

long Connection::last_rowid() { return sqlite3_last_insert_rowid(h_); }

std::vector<std::string> Connection::columns(const std::string &table) {
    auto txt = fmt::format("pragma table_info({});", table);
    auto qu = query(txt);

    std::vector<std::string> res;
    while (qu.next()) res.push_back(qu.record().get<std::string>(1));
    return res;
}

std::vector<std::string> Connection::tables() {
    auto qu = query(R"SQL(
        select name from sqlite_master where type = 'table';
    )SQL");

    std::vector<std::string> res;
    while (qu.next()) res.push_back(qu.record().get<std::string>());
    return res;
}

bool Connection::has_tables(const std::vector<std::string> &test) {
    auto tabs = tables();
    for (auto &t : test) {
        if (std::find(tabs.begin(), tabs.end(), t) == tabs.end()) {
            return false;
        }
    }
    return true;
}

long Connection::num_records(const std::string &table) {
    auto q = query(fmt::format("select count(*) from {};", table));
    checkx(q.next(), "Unable to count records: %s", errmsg());
    return q.record().get<long>();
}

Connection Connection::get_default(bool required) {
    auto data_file = Option::get("data").as_string("chop.db");
    if (required) {
        checkx(fs::exists(data_file), "Database does not exit (%s)", data_file);
    }
    Connection db(data_file);
    db.exec(SQL_COMMON_PRAGMAS);
    return db;
}

long Connection::max_rows() { return std::numeric_limits<long>::max(); }
