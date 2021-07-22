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

#include "support/check.h"
#include "support/log.h"
#include "support/options.h"

#include <fcntl.h>
#include <malloc.h>
#include <sys/mman.h>
#include <unistd.h>
#include <algorithm>
#include <string.h>

#include <sys/syscall.h>

using namespace chopstix;

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
static int protect_bits_RX = PROT_NONE;   // Text section
static int protect_bits_WX = PROT_NONE;   // Should never happen
static int protect_bits_RWX = PROT_READ;  // Might happen?

#define REGION_SIZE(R) ((R)->addr[1] - (R)->addr[0])

#define REGION_FMT "%lx-%lx %s %lx %x:%x %d %s\n"
#define REGION_UNWRAP(REG)                                              \
    REG addr[0], REG addr[1], (char *)REG perm, REG offset, REG dev[0], \
        REG dev[1], REG inode, (char *)REG path

#define streq(A, B) (strcmp(A, B) == 0)

extern void *__libc_init_first;
extern void *__libc_start_main;
// extern void* explicit_bzero;
// extern void* explicit_bzero  __attribute__((weak)) = __libc_init_first; //
// Ensure the symbol is always defined

// Register here the libc/libpthread reserved symbols.
// Pages containing these symbols will not be protected and
// always dumped to disk
unsigned long libc_addrs[] = {

    (unsigned long)&open,  // Space for NULL sep
    (unsigned long)&open,  // Space for NULL sep
    (unsigned long)&syscall,

#ifdef PROTECTALLSYMBOLS
    (unsigned long)&open,
    (unsigned long)&close,
    (unsigned long)&creat,
    (unsigned long)&mprotect,
    (unsigned long)&write,
    (unsigned long)&strcmp,
    (unsigned long)&gsignal,
    (unsigned long)&signal,

    //(unsigned long)&strstr,
    (unsigned long)&__libc_init_first,
    (unsigned long)&__libc_start_main,
    (unsigned long)&explicit_bzero,
    (unsigned long)&bzero,
#endif
};
int libc_count = sizeof(libc_addrs) / sizeof(unsigned long);
// int libc_count = -1;

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
                continue;
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
static int filter_region(mem_region *reg, const char *perm,
                         unsigned long alt_sp) {
    // return 1 for protecting the region and zero for not
    // protecting it. The more 1s the better
    if (!filter_perm(reg->perm, perm)) return 0;
    if (streq(reg->path, "")) return 1;
    if (streq(reg->path, "[stack]")) return 1;
    if (streq(reg->path, "[heap]")) return 1;
    if (strstr(reg->path, "/libc-") && strstr(reg->perm, "r--p")) return 1;
    if (strstr(reg->path, "/libc-") && strstr(reg->perm, "rw-p")) return 1;
    if (strstr(reg->path, "/libc-") != NULL) return 1;
    if (strstr(reg->path, "/libpthread-") != NULL) return 1;
    if (strstr(reg->path, "/libgfortran") != NULL) return 1;
    if (strstr(reg->path, "/libm-") != NULL) return 1;
    if (strstr(reg->path, "/libgcc") != NULL) return 1;
    if (strstr(reg->path, "/libstdc++") != NULL) return 1;
    if (strstr(reg->path, "/libcxtrace.so")) return 0;
    if (strstr(reg->path, "/ld-") != NULL) return 0;
    if (strstr(reg->path, "[v")) return 0;
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
}  // namespace

Memory::Memory() {
    pagesize_ = sysconf(_SC_PAGESIZE);
    if (getopt("code").as_bool()) {
        safe_strncpy(perm_, "r*-,r-x", sizeof(perm_));
    } else {
        safe_strncpy(perm_, "r*-", sizeof(perm_));
    }
    alt_stack_.ss_flags = 0;
    alt_stack_.ss_size = SIGSTKSZ * 2;
    alt_stack_.ss_sp = stack_buf_;
    // check(alt_stack_.ss_sp, "Unable to allocate alternative stack");
    log::debug("Memory::Memory init: alt stack is %x-%x",
               (unsigned long)alt_stack_.ss_sp,
               (unsigned long)alt_stack_.ss_sp + alt_stack_.ss_size);

    for (int i = 0; i < libc_count; ++i) {
        libc_addrs[i] = libc_addrs[i] / pagesize_ * pagesize_;
        log::debug(
            "Memory::Memory init: registering libc page for protected "
            "function: %x",
            (unsigned long)libc_addrs[i]);
    }
    std::sort(libc_addrs, libc_addrs + libc_count);
    auto it = std::unique(libc_addrs, libc_addrs + libc_count);
    libc_count = it - libc_addrs;
    for (int i = 0; i < it - libc_addrs; ++i) {
        log::verbose(
            "Memory::Memory init: registering libc page for protected function "
            "(sorted/uniq): %x",
            (unsigned long)libc_addrs[i]);
    }
    libc_addrs[libc_count] = 0;

    prot_[0].addr[0] = 0;
    prot_[0].addr[1] = 0;
}

