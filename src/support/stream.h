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
 * NAME        : support/stream.h
 * DESCRIPTION : Stream utilities, mostly for parsing
 ******************************************************************************/

#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace chopstix {
namespace stream {

typedef std::function<std::ostream &(std::ostream &)> omanip;
typedef std::function<std::istream &(std::istream &)> imanip;

std::istream &skipline(std::istream &, char = '\n');

template <typename T>
imanip expect(const T &e) {
    return [&](std::istream &is) -> std::istream & {
        T r;
        is >> r;
        if (r != e) {
            is.setstate(std::ios::failbit);
        }
        return is;
    };
}

template <typename T>
std::istream &skip(std::istream &is) {
    T t;
    return is >> t;
}

template <typename T>
imanip push_back(std::vector<T> &vec) {
    return [&](std::istream &is) -> std::istream & {
        vec.resize(vec.size() + 1);
        auto &el = vec.back();
        is >> el;
        if (is.fail()) {
            vec.pop_back();
        }
        return is;
    };
}

template <typename T>
omanip unwrap(const std::vector<T> &vec, const std::string &sep = ",") {
    return [&](std::ostream &os) -> std::ostream & {
        auto it = vec.begin();
        auto end = vec.end() - 1;
        for (; it < end; ++it) {
            os << *it << sep;
        }
        return os << vec.back();
    };
}
}  // namespace stream

inline std::ostream &operator<<(std::ostream &os, stream::omanip manip) {
    return manip(os);
}
inline std::istream &operator>>(std::istream &is, stream::imanip manip) {
    return manip(is);
}

}  // namespace chopstix
