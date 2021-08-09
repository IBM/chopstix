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

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "log.h"

namespace chopstix {

void backtrace();

template <typename... Args>
__attribute__((noreturn)) void fail(const char *fmt, Args... args) {
    log::error(fmt, args...);
    log::error("(errno: %d) %s", errno, strerror(errno));
    if (getenv("CHOPSTIX_DEBUG")) backtrace();
    exit(EXIT_FAILURE);
}

template <typename... Args>
__attribute__((noreturn)) void failx(const char *fmt, Args... args) {
    log::error(fmt, args...);
    if (getenv("CHOPSTIX_DEBUG")) backtrace();
    exit(EXIT_FAILURE);
}

#define check(cond, ...) if (!(cond)) _check(__VA_ARGS__)
template <typename... Args>
void _check(const char *fmt, Args... args) {
    log::error(fmt, args...);
    log::error("(errno: %d) %s", errno, strerror(errno));
    if (getenv("CHOPSTIX_DEBUG")) backtrace();
    exit(EXIT_FAILURE);
}

#define checkx(cond, ...) if (!(cond)) _checkx(__VA_ARGS__)
template <typename... Args>
void _checkx(const char *fmt, Args... args) {
    log::error(fmt, args...);
    if (getenv("CHOPSTIX_DEBUG")) backtrace();
    exit(EXIT_FAILURE);
}

}  // namespace chopstix
