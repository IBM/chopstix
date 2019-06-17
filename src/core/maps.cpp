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
/******************************************************************************
 * NAME        : core/maps.cpp
 * DESCRIPTION : Parsed from /proc/<pid>/maps
 ******************************************************************************/

#include "maps.h"

#include <linux/limits.h>
#include <cstdio>
#include <fstream>

#include "fmt/format.h"

#include "support/check.h"

using namespace chopstix;

using map_list = std::vector<MapEntry>;

map_list chopstix::parse_maps(long pid) {
    map_list res;

    auto path = fmt::format("/proc/{}/maps", pid);
    std::ifstream ifs(path);
    std::string line;

    char buf[PATH_MAX];

    while (std::getline(ifs, line)) {
        MapEntry entry;
        sscanf(line.c_str(), CHOPSTIX_MAP_FMT, &entry.addr[0], &entry.addr[1],
               entry.perm, &entry.offset, &entry.dev[0], &entry.dev[1],
               &entry.inode, buf);
        entry.path = buf;
        res.push_back(entry);
    }

    return res;
}
