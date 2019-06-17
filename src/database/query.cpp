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
 * NAME        : database/query.cpp
 * DESCRIPTION : Wrapper for SQL query, i.e. statement
 ******************************************************************************/

#include "support/check.h"
#include "support/log.h"

#include "query.h"

#include <cstring>

using namespace chopstix;

Query::~Query() { sqlite3_finalize(h_); }

Query::Query(Query &&qu) : h_(qu.h_) { qu.h_ = nullptr; }

Query &Query::operator=(Query &&qu) {
    if (&qu != this) {
        h_ = qu.h_;
        qu.h_ = nullptr;
    }
    return *this;
}

namespace chopstix {

template <>
Query &Query::bind(int *i, int v) {
    int ret = sqlite3_bind_int(h_, *i, v);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, long v) {
    int ret = sqlite3_bind_int64(h_, *i, v);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, unsigned int v) {
    int ret = sqlite3_bind_int(h_, *i, v);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, unsigned long v) {
    int ret = sqlite3_bind_int64(h_, *i, v);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}
template <>
Query &Query::bind(int *i, double v) {
    int ret = sqlite3_bind_double(h_, *i, v);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, std::string v) {
    int ret = sqlite3_bind_text(h_, *i, v.c_str(), v.size(), SQLITE_TRANSIENT);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, const std::string &v) {
    int ret = sqlite3_bind_text(h_, *i, v.c_str(), v.size(), SQLITE_TRANSIENT);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

template <>
Query &Query::bind(int *i, const char *v) {
    int ret = sqlite3_bind_text(h_, *i, v, strlen(v), SQLITE_TRANSIENT);
    checkx(ret == SQLITE_OK, "Unable to bind value: %s", errmsg());
    *i += 1;
    return *this;
}

}  // namespace chopstix

Query &Query::bind_null(int i) {
    int ret = sqlite3_bind_null(h_, i);
    checkx(ret == SQLITE_OK, "Unable to bind value %s", errmsg());
    return *this;
}

// void Query::bind_blob(int i, void *v, size_t s) {
//     int ret = sqlite3_bind_blob(h_, *i, v, s, SQLITE_TRANSIENT);
//     checkx(ret == SQLITE_OK, "Unable to bind value %s", errmsg());
// }

int Query::index(const std::string &k) {
    return sqlite3_bind_parameter_index(h_, k.c_str());
}

void Query::clear() { sqlite3_clear_bindings(h_); }
int Query::step() { return sqlite3_step(h_); }
int Query::reset() { return sqlite3_reset(h_); }

std::string Query::errmsg() { return sqlite3_errmsg(sqlite3_db_handle(h_)); }

bool Query::next() {
    while (1) {
        int ret = step();
        switch (ret) {
            case SQLITE_ROW: return true;
            case SQLITE_DONE: return false;
            case SQLITE_BUSY: check(0, "Database busy");
            case SQLITE_ERROR:
                log::error("SQL Error: %s", errmsg());
                fail("Database error");
            case SQLITE_MISUSE:
                log::error("SQL Error: %s", errmsg());
                fail("Database misuse");
        }
    }
}

void Query::finish() {
    while (next()) {
        /* void */
    }
    reset();
}

// TODO Support utf-X
