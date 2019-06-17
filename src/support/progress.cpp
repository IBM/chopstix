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
 * NAME        : support/progress.cpp
 * DESCRIPTION : Utility class to track and format progress
 ******************************************************************************/

#include "progress.h"

#include <limits>

namespace chopstix {

double Progress::value() const { return complete() ? 1.0 : count_ / max_; }

Progress Progress::infty() {
    return Progress(std::numeric_limits<double>::infinity());
}

std::ostream &operator<<(std::ostream &os, const Progress &prog) {
    std::ios::fmtflags f(os.flags());
    double pct = prog.value() * 100;
    os << '[' << std::setfill(' ') << std::setw(3) << std::setprecision(0)
       << std::fixed << pct << "%]";
    os.flags(f);
    return os;
}

}  // namespace chopstix
