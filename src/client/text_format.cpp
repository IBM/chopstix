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
/***********************************************************
 * NAME        : client/text_format.cpp
 * DESCRIPTION : Implements various formatters for assembly
 *               instructions
 ***********************************************************/

#include "text_format.h"

#include <algorithm>

#include "fmt/format.h"
#include "fmt/ostream.h"

using namespace chopstix;

std::ostream &TextFormat::header(std::ostream &os) { return os; }

std::ostream &TextFormat::format(std::ostream &os, const Module &module) {
    fmt::print(os, "{} {}\n", module.name(), module.arch());
    for (auto &func : module.funcs()) format(os, *func);
    return os;
}

std::ostream &TextFormat::format(std::ostream &os, const Function &func) {
    fmt::print(os, "{:08x} <{}>:\n", func.addr(), func.name());
    for (auto &block : func.blocks()) format(os, *block);
    return os;
}

std::ostream &TextFormat::format(std::ostream &os, const BasicBlock &block) {
    fmt::print(os, "+ {:08x}:", block.addr());
    std::string pref = "=>";
    for (auto ptr : block.next()) {
        if (auto next = ptr.lock()) {
            fmt::print(os, " {} {:08x}", pref, next->addr());
            pref = ",";
        }
    }
    fmt::print(os, "\n");
    for (auto &inst : block.insts()) format(os, inst);
    return os;
}

std::ostream &TextFormat::format(std::ostream &os, const Instruction &inst) {
    std::string raw = "";
    auto bytes = inst.bytes();
    for (auto byte : bytes) raw += fmt::format(" {:02x}", byte);
    fmt::print(os, "  {:08x}: {:25s} {}\n", inst.addr, raw, inst.text);
    return os;
}

std::ostream &TextFormat::format(std::ostream &os, const Path &path) {
    fmt::print(os, "Path from {:08x} to {:08x}\n", path.nodes().front()->addr(),
               path.nodes().back()->addr());
    for (auto &block : path.nodes()) format(os, *block);
    return os;
}

namespace {

std::ostream &format_score(std::ostream &os, double score) {
    if (score < 0.001) {
        os << "       ";
    } else {
        fmt::print(os, "{: 6.1f}%", score * 100);
    }
    return os << " | ";
}

}  // namespace

std::ostream &AnnotFormat::format(std::ostream &os, const Module &module) {
    format_score(os, module.score());
    return TextFormat::format(os, module);
}

std::ostream &AnnotFormat::format(std::ostream &os, const Function &func) {
    format_score(os, func.score());
    return TextFormat::format(os, func);
}

std::ostream &AnnotFormat::format(std::ostream &os, const BasicBlock &block) {
    format_score(os, block.score());
    return TextFormat::format(os, block);
}

std::ostream &AnnotFormat::format(std::ostream &os, const Instruction &inst) {
    format_score(os, inst.score());
    return TextFormat::format(os, inst);
}

std::ostream &AnnotFormat::format(std::ostream &os, const Path &path) {
    format_score(os, path.score());
    return TextFormat::format(os, path);
}

std::ostream &MptFormat::header(std::ostream &os) {
    return os << "[MPT]\n"
                 "mpt_version = 0.5 ; Format version of this MPT file.\n"
                 "\n"
                 "[CODE]\n"
                 "instructions = \n";
}

std::ostream &MptFormat::format(std::ostream &os, const Module &module) {
    fmt::print(os, "; module-id {}\n", module.rowid());
    fmt::print(os, "; module-name {}\n", module.name());
    fmt::print(os, "; module-range {}\n", module.range().repr());
    fmt::print(os, "; module-score {}\n", module.score());
    for (auto &func : module.funcs()) format(os, *func);
    return os;
}

std::ostream &MptFormat::format(std::ostream &os, const Function &func) {
    fmt::print(os, "; func-id {}\n", func.rowid());
    fmt::print(os, "; func-name {}\n", func.name());
    fmt::print(os, "; func-range {}\n", func.range().repr());
    fmt::print(os, "; func-score {}\n", func.score());
    for (auto &block : func.blocks()) format(os, *block);
    return os;
}

std::ostream &MptFormat::format(std::ostream &os, const BasicBlock &block) {
    fmt::print(os, "; block-id {}\n", block.rowid());
    fmt::print(os, "; block-range {}\n", block.range().repr());
    fmt::print(os, "; block-score {}\n", block.score());
    fmt::print(os, "  <BLOCK_{}>:\n", block.rowid());
    for (auto &inst : block.insts()) format(os, inst);
    return os;
}

namespace {

std::string invert_mpt(std::string raw) {
    for (size_t i = 0; i < raw.size() / 2; i += 2) {
        std::swap(raw[i], raw[raw.size() - i - 2]);
        std::swap(raw[i + 1], raw[raw.size() - i - 1]);
    }
    return raw;
}
}  // namespace

std::ostream &MptFormat::format(std::ostream &os, const Instruction &inst) {
    auto text = inst.text;
    std::replace(text.begin(), text.end(), '%', '$');
    // TODO Check arch
    fmt::print(os, "    0x{} ; {}\n", inst.raw_be(), text);
    return os;
}

std::ostream &MptFormat::format(std::ostream &os, const Path &path) {
    fmt::print(os, "; path-id {}\n", path.rowid());
    fmt::print(os, "; path-score {}\n", path.score());
    for (auto &block : path.nodes()) format(os, *block);
    return os;
}