unsigned long *Memory::restricted_pages() { return libc_addrs; }
mem_region *Memory::restricted_regions() { return prot_; }

Memory::~Memory() {}

ssize_t readline(int fd, char *buf, size_t siz) {
    char c;
    size_t tot = 0;
    while (syscall(SYS_read, fd, &c, 1) == 1) {
        buf[tot] = c;
        ++tot;
        if (tot == siz - 1) break;
        if (c == '\n') break;
    }
    buf[tot + 1] = '\0';
    return tot;
}

void Memory::update() {
    log::debug("Memory::update: Memory::update");
    int fd = syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY);
    check(fd != -1, "Unable to open maps");
    char line[1024];
    size_t n = 0;
    int stack_cnt = 0;

    prot_siz_ = 0;
    prot_[prot_siz_].addr[0] = 0;
    prot_[prot_siz_].addr[1] = 0;

    while (readline(fd, line, sizeof(line))) {
        checkx(n < REGIONS_MAX, "Too many memory regions");
        parse_region(line, map_ + n);
        log::debug("Memory::update: raw parsed line: %x-%x %s %s",
                   map_[n].addr[0], map_[n].addr[1], map_[n].perm,
                   map_[n].path);

        if (filter_region(map_ + n, perm_, (unsigned long)alt_stack_.ss_sp)) {
            log::debug("Memory::update: not filtered. current index: %d", n);
            ++n;
            // TODO Cleanup here
            if (streq(map_[n - 1].path, "")) {
                for (long i = 0; i < res_siz_; ++i) {
                    if (res_[i].addr[0] >= map_[n - 1].addr[0] &&
                        res_[i].addr[0] < map_[n - 1].addr[1]) {
                        log::debug(
                            "Memory::update: overlap maps %x-%x with res %x-%x",
                            map_[n - 1].addr[0], map_[n - 1].addr[1],
                            res_[i].addr[0], res_[i].addr[1]);
                        // log::debug("anonym region %x", map_[n-1].addr[0]);
                        // Split region
                        unsigned long res_begin = res_[i].addr[0];
                        unsigned long res_end = res_[i].addr[1];
                        if (map_[n - 1].addr[0] == res_begin) {
                            // Stack is at beginning of heap
                            map_[n - 1].addr[0] = res_end;
                            if (map_[n - 1].addr[1] == res_end) {
                                --n;
                            }
                        } else if (map_[n - 1].addr[1] == res_end) {
                            // Stack is at end of heap
                            map_[n - 1].addr[1] = res_begin;
                        } else {
                            // Stack is in the middle of heap
                            map_[n] = map_[n - 1];
                            map_[n].addr[0] = res_end;
                            map_[n - 1].addr[1] = res_begin;
                            ++n;
                        }
                    }
                }
            }
            if (streq(map_[n - 1].path, "[stack]")) {
                ++stack_cnt;
            }
            if ((strstr(map_[n - 1].path, "/libc-") ||
                 strstr(map_[n - 1].path, "/libpthread-")) &&
                map_[n - 1].perm[2] == 'x') {
                log::debug(
                    "Memory::update: original mapping for reserved symbols "
                    "[x]: %x-%x",
                    map_[n - 1].addr[0], map_[n - 1].addr[1]);

                int cin = 0;
                // if (strstr(map_[n - 1].path, "/libc-"))
                //{
                //    while(map_[n-1].addr[0] < libc_addrs[cin]) {
                //        log::debug("Skip initial libc pages...");
                //        map_[n-1].addr[0] += pagesize_;
                //    }
                //}

                while (libc_addrs[cin] < map_[n - 1].addr[0] &&
                       cin < libc_count) {
                    cin++;
                }

                unsigned long end_address = map_[n - 1].addr[1];

                while (map_[n - 1].addr[0] == libc_addrs[cin] &&
                       cin < libc_count) {
                    log::debug("Memory::update: skip initial protected page %x",
                               libc_addrs[cin]);
                    map_[n - 1].addr[0] += pagesize_;
                    cin++;
                }

                map_[n - 1].addr[1] = map_[n - 1].addr[0] + pagesize_;

                while (map_[n - 1].addr[1] <= end_address && cin < libc_count) {
                    if (libc_addrs[cin] != map_[n - 1].addr[1]) {
                        if ((map_[n - 1].addr[1] + pagesize_) > end_address) {
                            log::debug(
                                "Memory::update: region last split: %x-%x %s "
                                "%s",
                                map_[n - 1].addr[0], map_[n - 1].addr[1],
                                map_[n - 1].perm, map_[n - 1].path);
                            ++n;
                            break;
                        }
                        map_[n - 1].addr[1] += pagesize_;
                    } else {
                        map_[n] = map_[n - 1];
                        map_[n].addr[0] = map_[n - 1].addr[1];
                        map_[n].addr[1] = map_[n].addr[0] + pagesize_;

                        log::debug("Memory::update: region split: %x-%x %s %s",
                                   map_[n - 1].addr[0], map_[n - 1].addr[1],
                                   map_[n - 1].perm, map_[n - 1].path);

                        ++n;
                        while (map_[n - 1].addr[0] == libc_addrs[cin] &&
                               cin < libc_count) {
                            log::debug(
                                "Memory::update: skip consecutive protected "
                                "page %x",
                                libc_addrs[cin]);
                            map_[n - 1].addr[0] += pagesize_;
                            map_[n - 1].addr[1] += pagesize_;
                            cin++;
                        }

                        if ((map_[n - 1].addr[0]) >= end_address) {
                            break;
                        }
                    }
                }
                // Check pages were not at the end

                // while (1) {
                //    if (cin == libc_count) goto finish_libc;
                //     // libc pages are at the beginning of region
                //    while (libc_addrs[cin] == map_[n - 1].addr[0]) {
                //        map_[n - 1].addr[0] += pagesize_;
                //        ++cin;
                //    }
                //    // remaining libc pages were at end
                //    if (map_[n - 1].addr[0] >= map_[n - 1].addr[1])
                //        goto finish_libc;
                //        // remaining region is good already
                //        if (cin == libc_count) goto finish_libc;
                //    // split region
                //    map_[n] = map_[n - 1];
                //    map_[n - 1].addr[1] = libc_addrs[cin];
                //    map_[n].addr[0] = libc_addrs[cin] + pagesize_;
                //    ++n;
                //    ++cin;
                //}
                // finish_libc:
                // if (map_[n - 1].addr[0] >= map_[n - 1].addr[1]) n -= 1;
            }
        } else {
            if (!strstr(map_[n].path, "/libcxtrace")) {
                log::debug("Region not protected. Registering for dump");
                prot_[prot_siz_].addr[0] = map_[n].addr[0];
                prot_[prot_siz_].addr[1] = map_[n].addr[1];
                safe_strncpy(prot_[prot_siz_].perm, map_[n].perm,
                             sizeof(map_[n].perm));
                safe_strncpy(prot_[prot_siz_].path, map_[n].path,
                             sizeof(map_[n].path));
                prot_siz_++;
                prot_[prot_siz_].addr[0] = 0;
                prot_[prot_siz_].addr[1] = 0;
            }
        }
        map_[n].addr[0] = 0;
        map_[n].addr[1] = 0;
    }
    log::debug("Memory::update: %d stack regions", stack_cnt);
    siz_ = n;
    syscall(SYS_close, fd);
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
    log::debug("Memory:: restict_map: parsed restrict_map (%d items)", n);
    res_siz_ = n;
}

