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
 * NAME        : support/string.cpp
 * DESCRIPTION : Utility functions for string objects
 ******************************************************************************/

#include "string.h"

#include <cxxabi.h>
#include <algorithm>
#include <numeric>
#include <sstream>

using namespace chopstix;

std::vector<std::string> string::splitg(const std::string &str,
                                        const std::string &delim) {
    std::vector<std::string> vec;

    auto start = str.find_first_not_of(delim, 0);
    auto end = str.find_first_of(delim, start);

    while (start < str.size()) {
        vec.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delim, end);
        end = str.find_first_of(delim, start);
    }

    return vec;
}

std::pair<std::string, std::string> string::split(const std::string &str,
                                                  const std::string &delim) {
    auto start = str.find_first_of(delim, 0);
    auto end = str.find_first_not_of(delim, start);
    return make_pair(str.substr(0, start),
                     end == std::string::npos ? "" : str.substr(end));
}

void string::tolower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void string::toupper(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void string::ltrim(std::string &str, const std::string &ws) {
    str.erase(0, str.find_first_not_of(ws));
}

void string::rtrim(std::string &str, const std::string &ws) {
    str.erase(str.find_last_not_of(ws) + 1);
}

void string::trim(std::string &str, const std::string &ws) {
    string::rtrim(str, ws);
    string::ltrim(str, ws);
}

std::string string::join(const std::vector<std::string> &vec,
                         const std::string &sep) {
    if (vec.empty()) return "";

    std::stringstream ss;
    for (size_t i = 0; i < vec.size() - 1; ++i) {
        ss << vec[i] << sep;
    }
    ss << vec.back();

    return ss.str();
}

int string::distance(const std::string &s1, const std::string &s2) {
    int s1len = s1.size();
    int s2len = s2.size();

    auto column_start = (decltype(s1len))1;

    auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start, column + s1len + 1, column_start);

    for (auto x = column_start; x <= s2len; ++x) {
        column[0] = x;
        auto last_diagonal = x - column_start;
        for (auto y = column_start; y <= s1len; ++y) {
            auto old_diagonal = column[y];
            auto possibilities = {
                column[y] + 1, column[y - 1] + 1,
                last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)};
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}

std::string string::demangle(const std::string &s) {
    int status;
    char *realname = abi::__cxa_demangle(s.data(), nullptr, nullptr, &status);
    if (status != 0) return s;
    std::string ret = realname;
    free(realname);
    return ret;
}