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
 * NAME        : core/search.h
 * DESCRIPTION : Configurable implementation of search algorithm
 ******************************************************************************/

#pragma once

#include <limits>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include "support/progress.h"

#include "path.h"
#include "traits.h"

namespace chopstix {

class Search : public trait_score {
  public:
    using node_id = long;
    using block_ptr = std::shared_ptr<BasicBlock>;
    using node_set = std::set<node_id>;
    using node_count = std::map<node_id, long>;
    using path_vec = std::vector<Path>;
    using path_queue = std::priority_queue<Path, path_vec>;
    using edge_vec = std::vector<Edge>;

    Search() : coverage_(Progress::infty()), count_(Progress::infty()) {}

    void add_backedges(const edge_vec &edges);
    void target_coverage(double cov) { coverage_.set_max(cov); }
    void target_count(long count) { count_.set_max(count); }
    void heur_reps(long reps) { heur_reps_ = std::max(reps, 1l); }
    void heur_ins(long ins) { heur_ins_ = std::max(ins, 1l); }

    bool next();
    void finish();
    bool complete() const;

    const Progress &progress() const;
    const path_vec &paths() const { return paths_; }

    double coverage() const { return coverage_.count(); }
    long count() const { return count_.count(); }
    long num_blocks() const;

    double heur(const Path &);

    void score_path(Path);
    bool found_path(const Path &) const;
    void expand_path(const Path &, const block_ptr &);

    double get_score(block_ptr);  // TODO mark as const

    void set_cutoff(double cutoff) { cutoff_ = cutoff; }

  private:
    node_count visited_;
    node_count covered_;
    node_set scored_;

    path_queue queue_;
    path_vec paths_;

    double cutoff_ = 0;

    long heur_ins_ = std::numeric_limits<long>::max();
    long heur_reps_ = std::numeric_limits<long>::max();

    Progress coverage_;
    Progress count_;
};

}  // namespace chopstix
