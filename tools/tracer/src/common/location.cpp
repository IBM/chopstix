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
#include "location.h"

#include "check.h"
#include "filesystem.h"
#include "popen.h"
#include "stringutils.h"

using namespace cxtrace;
namespace fs = filesystem;

namespace {

static long search_module(const std::string &module, const std::string &name) {
    if (!fs::exists(module)) return -1;

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "readelf -s %s", module.c_str());
    log::debug("%s", cmd);
    Popen readelf{cmd};

    std::string line;
    std::stringstream ss;
    ss << readelf;

    while (std::getline(ss, line)) {
        auto words = ssplitg(line, " \t");
        if (words.empty()) continue;
        if (words.size() < 8) continue;
        if (!::isdigit(words[0].front())) continue;
        if (words.back() != name) continue;
        return std::stol(words[1], 0, 16);
    }

    return -1;
}

} // namespace

long Location::get_addr() {
    return symbol_.empty() ? get_addr_module() : get_addr_symbol();
}

long Location::get_addr_symbol() {
    auto maps = parse_maps(pid_);
    for (auto &entry : maps) {
        if (!module_.empty() && entry.path != module_) continue;
        if (entry.perm[2] != 'x') continue;
        long addr = search_module(entry.path, symbol_);
        if (addr != -1) {
            entry_ = entry;
            return (addr >= entry.addr[0] && addr < entry.addr[1])
                       ? addr
                       : addr + entry.addr[0];
        }
    }
    checkx(0, "Symbol '%s' not found", symbol_.c_str());
}

long Location::get_addr_module() {
    auto maps = parse_maps(pid_);
    for (auto &entry : maps) {
        if (entry.path == module_) {
            entry_ = entry;
            return entry.addr[0];
        }
    }
    checkx(0, "Module '%s' not found", module_.c_str());
}
