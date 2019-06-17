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
 * NAME        : core/range.cpp
 * DESCRIPTION : Address range [begin,end] (both inclusive)
 ******************************************************************************/

#include "range.h"

#include "fmt/format.h"

#include "database/query.h"

namespace chopstix {

template <>
Range Record::get(int *i) {
    return Range{get<long>(i), get<long>(i)};
}

template <>
Query &Query::bind(int *i, Range range) {
    bind(i, range.begin);
    bind(i, range.end);
    return *this;
}

}  // namespace chopstix

using namespace chopstix;

std::string Range::repr() const {
    return fmt::format("[{:x},{:x}]", begin, end);
}

bool Range::contains(long addr) const { return addr >= begin && addr <= end; }

bool Range::is_subset(const Range &rg) const { return rg.is_superset(*this); }

bool Range::is_superset(const Range &rg) const {
    return contains(rg.begin) && contains(rg.end);
}

bool Range::intersects(const Range &rg) const {
    return contains(rg.begin) || contains(rg.end);
}

addr_type Range::shift(const Range &from, const Range &to, addr_type addr) {
    if (to.is_superset(from) || from.is_superset(to)) {
        return addr;
    }
    return addr + to.begin - from.begin;
}

Range Range::shift(const Range &from, const Range &to, Range rg) {
    rg.begin = shift(from, to, rg.begin);
    rg.end = shift(from, to, rg.end);
    return rg;
}
