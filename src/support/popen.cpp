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
 * NAME        : support/popen.cpp
 * DESCRIPTION : Get a std::ostream from a call to popen.
 *               This is meant to read the result of trivial shell commands.
 ******************************************************************************/

#include "popen.h"

// Language headers
#include <cstdio>

// Public headears
#include "support/check.h"

namespace chopstix {

std::ostream &operator<<(std::ostream &out, const Popen &p) {
    char buf[p.buf_size_];
    FILE *in = popen(p.cmd_.c_str(), "r");
    check(in, "Cannot open pipe");
    while (fgets(buf, p.buf_size_, in) != nullptr) {
        out << buf;
    }
    pclose(in);
    return out;
}
}  // namespace chopstix
