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
#include "stringutils.h"

using str_vector = std::vector<std::string>;
using str_pair = std::pair<std::string, std::string>;

str_pair cxtrace::ssplit(const std::string &str, const std::string &delim) {
    auto start = str.find_first_of(delim, 0);
    auto end = str.find_first_not_of(delim, start);
    return make_pair(str.substr(0, start),
                     end == std::string::npos ? "" : str.substr(end));
}

str_vector cxtrace::ssplitg(const std::string &str, const std::string &delim) {
    str_vector vec;
    auto start = str.find_first_not_of(delim, 0);
    auto end = str.find_first_of(delim, start);
    while (start < str.size()) {
        vec.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delim, end);
        end = str.find_first_of(delim, start);
    }
    return std::move(vec);
}

long long cxtrace::stoaddr(const std::string &str) {
    return std::stoll(str, nullptr, 16);
}
