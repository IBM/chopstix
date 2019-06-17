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
/*****************************************************************
 * NAME        : trace/utils.c
 * DESCRIPTION : Generic support functions
 *****************************************************************/

#include "utils.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// TODO Use clock here for better precision
double get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

#define LOG_WIDTH 1024
static int logfile = -1;
static char logbuf[LOG_WIDTH];

cxtrace_status create_log(const char *path) {
    if (logfile != -1) destroy_log();
    logfile = creat(path, PERM_664);
    if (logfile == -1) return CXTRACE_EOPEN;
    return CXTRACE_OK;
}

void destroy_log() {
    if (logfile) close(logfile);
}

cxtrace_status log_println(const char *fmt, ...) {
    int n;
    n = snprintf(logbuf, sizeof(logbuf), "%.6f> ", get_timestamp());
    write(logfile, logbuf, n);
    va_list args;
    va_start(args, fmt);
    n = vsnprintf(logbuf, sizeof(logbuf), fmt, args);
    if (n >= sizeof(logbuf)) return CXTRACE_EINVAL;
    write(logfile, logbuf, n);
    write(logfile, "\n", 1);
    va_end(args);
    return CXTRACE_OK;
}

static const char *get_value(const char *name) {
    char envname[128];
    int n = snprintf(envname, sizeof(envname), "CHOPSTIX_OPT_%s", name);
    for (int i = 0; i < n; ++i) {
        envname[i] = toupper(envname[i]);
    }
    return getenv(envname);
}

int option_bool(const char *name, int def) {
    const char *val = get_value(name);
    if (val == NULL) return def;
    if (strcasecmp(val, "yes") == 0) return 1;
    if (strcasecmp(val, "true") == 0) return 1;
    if (strcasecmp(val, "on") == 0) return 1;
    if (strcasecmp(val, "1") == 0) return 1;
    if (strcasecmp(val, "no") == 0) return 0;
    if (strcasecmp(val, "false") == 0) return 0;
    if (strcasecmp(val, "off") == 0) return 0;
    if (strcasecmp(val, "0") == 0) return 0;
    return 0;
}

long option_int(const char *name, long def) {
    const char *val = get_value(name);
    if (val == NULL) return def;
    return 0;
}

long option_hex(const char *name, long def) {
    const char *val = get_value(name);
    if (val == NULL) return def;
    return 0;
}

double option_float(const char *name, double def) {
    const char *val = get_value(name);
    if (val == NULL) return def;
    return 0;
}

const char *option_string(const char *name, const char *def) {
    const char *val = get_value(name);
    if (val == NULL) return def;
    return val;
}