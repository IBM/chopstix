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
 * NAME        : core/sample.h
 * DESCRIPTION :
 ******************************************************************************/

#pragma once

// Language headers
#include <string>
#include <vector>

namespace chopstix {

struct Sample {
    static constexpr size_t header_count = 3;
    static constexpr size_t header_size = sizeof(uint64_t) * header_count;

    typedef std::vector<uint64_t> value_list;

    uint64_t ip;
    uint32_t pid;
    uint32_t tid;
    uint64_t time;
    value_list data;

    std::string repr() const;
};

}  // namespace chopstix
