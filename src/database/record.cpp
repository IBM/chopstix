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
 * NAME        : database/record.cpp
 * DESCRIPTION : Access query results
 ******************************************************************************/

#include "record.h"

using namespace chopstix;

namespace chopstix {

template <>
std::string Record::get(int* i) {
    auto ret = (const char*)sqlite3_column_text(h_, *i);
    *i += 1;
    return ret;
}

template <>
int Record::get(int* i) {
    auto ret = sqlite3_column_int(h_, *i);
    *i += 1;
    return ret;
}

template <>
long Record::get(int* i) {
    auto ret = sqlite3_column_int64(h_, *i);
    *i += 1;
    return ret;
}

template <>
double Record::get(int* i) {
    auto ret = sqlite3_column_double(h_, *i);
    *i += 1;
    return ret;
}

}  // namespace chopstix

std::string Record::name(int i) {
    return (const char*)sqlite3_column_name(h_, i);
}

int Record::index(const std::string& k) {
    for (int i = 0; i < size(); ++i) {
        if (name(i) == k) {
            return i;
        }
    }
    return -1;
}

int Record::size() { return sqlite3_column_count(h_); }
