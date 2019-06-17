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
 * NAME        : core/instruction.cpp
 * DESCRIPTION : Dynamic location of a symbol in a module (i.e. a function)
 ******************************************************************************/

#include "location.h"

#include "support/check.h"
#include "support/filesystem.h"
#include "support/popen.h"
#include "support/string.h"

#include <fmt/format.h>

using namespace chopstix;
namespace fs = filesystem;

namespace {

static long search_module(const std::string &module, const std::string &name) {
    if (!fs::exists(module)) return -1;

    Popen readelf(fmt::format("readelf -Ws {}", module));

    std::string line;
    std::stringstream ss;
    ss << readelf;

    while (std::getline(ss, line)) {
        auto words = string::splitg(line, " \t");
        if (words.empty()) continue;
        if (words.size() < 8) continue;
        if (!::isdigit(words[0].front())) continue;
        if (words.back() != name) continue;
        return std::stol(words[1], 0, 16);
    }

    return -1;
}

}  // namespace

long Location::get_addr() {
    return symbol_.empty() ? get_addr_module() : get_addr_symbol();
}

long Location::get_addr_symbol() {
    auto maps = parse_maps(pid_);
    for (auto &entry : maps) {
        long addr = search_module(entry.path, symbol_);
        if (addr != -1) {
            entry_ = entry;
            return (addr >= entry.addr[0] && addr < entry.addr[1])
                       ? addr
                       : addr + entry.addr[0];
        }
    }
    failx("Symbol '%s' not found", symbol_);
}

long Location::get_addr_module() {
    auto maps = parse_maps(pid_);
    for (auto &entry : maps) {
        if (entry.path == module_) {
            entry_ = entry;
            return entry.addr[0];
        }
    }
    failx("Module '%s' not found", module_);
}