void Memory::protect_all() {
    log::debug("Memory: proctect_all: protect all. Start.");
    for (auto reg = begin(); reg != end(); ++reg) {
        log::debug("Memory::protect_all: protect %x-%x %s %s", reg->addr[0],
                   reg->addr[1], reg->perm, reg->path);
        protect_region(reg);
    }
    log::debug("Memory: proctect_all: protect all. End.");
}

void Memory::unprotect_all() {
    for (auto reg = begin(); reg != end(); ++reg) {
        unprotect_region(reg);
    }
}

void Memory::protect_region(mem_region *reg) {
    log::debug("Memory::protect_region: %x-%x %s %s (bits: %d)", reg->addr[0],
               reg->addr[1], reg->perm, reg->path,
               protect_bits(decode_perm(reg->perm)));
    // int err = mprotect((void *)reg->addr[0], REGION_SIZE(reg),
    //                    protect_bits(decode_perm(reg->perm)));

    int err = syscall(SYS_mprotect, reg->addr[0], REGION_SIZE(reg),
                      protect_bits(decode_perm(reg->perm)));

    log::debug("Memory::protect_region: %x-%x %s %s (bits: %d) OK",
               reg->addr[0], reg->addr[1], reg->perm, reg->path,
               protect_bits(decode_perm(reg->perm)));

    check(!err, "Unable to protect region %x-%x %s %s", reg->addr[0],
          reg->addr[1], reg->perm, reg->path);
    log::debug("Memory::protect_region: protected %d bytes at %x",
               REGION_SIZE(reg), reg->addr[0]);
}

