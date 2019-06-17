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
 * NAME        : support/progress.h
 * DESCRIPTION : Utility class to track and format progress
 ******************************************************************************/

#pragma once

#include <iomanip>
#include <istream>
#include <limits>

namespace chopstix {
class Progress {
  public:
    Progress(double max = 1.0) : max_(max), count_(0) {}

    static Progress infty();

    Progress &next(double steps = 1) {
        count_ = std::min(count_ + steps, max_);
        return *this;
    }

    Progress &reset() {
        count_ = 0;
        return *this;
    }

    Progress &finish() {
        count_ = max_;
        return *this;
    }

    double count() const { return count_; }
    double max() const { return max_; }
    void set_max(double max) { max_ = max; }
    bool complete() const { return count_ >= max_; }
    double value() const;

    Progress &operator++() { return next(); }
    Progress operator++(int) {
        Progress tmp = *this;
        next();
        return tmp;
    }

    Progress &operator+(double steps) {
        count_ += steps;
        return *this;
    }

    Progress &operator+=(double steps) {
        count_ += steps;
        return *this;
    }

    bool operator<(const Progress &other) const {
        return value() < other.value();
    }

    bool operator>(const Progress &other) const {
        return value() > other.value();
    }

    bool operator<(double val) const { return value() < val; }
    bool operator>(double val) const { return value() > val; }

    explicit operator double() const { return value(); }
    explicit operator bool() const { return complete(); }

  private:
    double max_;
    double count_;
};

std::ostream &operator<<(std::ostream &os, const Progress &prog);

}  // namespace chopstix
