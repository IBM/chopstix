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
 * NAME        : trace/lib.c
 * DESCRIPTION : Implementation of the main tracing calls
 *****************************************************************/

#include "config.h"
#include "cxtrace.h"
#include "defaults.h"
#include "memory.h"
#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static cxtrace_config config;
static int ready = 0;
static int tracing = 0;
static long sys_pagesize = 4096;
static long trace_id = 0;

static char trace_path[PATH_MAX];

static mem_layout layout;

#define PERM_664 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
#define PERM_775                                                          \
    S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | \
        S_IXOTH

#define TRY_CATCH(CMD) \
    if ((err = CMD)) return err

static void save_page(long page_addr) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s.%lx", trace_path, page_addr);
    int fd = creat(path, PERM_664);
    if (fd == -1) {
        log_println("error: unable to open '%s': %s", path, strerror(errno));
    } else {
        write(fd, (void *)page_addr, sys_pagesize);
        close(fd);
    }
}

static void sigsegv_handler(int sig, siginfo_t *si, void *unused) {
    long addr = (long)si->si_addr;
    long page_addr = (addr / sys_pagesize) * sys_pagesize;
    mem_region *reg = find_region(&layout, page_addr);
    if (!reg) return;
    log_println("access %x %s", addr, reg->path);
    unprotect_page(reg, page_addr);
    if (config.save_content) {
        save_page(page_addr);
    }
}

static void register_handlers() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigsegv_handler;
    sigaction(SIGSEGV, &sa, NULL);
}

cxtrace_status cxtrace_init() {
    config.log_path = strdup(option_string("log_path", DEFAULT_LOG_PATH));
    config.with_perm = strdup(option_string("with_perm", DEFAULT_WITH_PERM));
    config.trace_path = strdup(option_string("trace_path", DEFAULT_TRACE_PATH));
    config.save_content = option_bool("save_content", DEFAULT_SAVE_CONTENT);
    sys_pagesize = sysconf(_SC_PAGESIZE);
    register_handlers();
    memory_init();
    return CXTRACE_OK;
}

static cxtrace_status cxtrace_init_lazy() {
    cxtrace_status err;
    TRY_CATCH(create_log(config.log_path));
    TRY_CATCH(create_layout(&layout));
    if (config.save_content) mkdir(config.trace_path, PERM_775);
    ready = 1;
    return CXTRACE_OK;
}

static void cxtrace_quit_lazy() {
    destroy_layout(&layout);
    destroy_log();
    ready = 0;
}

void cxtrace_quit() {
    if (tracing) cxtrace_stop_trace();
    if (ready) cxtrace_quit_lazy();
    if (config.trace_path) {
        free(config.trace_path);
        config.trace_path = NULL;
    }
    if (config.log_path) {
        free(config.log_path);
        config.log_path = NULL;
    }
    if (config.with_perm) {
        free(config.with_perm);
        config.with_perm = NULL;
    }
    memory_quit();
}

cxtrace_status cxtrace_get_config(cxtrace_config *out) {
    if (!out) return CXTRACE_EINVAL;
    *out = config;
    return CXTRACE_OK;
}

cxtrace_status cxtrace_set_config(cxtrace_config *in) {
    config = *in;
    if (in->log_path) {
        free(config.log_path);
        config.log_path = strdup(in->log_path);
    }
    if (in->trace_path) {
        free(config.trace_path);
        config.trace_path = strdup(in->trace_path);
    }
    if (in->with_perm) {
        free(config.with_perm);
        config.with_perm = strdup(in->with_perm);
    }
    return CXTRACE_OK;
}

cxtrace_status cxtrace_start_trace() {
    cxtrace_status err;
    if (!ready) TRY_CATCH(cxtrace_init_lazy());
    if (tracing) return CXTRACE_EPROT;
    log_println("start trace %d", trace_id);
    snprintf(trace_path, sizeof(trace_path), "%s/page.%ld", config.trace_path,
             trace_id);
    TRY_CATCH(update_layout(&layout, config.with_perm));
    TRY_CATCH(protect_memory(&layout));
    tracing = 1;
    return CXTRACE_OK;
}

cxtrace_status cxtrace_stop_trace() {
    cxtrace_status err;
    if (!tracing) return CXTRACE_EPROT;
    log_println("stop trace %d", trace_id);
    TRY_CATCH(unprotect_memory(&layout));
    tracing = 0;
    if (config.save_content) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/maps.%ld", config.trace_path,
                 trace_id);
        int fd = creat(path, PERM_664);
        save_layout(&layout, fd);
        close(fd);
    }
    ++trace_id;
    return CXTRACE_OK;
}

const char *cxtrace_strerror(cxtrace_status err) {
    switch (err) {
        case CXTRACE_OK: return "No error";
        case CXTRACE_EINVAL: return "Invalid argument";
        case CXTRACE_EPROT: return "Protocol error";
        case CXTRACE_ESPACE: return "Out of space";
        case CXTRACE_EOPEN: return "Unable to open file";
        case CXTRACE_EIMPL: return "Not implemented";
        default: return "Unknown error";
    }
}

const char *cxtrace_version(void) { return CHOPSTIX_VERSION; }

// Dynamic instrumentation wrappers

void cxtrace_init_dyn() {
    cxtrace_init();
    atexit(cxtrace_quit);
    raise(SIGTRAP);
}

void cxtrace_start_trace_dyn() {
    cxtrace_start_trace();
    raise(SIGTRAP);
}

void cxtrace_stop_trace_dyn() {
    cxtrace_stop_trace();
    raise(SIGTRAP);
}