void Memory::unprotect_region(mem_region *reg) {
    log::debug("Memory::unprotect_region: %x-%x %s %s (bits: %d)", reg->addr[0],
               reg->addr[1], reg->perm, reg->path, decode_perm(reg->perm));
    int err = syscall(SYS_mprotect, (void *)reg->addr[0], REGION_SIZE(reg),
                      decode_perm(reg->perm));
    if (err) {
        log::warn("Unable to unprotect region %x-%x %s %s", reg->addr[0],
                  reg->addr[1], reg->perm, reg->path);
    }
    // check(!err, "Unable to unprotect region %x-%x %s %s", reg->addr[0],
    //       reg->addr[1], reg->perm, reg->path);
}

mem_region *Memory::find_region(unsigned long page_addr) {
    log::debug("Memory::find_region: Region for address %x", page_addr);
    for (auto reg = begin(); reg != end(); ++reg) {
        // log::debug("Memory::find_region: Check region %x-%x %s %s",
        //    reg->addr[0], reg->addr[1], reg->perm, reg->path);

        if (page_addr < reg->addr[0]) continue;
        if (page_addr >= reg->addr[1]) continue;

        log::debug("Memory::find_region: Region found: %x-%x %s %s",
                   reg->addr[0], reg->addr[1], reg->perm, reg->path);

        return reg;
    }
    return NULL;
}

void Memory::protect_page(mem_region *reg, unsigned long page_addr) {
    log::debug("Memory::protect_page: %x in region %x-%x %s %s (bits: %d)",
               page_addr, reg->addr[0], reg->addr[1], reg->perm, reg->path,
               protect_bits(decode_perm(reg->perm)));
    int err = syscall(SYS_mprotect, (void *)page_addr, pagesize_,
                      protect_bits(decode_perm(reg->perm)));
    check(!err, "Unable to protect page %x in region %x-%x %s %s", page_addr,
          reg->addr[0], reg->addr[1], reg->perm, reg->path);
}

void Memory::unprotect_page(mem_region *reg, unsigned long page_addr) {
    log::debug("Memory::unprotect_page: %x in region %x-%x %s %s (bits: %d)",
               page_addr, reg->addr[0], reg->addr[1], reg->perm, reg->path,
               decode_perm(reg->perm));
    int err = syscall(SYS_mprotect, (void *)page_addr, pagesize_,
                      decode_perm(reg->perm));
    check(!err, "Unable to unprotect page %x in region %x-%x %s %s", page_addr,
          reg->addr[0], reg->addr[1], reg->perm, reg->path);
    log::debug("Memory::unprotect_page: unprotected %d bytes at %x", pagesize_,
               page_addr);
}

void Memory::debug_all() {
    for (auto reg = begin(); reg != end(); ++reg) {
        log::debug("Memory::debug_all: %x-%x %s %s", reg->addr[0], reg->addr[1],
                   reg->perm, reg->path);
    }
}
