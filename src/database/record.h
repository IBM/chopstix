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
 * NAME        : database/record.h
 * DESCRIPTION : Access query results
 ******************************************************************************/

#pragma once

#include <string>

#include <sqlite3.h>

namespace chopstix {

class Record {
  public:
    typedef sqlite3_stmt *handle_type;

    Record(handle_type h) : h_(h) {}
    Record(const Record &) = default;
    Record &operator=(const Record &) = default;
    Record(Record &&) = default;
    Record &operator=(Record &&) = default;

    template <typename T>
    T get(int *i);

    template <typename T>
    T get(int i = 0) {
        return get<T>(&i);
    }

    template <typename T>
    T get(const std::string &k) {
        return get<T>(index(k));
    }

    int index(const std::string &k);
    std::string name(int i);
    int size();

  private:
    handle_type h_;
};

}  // namespace chopstix
