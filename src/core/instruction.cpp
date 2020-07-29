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
 * DESCRIPTION : Simple struct to save an instruction
 ******************************************************************************/

#include "instruction.h"

#include "sql/select_inst.h"

#include <sstream>

#include "fmt/format.h"

#include "support/stream.h"
#include "support/string.h"

#include "database/query.h"

namespace chopstix {

template <>
Instruction Record::get(int *i) {
    Instruction inst;
    inst.rowid_ = get<long>(i);
    inst.addr = get<addr_type>(i);
    inst.raw = get<std::string>(i);
    inst.text = get<std::string>(i);
    inst.count_ = get<long>(i);
    inst.score_ = get<double>(i);
    return inst;
}

template <>
Query &Query::bind(int *i, Instruction inst) {
    bind(i, inst.addr);
    bind(i, inst.raw);
    bind(i, inst.text);
    return *this;
}

}  // namespace chopstix

using namespace chopstix;

std::string Instruction::repr() const {
    return fmt::format("<Instruction {:x} {} '{}'>", addr, raw, text);
}

std::vector<int> Instruction::bytes() const {
    std::vector<int> bytes;
    for (size_t i = 0; i < raw.size(); i += 2) {
        auto byte = raw.substr(i, 2);
        bytes.push_back(std::stol(byte, nullptr, 16));
    }
    return bytes;
}

std::string Instruction::mnemonic() const {
    return string::split(text, " \t").first;
}

std::vector<std::string> Instruction::operands() const {
    auto ops = string::split(text, " \t").second;
    return string::splitg(ops, ",");
}

Instruction::opcode_type Instruction::opcode() const {
    opcode_type op;
    std::stringstream ss(raw);
    ss >> std::hex >> op;
    return op;
}

std::string Instruction::raw_be() const {
    std::string be{raw};
    for (size_t i = 0; i < raw.size(); i += 2) {
        be[i] = raw[raw.size() - 2 - i];
        be[i + 1] = raw[raw.size() - 1 - i];
    }
    return be;
}

std::istream &Instruction::parse_line(std::istream &is) {
    addr = 0;
    raw = "";
    text = "";

    // Read address
    is >> std::hex >> addr;
    is >> stream::expect(':') >> std::ws;
    if (is.fail()) return is;

    // Read raw bytes
    while (!(is.peek() == '\t' || is.peek() == ' ' || is.eof())) {
        std::string tmp;
        is >> std::ws >> tmp;
        raw += tmp;
        is.get();
    }
    if (is.fail()) return is;

    // Read mnemonic + operands (optional)
    if (!is.eof()) {
        is >> std::ws >> text;
        if (!is.eof()) {
            std::string ops;
            is >> std::ws >> ops;
            if (!ops.empty()) {
                text += " " + ops;
            }
        }
    }

    return is;
}

Query Instruction::list_by_block(Connection &db, long block_id) {
    auto q = db.query(SQL_SELECT_INST "WHERE block_id = ?;");
    q.bind(1, block_id);
    return q;
}
