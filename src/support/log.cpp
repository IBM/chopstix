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
#include "options.h"
#include "safeformat.h"

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

using namespace chopstix;

namespace fs = filesystem;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

using namespace chopstix;

const char *log::prefix[log::MODES_MAX] = {
    "error", "warn", "info", "verbose", "debug",
};

log::Logger::Logger() {
    std::string loglevel = getopt("log-level").as_string(prefix[mode_]);
    if (loglevel == "error") {
        mode_ = log::ERROR;
    } else if (loglevel == "warn") {
        mode_ = log::WARN;
    } else if (loglevel == "info") {
        mode_ = log::INFO;
    } else if (loglevel == "verbose") {
        mode_ = log::VERBOSE;
    } else if (loglevel == "debug") {
        mode_ = log::DEBUG;
    } else {
        checkx(0, "Unknown log level '%s'", loglevel);
    }
    auto logfile = getopt("log-path");
    if (logfile.is_set()) {
        char fname[PATH_MAX];
        sfmt::format(fname, sizeof(fname), "%s", logfile.as_string());
        fs::mkdir(fs::dirname(logfile.as_string()));
        fd_ = creat(fname, PERM_664);
        check(fd_ != -1, "Unable to open log '%s'", logfile.as_string());
        // printf("Logfile created in '%s'\n", logfile.as_string().c_str());
    }
}

log::Logger::~Logger() {
    if (fd_ > 2) {
        close(fd_);
        fd_ = -1;
    }
}
