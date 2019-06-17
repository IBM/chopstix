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
#include "memory.h"

#include "common/check.h"
#include "common/log.h"
#include "common/param.h"

#include <algorithm>
#include <malloc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace cxtrace;

#define PERM_R PROT_READ
#define PERM_W PROT_WRITE
#define PERM_X PROT_EXEC
#define PERM_RW (PROT_READ | PROT_WRITE)
#define PERM_RX (PROT_READ | PROT_EXEC)
#define PERM_WX (PROT_WRITE | PROT_EXEC)
#define PERM_RWX (PROT_READ | PROT_WRITE | PROT_EXEC)

static int protect_bits_R = PROT_NONE;   // Read-only (contains GOT :S)
static int protect_bits_W = PROT_NONE;   // Should never happen
static int protect_bits_X = PROT_NONE;   // Should never happen
static int protect_bits_RW = PROT_NONE;  // Data section
static int protect_bits_RX = PROT_READ;  // Text section
static int protect_bits_WX = PROT_NONE;  // Should never happen
static int protect_bits_RWX = PROT_READ; // Might happen?

#define REGION_SIZE(R) ((R)->addr[1] - (R)->addr[0])

#define REGION_FMT "%lx-%lx %s %lx %x:%x %d %s\n"
#define REGION_UNWRAP(REG)                                                     \
    REG addr[0], REG addr[1], (char *)REG perm, REG offset, REG dev[0],        \
        REG dev[1], REG inode, (char *)REG path

#define streq(A, B) (strcmp(A, B) == 0)

unsigned long libc_addrs[] = {
    (unsigned long)&open, // Space for NULL sep
    (unsigned long)&open,
    (unsigned long)&close,
    (unsigned long)&creat,
    (unsigned long)&mprotect,
    (unsigned long)&write,
    // (unsigned long)&strstr,
    (unsigned long)&strcmp,
    (unsigned long)&gsignal,
};
int libc_count = sizeof(libc_addrs) / sizeof(unsigned long);

static mem_region the_stack;

