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
 * NAME        : support/cache.h
 * DESCRIPTION : Cache values inside a std::map.
 *               The producer function is only called once for each key/value.
 ******************************************************************************/

#pragma once

#include <functional>
#include <map>
#include <utility>

namespace chopstix {

template <class Key, class T>
class Cache : public std::map<Key, T> {
  public:
    using producer = std::function<T(const Key &)>;

    Cache(producer produce) : produce_(produce) {}

    T &cache(const Key &key) {
        auto it = this->find(key);
        if (it != this->end()) {
            return it->second;
        } else {
            auto ret = this->insert(std::make_pair(key, produce_(key)));
            return ret.first->second;
        }
    }

  private:
    producer produce_;
};

}  // namespace chopstix
