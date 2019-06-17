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
 * NAME        : trace/utils.h
 * DESCRIPTION : Generic support functions
 *****************************************************************/

#pragma once

#include <stdarg.h>
#include <string.h>

#include "cxtrace.h"

#define streq(a, b) (strcmp((a), (b)) == 0)

double get_timestamp();

cxtrace_status create_log(const char *path);
void destroy_log();
cxtrace_status log_println(const char *fmt, ...);

int option_bool(const char *name, int def);
long option_int(const char *name, long def);
long option_hex(const char *name, long def);
double option_float(const char *name, double def);
const char *option_string(const char *name, const char *def);