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
#include "log.h"

#include "check.h"
#include "format.h"
#include "misc.h"
#include "param.h"

#include <ctype.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "filesystem.h"

using namespace cxtrace;

namespace fs = filesystem;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

using namespace cxtrace;

const char *log::prefix[log::MODES_MAX] = {
    "error", "warn", "info", "verbose", "debug",
};

log::Logger::Logger() {
    auto loglevel = getparam("loglevel", prefix[mode_]);
    if (strcasecmp(loglevel, "error") == 0) {
        mode_ = log::ERROR;
    } else if (strcasecmp(loglevel, "warn") == 0) {
        mode_ = log::WARN;
    } else if (strcasecmp(loglevel, "info") == 0) {
        mode_ = log::INFO;
    } else if (strcasecmp(loglevel, "verbose") == 0) {
        mode_ = log::VERBOSE;
    } else if (strcasecmp(loglevel, "debug") == 0) {
        mode_ = log::DEBUG;
    } else {
        checkx(0, "Unknown log level '%s'", loglevel);
    }
    auto logfile = getparam("logfile");
    if (logfile) {
        char fname[PATH_MAX];
        snprintf(fname, sizeof(fname), "%s.%d", logfile, getpid());
        fs::mkdir(fs::dirname(logfile));
        fd_ = creat(fname, PERM_664);
        check(fd_ != -1, "Unable to open log '%s'", logfile);
    }
}

log::Logger::~Logger() {
    if (fd_ > 2) {
        close(fd_);
        fd_ = -1;
    }
}
