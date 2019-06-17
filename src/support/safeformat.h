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

#include <assert.h>
#include <stdlib.h>
#include <string>

#include "safestring.h"

namespace chopstix {
namespace sfmt {
namespace impl {

// write_arg(...) returns characters written
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 char arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 unsigned char arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 int arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 unsigned int arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 long arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 unsigned long arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 long long arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 unsigned long long arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 const char *arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 const std::string &arg);
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 void *arg);
// TODO Implement floating point formatting
// size_t write_arg(char *str, size_t size, const char *fmt_str, size_t
// fmt_size, float arg);
// size_t write_arg(char *str, size_t size, const char *fmt_str, size_t
// fmt_size, double arg);

// returns next fmt character
int next_fmt(const char *fmt, char *&fmt_str, size_t &fmt_size);

template <typename T>
size_t write_step(char *str, size_t size, const char *&fmt, T arg) {
    // Find next format string %...
    char *fmt_str;
    size_t fmt_size;
    char fmt_char = next_fmt(fmt, fmt_str, fmt_size);
    assert(fmt_char && "Trailing format arguments");
    // Copy part before %...
    size_t len = fmt_str - fmt;
    assert(len < size && "Buffer overflow");
    safe_strncpy(str, fmt, len);
    fmt += len + fmt_size;
    str += len;
    size -= len;
    if (fmt_char == '%') {
        // Write raw %
        assert(1 < size && "Buffer overflow");
        safe_strncpy(str, "%", 1);
        return len + 1 + write_step(str, size, fmt, arg);
    } else {
        // Write/format argument
        return len + write_arg(str, size, fmt_str, fmt_size, arg);
    }
}

size_t write_fmt(char *str, size_t size, const char *&fmt);

template <typename T, typename... Args>
size_t write_fmt(char *str, size_t size, const char *&fmt, T arg,
                 Args... rest) {
    size_t len = impl::write_step(str, size, fmt, arg);
    return len + impl::write_fmt(str + len, size - len, fmt, rest...);
}
}  // namespace impl

// safe snprintf implementation
template <typename... Args>
size_t format(char *str, size_t size, const char *fmt, Args... args) {
    size_t len = impl::write_fmt(str, size, fmt, args...);
    str[len] = '\0';
    return len;
}
}  // namespace sfmt
}  // namespace chopstix
