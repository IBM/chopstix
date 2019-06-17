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
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void cxtrace::assert_impl(bool cond, const char *msg,
        const char *fname, int lineno, const char *test) {
#ifndef NDEBUG
    if (cond) return;
    fprintf(stderr, "Error: %s\n", msg);
    fprintf(stderr, "Assertion '%s' failed in %s at line %d\n",
            test, fname, lineno);
    fprintf(stderr, "Backtrace:\n");
    void *callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);
    for (int i = 1; i < frames; ++i) {
        fprintf(stderr, "(%d)  %s\n", i, strs[i]);
    }
    free(strs);
    abort();
#endif
}
