/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2022 IBM Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

static void check_address() __attribute__((constructor));

void check_address() {

    char * symbol = getenv("LD_SYMBOL");
    if (symbol == NULL) {
        fprintf(stderr, "chop-marks-dyn-addr: not symbol specified in LD_SYMBOL");
        exit(1);
    }

    fprintf(stderr, "chop-marks-dyn-addr: search symbol: %s\n", symbol);

    void * handle = dlopen(NULL, RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL) {
        char * err = dlerror();
        fprintf(stderr, "chop-marks-dyn-addr: dlopen error: %s\n", err);
        exit(1);
    }

    void * addr = dlsym(handle, symbol);
    if (addr == NULL) {
        char * err = dlerror();
        fprintf(stderr, "chop-marks-dyn-addr: dlsym error: %s\n", err);
        fprintf(stderr, "chop-marks-dyn-addr: Symbol not exported. Make sure it is defined using 'readelf -s <binary>'\n"); 
        exit(1);
    }

    fprintf(stderr, "chop-marks-dyn-addr: %s addr is: %p\n", symbol, addr);
    fprintf(stdout, "%p\n", addr);
    exit(0);
}
