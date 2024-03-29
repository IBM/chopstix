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
#include <link.h>

static void check_address() __attribute__((constructor));

static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    char *type;
    int p_type, j;

    fprintf(stderr, "chop-marks-dyn-addr: name: \"%s\" (%d segments)\n", info->dlpi_name,
            info->dlpi_phnum);

    char const *name = info->dlpi_name;

    for (j = 0; j < info->dlpi_phnum; j++) {
        p_type = info->dlpi_phdr[j].p_type;
        type = (p_type == PT_LOAD) ? "PT_LOAD" :
            (p_type == PT_DYNAMIC) ? "PT_DYNAMIC" :
            (p_type == PT_INTERP) ? "PT_INTERP" :
            (p_type == PT_NOTE) ? "PT_NOTE" :
            (p_type == PT_INTERP) ? "PT_INTERP" :
            (p_type == PT_PHDR) ? "PT_PHDR" :
            (p_type == PT_TLS) ? "PT_TLS" :
            (p_type == PT_GNU_EH_FRAME) ? "PT_GNU_EH_FRAME" :
            (p_type == PT_GNU_STACK) ? "PT_GNU_STACK" :
            (p_type == PT_GNU_RELRO) ? "PT_GNU_RELRO" : NULL;

        fprintf(stderr, "chop-marks-dyn-addr:   %2d: [%14p (base: %14p); memsz:%7lx] flags: 0x%x; ", j,
                (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr),
                (void *) (info->dlpi_addr),
                info->dlpi_phdr[j].p_memsz,
                info->dlpi_phdr[j].p_flags);
        if (type != NULL)
            fprintf(stderr, "%s", type);
        else
            fprintf(stderr, "[other (0x%x)]", p_type);

        fprintf(stderr, " @%s@\n", name);
    }

    return 0;
}

static void check_address() {

    char * symbol = getenv("LD_SYMBOL");
    if (symbol == NULL) {
        fprintf(stderr, "chop-marks-dyn-addr: not symbol specified in LD_SYMBOL");
        exit(EXIT_FAILURE);
    }

    dl_iterate_phdr(callback, NULL);

    fprintf(stderr, "chop-marks-dyn-addr: search symbol: %s\n", symbol);

    void * handle = dlopen(NULL, RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL) {
        char * err = dlerror();
        fprintf(stderr, "chop-marks-dyn-addr: dlopen error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    void * addr = dlsym(handle, symbol);
    if (addr == NULL) {
        char * err = dlerror();
        fprintf(stderr, "chop-marks-dyn-addr: dlsym error: %s\n", err);
        fprintf(stderr, "chop-marks-dyn-addr: Symbol not exported. Make sure it is defined using 'readelf -s <binary>'\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "chop-marks-dyn-addr: %s addr is: %p\n", symbol, addr);
    fprintf(stdout, "%p\n", addr);
    exit(EXIT_SUCCESS);
}
