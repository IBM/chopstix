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

#pragma once

#include "maps.h"
#include "types.h"

namespace chopstix {

class Location {
  public:
    operator long() { return addr(); }
    long addr() {
        if (addr_ == -1) addr_ = get_addr();
        return addr_;
    }

    Location(long pid, std::string symbol, std::string module)
        : pid_(pid), symbol_(symbol), module_(module) {}
    const MapEntry& entry() const { return entry_; }
    const std::string symbol() const { return symbol_; }
    const std::string module() const { return module_; }

    static Location Symbol(long pid, std::string name) {
        return Location(pid, name, "");
    }

    static Location Symbol(long pid, std::string name, std::string md_name) {
        return Location(pid, name, md_name);
    }

    static Location Module(long pid, std::string name) {
        return Location(pid, "", name);
    }

    static Location Address(long addr) {
        Location loc(-1, "", "");
        loc.addr_ = addr;
        return loc;
    }

  private:
    long get_addr();
    long get_addr_symbol();
    long get_addr_module();

    long pid_;
    std::string symbol_;
    std::string module_;

    MapEntry entry_;
    long addr_ = -1;
};

}  // namespace chopstix
