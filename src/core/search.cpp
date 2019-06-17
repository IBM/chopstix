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
 * NAME        : core/search.cpp
 * DESCRIPTION : Configurable implementation of search algorithm
 ******************************************************************************/

#include "search.h"

#include "support/check.h"
#include "support/log.h"

#include "fmt/printf.h"

using namespace chopstix;

void Search::finish() {
    while (next())
        ;
}

bool Search::complete() const {
    return coverage_.complete() || count_.complete();
}

void Search::add_backedges(const edge_vec &edges) {
    for (auto &edge : edges) queue_.push(Path(edge.from(), edge.to()));
}

bool Search::found_path(const Path &path) const {
    return find(paths_.begin(), paths_.end(), path) != paths_.end();
}

void Search::score_path(Path path) {
    double score = 0.0;
    for (auto &node : path) score += node->score();
    path.update_score(score);
    paths_.push_back(path);
    count_ += 1;
    for (auto &node : path.nodes()) {
        auto ret = scored_.insert(node->rowid());
        if (ret.second) {
            coverage_ += node->score();
            score_ += node->score();
        }
        covered_[node->rowid()] += 1;
    }
    log::verbose("Found path: %s (score: %d)", path.repr(), (long)path.score());
}

double Search::get_score(block_ptr block) {
    auto num_reps = covered_[block->rowid()];
    return (num_reps > heur_reps_) ? 0 : block->score();
}

void Search::expand_path(const Path &original, const block_ptr &block) {
    Path path = original;
    path.add(block);
    path.update_score(heur(path));
    if (path.trend(heur_ins_) < cutoff_) return;
    queue_.push(path);
    log::verbose("Add path: %s (trend: %d)", path.repr(),
                 (long)path.trend(heur_ins_));
}

double Search::heur(const Path &path) {
    double score = 0;
    for (auto &node : path.nodes()) {
        score += get_score(node);
    }
    return score;
}

bool Search::next() {
    if (queue_.empty() || complete()) return false;

    auto path = queue_.top();
    queue_.pop();

    log::verbose("Current path: %s (trend: %d)", path.repr(),
                 (long)path.trend(heur_ins_));

    auto u = path.nodes().back();
    visited_[u->rowid()] += 1;

    if (u == path.target()) {
        if (!found_path(path)) score_path(path);
    } else if (visited_[u->rowid()] <= count_.max()) {
        for (auto v : u->next()) {
            auto vptr = v.lock();
            checkx(vptr != nullptr, "Lifetime of basic block expired");
            if (!path.contains(vptr)) expand_path(path, vptr);
        }
    }

    return true;
}

long Search::num_blocks() const {
    std::vector<long> block_ids;
    block_ids.reserve(paths_.size() * paths_.front().nodes().size());
    for (auto &path : paths_) {
        for (auto &block : path.nodes()) {
            block_ids.push_back(block->rowid());
        }
    }
    auto unique = std::unique(block_ids.begin(), block_ids.end());
    return unique - block_ids.begin();
}

const Progress &Search::progress() const { return std::max(coverage_, count_); }
