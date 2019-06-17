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
 * NAME        : trace/memory.h
 * DESCRIPTION : Helper functions to deal with memory layout
 *****************************************************************/

#pragma once

#include "cxtrace.h"

#include <linux/limits.h>

typedef struct mem_region {
    long addr[2];
    char perm[5];
    long offset;
    int dev[2];
    int inode;
    char path[PATH_MAX];
} mem_region;

typedef mem_region *mem_layout;

cxtrace_status memory_init();
void memory_quit();

cxtrace_status create_layout(mem_layout *layout);
cxtrace_status update_layout(mem_layout *layout, const char *perm);
void destroy_layout(mem_layout *layout);
cxtrace_status save_layout(mem_layout *layout, int fd);

cxtrace_status protect_memory(mem_layout *layout);
cxtrace_status unprotect_memory(mem_layout *layout);

cxtrace_status protect_region(mem_region *region);
cxtrace_status unprotect_region(mem_region *region);

mem_region *find_region(mem_layout *layout, long page_addr);

cxtrace_status protect_page(mem_region *region, long page_addr);
cxtrace_status unprotect_page(mem_region *region, long page_addr);