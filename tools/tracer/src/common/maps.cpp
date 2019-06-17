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
#include "maps.h"

#include <cstdio>
#include <fstream>
#include <linux/limits.h>

#include "check.h"

using namespace cxtrace;

using map_list = std::vector<MapEntry>;

map_list cxtrace::parse_maps(long pid) {
    map_list res;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%ld/maps", pid);
    std::ifstream ifs(path);
    std::string line;

    char buf[PATH_MAX];

    while (std::getline(ifs, line)) {
        MapEntry entry;
        sscanf(line.c_str(), "%lx-%lx %s %lx %x:%x %d %s\n", &entry.addr[0],
               &entry.addr[1], entry.perm, &entry.offset, &entry.dev[0],
               &entry.dev[1], &entry.inode, buf);
        entry.path = buf;
        res.push_back(entry);
    }

    return res;
}
