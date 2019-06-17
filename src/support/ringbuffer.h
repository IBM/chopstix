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
#pragma once

#include <cassert>
#include <vector>

namespace chopstix {

template <typename T>
class RingBuffer {
  public:
    typedef typename std::vector<T> vector;
    typedef typename vector::iterator vec_iter;

    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;

    class iterator {
      public:
        typedef iterator self_type;
        typedef T value_type;
        typedef typename vec_iter::reference reference;
        typedef typename vec_iter::pointer pointer;
        typedef std::forward_iterator_tag iterator_category;

        iterator(vector& vec, size_t it) : vec_(vec), pos_(it) {}

        self_type operator++() {
            self_type it = *this;
            pos_ = RingBuffer::roll_inc(vec_, pos_);
            return it;
        }

        self_type operator++(int) {
            pos_ = RingBuffer::roll_inc(vec_, pos_);
            return *this;
        }

        reference operator*() { return vec_[pos_]; }
        pointer operator->() { return &vec_[pos_]; }
        bool operator==(const self_type& rhs) { return pos_ == rhs.pos_; }
        bool operator!=(const self_type& rhs) { return !(*this == rhs); }

      private:
        vector& vec_;
        size_t pos_;
    };

    size_t capacity() const { return data_.size() - 1; }
    size_t size() const {
        return (begin_ <= end_) ? end_ - begin_
                                : (data_.size() - begin_ + end_);
    }
    bool empty() const { return begin_ == end_; }
    bool full() const { return size() == capacity(); }

    void pop_front() {
        if (empty()) return;
        begin_ = roll_inc(data_, begin_);
    }

    void pop_back() {
        if (empty()) return;
        end_ = roll_dec(data_, end_);
    }

    void push_front(value_type value) {
        if (full()) pop_back();
        begin_ = roll_dec(data_, begin_);
        data_[begin_] = value;
    }

    void push_back(value_type value) {
        if (full()) pop_front();
        data_[end_] = value;
        end_ = roll_inc(data_, end_);
    }

    reference front() {
        assert(!empty());
        return data_[begin_];
    }

    reference back() {
        assert(!empty());
        return data_[roll_dec(data_, end_)];
    }

    iterator begin() { return iterator(data_, begin_); }
    iterator end() { return iterator(data_, end_); }

    RingBuffer(size_t capacity) : data_(capacity + 1), begin_(0), end_(0) {}

  private:
    static size_t roll_inc(const vector& vec, size_t i) {
        return (i + 1 == vec.size()) ? 0 : i + 1;
    }

    static size_t roll_dec(const vector& vec, size_t i) {
        return (i == 0) ? vec.size() - 1 : i - 1;
    }

    vector data_;
    size_t begin_;
    size_t end_;
};

}  // namespace chopstix