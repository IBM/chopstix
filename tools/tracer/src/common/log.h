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

#include <stdarg.h>
#include <string>
#include <unistd.h>

#include "format.h"

namespace cxtrace {
namespace log {
enum mode {
    ERROR = 0,
    WARN = 1,
    INFO = 2,
    VERBOSE = 3,
    DEBUG = 4,
    MODES_MAX = 5,
};

extern const char *prefix[MODES_MAX];

struct Logger {
  public:
    static Logger &instance() {
        static Logger inst;
        return inst;
    }

    template <typename... Args>
    void println(mode m, const char *fmt, Args... args) {
        static char linebuf[1024];
        size_t n;
        if (m > mode_) return;
        n = fmt::format(linebuf, sizeof(linebuf), "%s: ", prefix[m]);
        ssize_t w = write(fd_, linebuf, n);
        assert(w == n && "Write errror");
        n = fmt::format(linebuf, sizeof(linebuf), fmt, args...);
        linebuf[n++] = '\n';
        w = write(fd_, linebuf, n);
        assert(w == n && "Write errror");
        // sync();
    }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

  private:
    Logger();
    ~Logger();

    int fd_ = 2;
    mode mode_ = INFO;
};

template <typename... Args>
void println(mode m, const char *fmt, Args... args) {
    Logger::instance().println(m, fmt, args...);
}

template <typename... Args> void debug(const char *fmt, Args... args) {
    println(DEBUG, fmt, args...);
}
template <typename... Args> void verbose(const char *fmt, Args... args) {
    println(VERBOSE, fmt, args...);
}
template <typename... Args> void info(const char *fmt, Args... args) {
    println(INFO, fmt, args...);
}
template <typename... Args> void warn(const char *fmt, Args... args) {
    println(WARN, fmt, args...);
}
template <typename... Args> void error(const char *fmt, Args... args) {
    println(ERROR, fmt, args...);
}
}
}
