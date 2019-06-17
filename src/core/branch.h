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
 * NAME        : core/branch.h
 * DESCRIPTION : Generic branch implementation
 ******************************************************************************/

#pragma once

#include "types.h"

#include <string>

namespace chopstix {

// Branch information
struct Branch {
    enum Flags {
        LINK = 1 << 0,
        CONDITION = 1 << 1,
        REGISTER = 1 << 2,
        RELATIVE = 1 << 3
    };

    addr_type source;
    addr_type target;
    int flags;

    void set(int bits) { flags |= bits; }
    void unset(int bits) { flags ^= bits; }
    bool test(int bits) const { return flags & bits; }

    bool link() const { return test(LINK); }
    bool cond() const { return test(CONDITION); }
    bool reg() const { return test(REGISTER); }
    bool rel() const { return test(RELATIVE); }

    std::string repr() const;
};

}  // namespace chopstix
