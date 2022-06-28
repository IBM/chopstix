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
#pragma once

#include <linux/limits.h>
#include <signal.h>

#define REGIONS_MAX 1023

namespace chopstix {
struct mem_region {
    unsigned long addr[2];
    char perm[5];
    unsigned long offset;
    int dev[2];
    int inode;
    char path[PATH_MAX];

    // long size() const { return addr[1] - addr[0]; }
};

struct Memory {
  public:
    static Memory &instance() {
        static Memory inst;
        return inst;
    }
    Memory(const Memory &) = delete;
    Memory &operator=(const Memory &) = delete;

    void update();
    void save(int id);

    unsigned long page_addr(unsigned long addr) const {
        return (addr / pagesize_) * pagesize_;
    }

    void save_stack(int fd);
    void protect_all();
    void unprotect_all();
    void protect_region(mem_region *reg);
    void unprotect_region(mem_region *reg);
    mem_region *find_region(unsigned long page_addr);
    void protect_page(mem_region *reg, unsigned long page_addr);
    void unprotect_page(mem_region *reg, unsigned long page_addr);
    void unprotect_page_for_read(mem_region *reg, unsigned long page_addr);

    using iterator = mem_region *;
    using stack_type = stack_t;

    stack_type *alt_stack() { return &alt_stack_; }
    void restrict_map(int fd);

    static unsigned long *restricted_pages();
    mem_region *restricted_regions();

  private:
    Memory();
    ~Memory();

    iterator begin() { return map_; }
    iterator end() { return map_ + siz_; }
    void debug_all();

    long pagesize_;
    char perm_[128];
    mem_region map_[REGIONS_MAX + 1];
    long siz_ = 0;
    mem_region res_[REGIONS_MAX + 1];
    long res_siz_ = 0;
    mem_region prot_[REGIONS_MAX + 1];
    long prot_siz_ = 0;
    stack_type alt_stack_;
    char stack_buf_[SIGSTKSZ * 2];
};
}  // namespace chopstix
