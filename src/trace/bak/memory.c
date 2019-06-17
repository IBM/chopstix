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
 * NAME        : trace/memory.c
 * DESCRIPTION : Helper functions to deal with memory layout
 *****************************************************************/

#include "memory.h"

#include "utils.h"

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

// TODO Make this heap allocated
#define REGIONS_MAX 1023
static mem_region the_layout[REGIONS_MAX + 1];

#define PERM_R PROT_READ
#define PERM_W PROT_WRITE
#define PERM_X PROT_EXEC
#define PERM_RW (PROT_READ | PROT_WRITE)
#define PERM_RX (PROT_READ | PROT_EXEC)
#define PERM_WX (PROT_WRITE | PROT_EXEC)
#define PERM_RWX (PROT_READ | PROT_WRITE | PROT_EXEC)

static int protect_bits_R = PROT_NONE;    // Read-only (contains GOT :S)
static int protect_bits_W = PROT_NONE;    // Should never happen
static int protect_bits_X = PROT_NONE;    // Should never happen
static int protect_bits_RW = PROT_NONE;   // Data section
static int protect_bits_RX = PROT_READ;   // Text section
static int protect_bits_WX = PROT_NONE;   // Should never happen
static int protect_bits_RWX = PROT_READ;  // Might happen?

static long sys_pagesize;

cxtrace_status memory_init() {
    // TODO Set protect_bits depending on architecture
    // TODO Allocate save space on heap
    sys_pagesize = sysconf(_SC_PAGESIZE);
    return CXTRACE_OK;
}

void memory_quit() {}

#define IS_REGION(R) ((R)->addr[0] != (R)->addr[1])
#define REGION_SIZE(R) ((R)->addr[1] - (R)->addr[0])

#define REGION_FMT "%lx-%lx %s %lx %x:%x %d %s\n"
#define REGION_UNWRAP(REG)                                              \
    REG addr[0], REG addr[1], (char *)REG perm, REG offset, REG dev[0], \
        REG dev[1], REG inode, (char *)REG path

static int parse_region(char *line, mem_region *region) {
    return sscanf(line, REGION_FMT, REGION_UNWRAP(&region->));
}

static int filter_perm(const char *test, const char *perm) {
    int i = 0;
    int match = 1;
    for (char *p = (char *)perm; *p != '\0'; ++p) {
        if (i >= 4) return 0;
        switch (*p) {
            case ',':
                if (match) return 1;
                i = 0;
                match = 1;
                break;
            case 'r':
            case 'w':
            case 'x':
            case '-':
                if (test[i] != *p) match = 0;
            case '*': ++i;
        }
    }
    return match;
}

static int filter_region(mem_region *reg, const char *perm) {
    if (!filter_perm(reg->perm, perm)) return 0;
    // TODO These are mapped regions and should not be ignored
    if (streq(reg->path, "")) return 0;
    if (streq(reg->path, "[stack]")) return 0;
    if (strstr(reg->path, "libcxtrace.so")) return 0;
    if (strstr(reg->path, "[v")) return 0;
    if (strstr(reg->path, "/libc-") != NULL) return 0;
    if (strstr(reg->path, "/ld-") != NULL) return 0;
    return 1;
}

static int decode_perm(const char *perm) {
    int prot = 0;
    if (perm[0] == 'r') prot |= PROT_READ;
    if (perm[1] == 'w') prot |= PROT_WRITE;
    if (perm[2] == 'x') prot |= PROT_EXEC;
    return prot;
}

static const char *encode_perm(int prot) {
    switch (prot) {
        case PERM_R: return "r--";
        case PERM_W: return "-w-";
        case PERM_X: return "--x";
        case PERM_RW: return "rw-";
        case PERM_RX: return "r-x";
        case PERM_WX: return "-wx";
        case PERM_RWX: return "rwx";
        default: return "---";
    }
}

static int protect_bits(int prot) {
    switch (prot) {
        case PERM_R: return protect_bits_R;
        case PERM_W: return protect_bits_W;
        case PERM_X: return protect_bits_X;
        case PERM_RW: return protect_bits_RW;
        case PERM_RX: return protect_bits_RX;
        case PERM_WX: return protect_bits_WX;
        case PERM_RWX: return protect_bits_RWX;
        default: return PROT_NONE;
    }
}

cxtrace_status create_layout(mem_layout *layout) {
    *layout = the_layout;
    return CXTRACE_OK;
}

cxtrace_status update_layout(mem_layout *layout, const char *perm) {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp == NULL) return CXTRACE_EOPEN;
    char line[PATH_MAX];
    size_t n = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (n == REGIONS_MAX) return CXTRACE_ESPACE;
        parse_region(line, *layout + n);
        if (filter_region(*layout + n, perm)) ++n;
        (*layout)[n].addr[0] = 0;
        (*layout)[n].addr[1] = 0;
    }
    (*layout)[n].addr[0] = 0;
    (*layout)[n].addr[1] = 0;
    fclose(fp);
    return CXTRACE_OK;
}

void destroy_layout(mem_layout *layout) {}

cxtrace_status save_layout(mem_layout *layout, int fd) {
    char buf[PATH_MAX];
    for (mem_region *reg = *layout; IS_REGION(reg); ++reg) {
        int n = snprintf(buf, sizeof(buf), REGION_FMT, REGION_UNWRAP(reg->));
        write(fd, buf, n);
    }
    return CXTRACE_OK;
}

cxtrace_status protect_memory(mem_layout *layout) {
    for (mem_region *reg = *layout; IS_REGION(reg); ++reg) {
        log_println("protect %x-%x %s %s", reg->addr[0], reg->addr[1],
                    reg->perm, reg->path);
        protect_region(reg);
    }
    return CXTRACE_OK;
}

cxtrace_status unprotect_memory(mem_layout *layout) {
    for (mem_region *reg = *layout; IS_REGION(reg); ++reg) {
        unprotect_region(reg);
    }
    return CXTRACE_OK;
}

cxtrace_status protect_region(mem_region *region) {
    int err = mprotect((void *)region->addr[0], REGION_SIZE(region),
                       protect_bits(decode_perm(region->perm)));
    if (err) return CXTRACE_EINVAL;
    return CXTRACE_OK;
}
cxtrace_status unprotect_region(mem_region *region) {
    int err = mprotect((void *)region->addr[0], REGION_SIZE(region),
                       decode_perm(region->perm));
    if (err) return CXTRACE_EINVAL;
    return CXTRACE_OK;
}

mem_region *find_region(mem_layout *layout, long page_addr) {
    for (mem_region *reg = *layout; IS_REGION(reg); ++reg) {
        if (page_addr < reg->addr[0]) continue;
        if (page_addr >= reg->addr[1]) continue;
        return reg;
    }
    return NULL;
}

cxtrace_status protect_page(mem_region *region, long page_addr) {
    int err = mprotect((void *)page_addr, sys_pagesize,
                       protect_bits(decode_perm(region->perm)));
    if (err) return CXTRACE_EINVAL;
    return CXTRACE_OK;
}

cxtrace_status unprotect_page(mem_region *region, long page_addr) {
    int err =
        mprotect((void *)page_addr, sys_pagesize, decode_perm(region->perm));
    if (err) return CXTRACE_EINVAL;
    return CXTRACE_OK;
}