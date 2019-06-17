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
 * NAME        : core/traits.h
 * DESCRIPTION : Common class traits
 ******************************************************************************/

#pragma once

#include "database/connection.h"
#include "database/query.h"

namespace chopstix {

struct trait_rowid {
    using rowid_type = long;

    trait_rowid(rowid_type rowid = 0) : rowid_(rowid) {}

    rowid_type rowid() const { return rowid_; }
    bool has_rowid() const { return rowid_ != 0; }

  protected:
    rowid_type rowid_ = 0;
};

struct trait_score {
    virtual double score() const { return score_; }

  protected:
    double score_ = 0.0;
};

struct trait_count {
    virtual long count() const { return count_; }

  protected:
    long count_ = 0;
};

}  // namespace chopstix