namespace {
// TODO Use safe_sscanf
static int parse_region(char *line, mem_region *region) {
    int ret = sscanf(line, REGION_FMT, REGION_UNWRAP(&region->));
    if (region->path[1] == '\0') region->path[0] = '\0';
    return ret;
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

// TODO Use safe_strstr
static int filter_region(mem_region *reg, const char *perm, unsigned long alt_sp) {
    if (!filter_perm(reg->perm, perm)) return 0;
    // These are mapped regions and should not be ignored
    if (streq(reg->path, "")) {
        return 1;
        // return 0;
    }
    if (streq(reg->path, "[stack]")) {
        the_stack = *reg;
        // return 0;
        return 1;
    }
    if (streq(reg->path, "[heap]")) return 1;
    if (strstr(reg->path, "libcxtrace.so")) return 0;
    if (strstr(reg->path, "[v")) return 0;
    if (strstr(reg->path, "/libc-") != NULL) return 1;
    if (strstr(reg->path, "/libgcc") != NULL) return 0;
    if (strstr(reg->path, "/libgfortran") != NULL) return 1;
    if (strstr(reg->path, "/libm-") != NULL) return 1;
    if (strstr(reg->path, "/libstdc++") != NULL) return 0;
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
}

Memory::Memory() {
    pagesize_ = sysconf(_SC_PAGESIZE);
    safe_strncpy(perm_, getparam("perm", "r*-"), sizeof(perm_));
    alt_stack_.ss_flags = 0;
    alt_stack_.ss_size = SIGSTKSZ * 2;
    alt_stack_.ss_sp = stack_buf_;
    // check(alt_stack_.ss_sp, "Unable to allocate alternative stack");
    log::debug("alt stack is %x-%x", (unsigned long)alt_stack_.ss_sp,
            (unsigned long)alt_stack_.ss_sp + alt_stack_.ss_size);
    for (int i = 0; i < libc_count; ++i) {
        libc_addrs[i] = libc_addrs[i] / pagesize_ * pagesize_;
    }
    std::sort(libc_addrs, libc_addrs + libc_count);
    auto it = std::unique(libc_addrs, libc_addrs + libc_count);
    libc_count = it - libc_addrs;
    for (int i = 0; i < it - libc_addrs; ++i) {
        log::debug("libc: %x", libc_addrs[i]);
    }
    libc_addrs[libc_count] = 0;

}

unsigned long *Memory::restricted_pages() {
    return libc_addrs;
}

Memory::~Memory() { }

ssize_t readline(int fd, char *buf, size_t siz) {
    char c;
    size_t tot = 0;
    while (read(fd, &c, 1) == 1) {
        buf[tot] = c;
        ++tot;
        if (tot == siz - 1) break;
        if (c == '\n') break;
    }
    buf[tot+1] = '\0';
    return tot;
}

void Memory::update() {
    log::debug("Memory::update");
    int fd = open("/proc/self/maps", O_RDONLY);
    check(fd != -1, "Unable to open maps");
    char line[1024];
    size_t n = 0;
    int stack_cnt = 0;
    while (readline(fd, line, sizeof(line))) {
        checkx(n < REGIONS_MAX, "Too many memory regions");
        parse_region(line, map_ + n);
        if (filter_region(map_ + n, perm_, (unsigned long) alt_stack_.ss_sp)) {
            ++n;
            // TODO Cleanup here
            if (streq(map_[n-1].path, "")) {
                for (long i = 0; i < res_siz_; ++i) {
                    if (res_[i].addr[0] >= map_[n-1].addr[0]
                        && res_[i].addr[0] < map_[n-1].addr[1]) {
                        log::debug("overlap maps %x-%x with res %x-%x",
                                    map_[n-1].addr[0], map_[n-1].addr[1],
                                    res_[i].addr[0], res_[i].addr[1]);
                        // log::debug("anonym region %x", map_[n-1].addr[0]);
                        // Split region
                        unsigned long res_begin = res_[i].addr[0];
                        unsigned long res_end = res_[i].addr[1];
                        if (map_[n-1].addr[0] == res_begin) {
                            // Stack is at beginning of heap
                            map_[n-1].addr[0] = res_end;
                            if (map_[n-1].addr[1] == res_end) {
                                --n;
                            }
                        } else if (map_[n-1].addr[1] == res_end) {
                            // Stack is at end of heap
                            map_[n-1].addr[1] = res_begin;
                        } else {
                            // Stack is in the middle of heap
                            map_[n] = map_[n-1];
                            map_[n].addr[0] = res_end;
                            map_[n-1].addr[1] = res_begin;
                            ++n;
                        }
                    }
                }
            }
            if (streq(map_[n-1].path, "[stack]")) {
                ++stack_cnt;
            }
            if (strstr(map_[n-1].path, "/libc-") && map_[n-1].perm[2] == 'x') {
                log::debug("original libc [x]: %x-%x", map_[n-1].addr[0], map_[n-1].addr[1]);
                int cin = 0;
                while (1) {
                    if (cin == libc_count) goto finish_libc;
                    // libc pages are at the beginning of region
                    while (libc_addrs[cin] == map_[n-1].addr[0]) {
                        map_[n-1].addr[0] += pagesize_;
                        ++cin;
                        // remaining libc pages were at end
                        if (map_[n-1].addr[0] >= map_[n-1].addr[1]) goto finish_libc;
                        // remaining region is good already
                        if (cin == libc_count) goto finish_libc;
                    }
                    // split region
                    map_[n] = map_[n-1];
                    map_[n-1].addr[1] = libc_addrs[cin];
                    map_[n].addr[0] = libc_addrs[cin] + pagesize_;
                    ++n;
                    ++cin;
                }
finish_libc:
                if (map_[n-1].addr[0] >= map_[n-1].addr[1]) n -= 1;
            }
        }
        map_[n].addr[0] = 0;
        map_[n].addr[1] = 0;
    }
    log::debug("%d stack regions", stack_cnt);
    siz_ = n;
    close(fd);
}

void Memory::restrict_map(int fd) {
    char line[1024];
    res_siz_ = 0;
    long n = 0;
    while (readline(fd, line, sizeof(line))) {
        checkx(n < REGIONS_MAX, "Too many memory regions");
        parse_region(line, res_ + n);
        ++n;
        res_[n].addr[0] = 0;
        res_[n].addr[1] = 0;
    }
    log::debug("parsed restrict_map (%d items)", n);
    res_siz_ = n;
}

// Don't do this here
void Memory::save_stack(int fd) {
    log::debug("stack: %x-%x", the_stack.addr[0], the_stack.addr[1]);
    int n = write(fd, (void *)the_stack.addr[0], REGION_SIZE(&the_stack));
}

void Memory::protect_all() {
    log::debug("protect all");
    for (auto reg = begin(); reg != end(); ++reg) {
        log::debug("protect %x-%x %s %s", reg->addr[0], reg->addr[1],
                reg->perm, reg->path);
        protect_region(reg);
    }
}

void Memory::unprotect_all() {
    for (auto reg = begin(); reg != end(); ++reg) {
        unprotect_region(reg);
    }
}

void Memory::protect_region(mem_region *reg) {
    int err = mprotect((void *)reg->addr[0], REGION_SIZE(reg),
                       protect_bits(decode_perm(reg->perm)));
    check(!err, "Unable to protect region %x-%x %s %s", reg->addr[0],
          reg->addr[1], reg->perm, reg->path);
}

void Memory::unprotect_region(mem_region *reg) {
    int err = mprotect((void *)reg->addr[0], REGION_SIZE(reg),
                       decode_perm(reg->perm));
    if (err) {
        log::warn( "Unable to unprotect region %x-%x %s %s", reg->addr[0],
            reg->addr[1], reg->perm, reg->path);
    }
    // check(!err, "Unable to unprotect region %x-%x %s %s", reg->addr[0],
    //       reg->addr[1], reg->perm, reg->path);
}

mem_region *Memory::find_region(unsigned long page_addr) {
    for (auto reg = begin(); reg != end(); ++reg) {
        if (page_addr < reg->addr[0]) continue;
        if (page_addr >= reg->addr[1]) continue;
        return reg;
    }
    return NULL;
}

void Memory::protect_page(mem_region *reg, unsigned long page_addr) {
    int err = mprotect((void *)page_addr, pagesize_,
                       protect_bits(decode_perm(reg->perm)));
    check(!err, "Unable to protect page %x in region %x-%x %s %s", page_addr,
          reg->addr[0], reg->addr[1], reg->perm, reg->path);
}

void Memory::unprotect_page(mem_region *reg, unsigned long page_addr) {
    int err = mprotect((void *)page_addr, pagesize_, decode_perm(reg->perm));
    check(!err, "Unable to unprotect page %x in region %x-%x %s %s", page_addr,
          reg->addr[0], reg->addr[1], reg->perm, reg->path);
}
