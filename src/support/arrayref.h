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
 * NAME        : support/arrayref.h
 * DESCRIPTION : Utility class to give a "view" into a std::vector.
 *               An ArrayRef does not own/free its referenced elements.
 ******************************************************************************/

#pragma once

// Language headers
#include <vector>

namespace chopstix {

// Subset of std::vector
template <typename T>
class ArrayRef {
  public:
    typedef typename std::vector<T> vector;
    typedef typename vector::iterator iterator;
    typedef typename vector::reference reference;
    typedef typename vector::const_iterator const_iterator;
    typedef typename vector::const_reference const_reference;
    typedef typename vector::size_type size_t;

    ArrayRef(iterator beg, iterator end) : beg_(beg), end_(end) {}
    ArrayRef(vector vec) : beg_(vec.begin()), end_(vec.end()) {}

    std::size_t size() const { return end_ - beg_; }
    bool empty() const { return size() == 0; }
    void clear() { beg_ = end_; }
    iterator begin() { return beg_; }
    iterator end() { return end_; }
    reference front() { return *beg_; }
    reference back() { return *(end_ - 1); }
    reference operator[](size_t i) { return *(beg_ + i); }
    reference at(size_t i) { return *(beg_ + i); }
    const_iterator begin() const { return beg_; }
    const_iterator end() const { return end_; }
    const_reference front() const { return *beg_; }
    const_reference back() const { return *(end_ - 1); }
    const_reference operator[](size_t i) const { return *(beg_ + i); }
    const_reference at(size_t i) const { return *(beg_ + i); }

    bool operator==(const ArrayRef &a) const {
        return beg_ == a.beg_ && end_ == a.end_;
    }
    bool operator!=(const ArrayRef &a) const { return !(*this == a); }

    vector vec() const { return vector(beg_, end_); }

  private:
    iterator beg_;
    iterator end_;
};

}  // namespace chopstix
