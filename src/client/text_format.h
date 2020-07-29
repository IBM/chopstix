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
 * NAME        : client/text_format.h
 * DESCRIPTION : Implements various formatters for assembly
 *               instructions.
 ***********************************************************/

#pragma once

#include <ostream>

#include "core/module.h"
#include "core/path.h"

namespace chopstix {

struct TextFormat {
    virtual std::ostream &header(std::ostream &os);
    virtual std::ostream &format(std::ostream &os, const Module &module);
    virtual std::ostream &format(std::ostream &os, const Function &func);
    virtual std::ostream &format(std::ostream &os, const BasicBlock &block);
    virtual std::ostream &format(std::ostream &os, const Instruction &inst);
    virtual std::ostream &format(std::ostream &os, const Path &path);
};

struct AnnotFormat : TextFormat {
    virtual std::ostream &format(std::ostream &os, const Module &module);
    virtual std::ostream &format(std::ostream &os, const Function &func);
    virtual std::ostream &format(std::ostream &os, const BasicBlock &block);
    virtual std::ostream &format(std::ostream &os, const Instruction &inst);
    virtual std::ostream &format(std::ostream &os, const Path &path);
};

struct MptFormat : TextFormat {
    MptFormat(Endianess endianess) : endianess(endianess) {}

    virtual std::ostream &header(std::ostream &os);
    virtual std::ostream &format(std::ostream &os, const Module &module);
    virtual std::ostream &format(std::ostream &os, const Function &func);
    virtual std::ostream &format(std::ostream &os, const BasicBlock &block);
    virtual std::ostream &format(std::ostream &os, const Instruction &inst);
    virtual std::ostream &format(std::ostream &os, const Path &path);

private:
    Endianess endianess;
};

};  // namespace chopstix
