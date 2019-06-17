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
 * NAME        : core/instruction.h
 * DESCRIPTION : Simple struct to save an instruction
 ******************************************************************************/

// TODO Change to class/shared_ptr for consistency?

#pragma once

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "branch.h"
#include "types.h"

#include "traits.h"

#include "database/connection.h"
#include "database/query.h"
#include "database/record.h"

namespace chopstix {

struct Instruction : public trait_rowid,
                     public trait_count,
                     public trait_score {
    friend class Record;
    using branch_ptr = std::shared_ptr<Branch>;
    using opcode_type = long;

    addr_type addr;
    std::string raw;
    std::string text;
    branch_ptr branch = nullptr;

    std::vector<int> bytes() const;
    opcode_type opcode() const;
    std::string mnemonic() const;
    std::vector<std::string> operands() const;

    Instruction() = default;
    Instruction(addr_type addr, std::string raw, std::string text)
        : addr(addr), raw(raw), text(text) {}

    std::string repr() const;

    std::string raw_be() const;

    static Query list_by_block(Connection &db, long block_id);

    std::istream &parse_line(std::istream &is);
};

}  // namespace chopstix
