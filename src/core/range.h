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
 * NAME        : core/range.h
 * DESCRIPTION : Address range [begin,end] (both inclusive)
 ******************************************************************************/

#pragma once

#include "types.h"

#include <string>

namespace chopstix {

struct Range {
    addr_type begin;
    addr_type end;

    Range(addr_type begin = 0, addr_type end = 0) : begin(begin), end(end) {}

    bool contains(long addr) const;
    bool is_subset(const Range &rg) const;
    bool is_superset(const Range &rg) const;
    bool intersects(const Range &rg) const;

    std::string repr() const;

    bool operator==(const Range &other) const;
    bool operator!=(const Range &other) const;

    bool operator<(const Range &other) const { return is_superset(other); }

    static addr_type shift(const Range &from, const Range &to, addr_type addr);
    static Range shift(const Range &from, const Range &to, Range rg);
};

}  // namespace chopstix
