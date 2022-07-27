#!/usr/bin/env sh
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

set -e

die() {
    echo "$*" >&2
    exit 1
}

tryme() {
    printf ">>>> command: > %s\\n(press return to continue)" "$*"
    read -r _ 
    printf ">>>> start command\\n"
    # shellcheck disable=SC2294
    eval "$@"
    printf ">>>> end command\\n"
    printf ">>>> (press return to continue)" 
    read -r _ 
}

chop -version >/dev/null || die "ChopStiX not setup correctly"

rm -f chop.db daxpy.1.mpt cluster.json profile.csv

clear
echo "########################################################################"
echo "# Start by sampling an invocation of daxpy, building the CFG, "
echo "# and linking the samples to the instructions."
echo "########################################################################"
tryme chop sample -events task-clock ./daxpy
tryme chop disasm ./daxpy
tryme chop count
tryme chop annotate

clear
echo "########################################################################"
echo "# Show the list of functions and their score (# of samples)" 
echo "#"
echo "# Function daxpy is the hottest one"
echo "########################################################################"
tryme "chop list functions | column -t"

clear
echo "########################################################################"
echo "# Profile the function daxpy from daxpy"
echo "#"
echo "# 1. First get the -begin/-end points of the region of interest"
echo "########################################################################"
tryme "chop-marks daxpy daxpy"

echo "########################################################################"
echo "# 2. Then, profile the function execution (in profile.csv)"
echo "########################################################################"
# shellcheck disable=SC2005,SC2046
tryme "chop-perf-invok -o profile.csv $(echo $(chop-marks ./daxpy daxpy)) -max 1000000 -- ./daxpy"

echo "########################################################################"
echo "# 3. Show first 20 invocations"
echo "########################################################################"
tryme "head -n 21 < profile.csv | column -s, -t"

clear
echo "########################################################################"
echo "# Show information on the generated profile"
echo "########################################################################"
tryme "cti_trace_info -t ipc profile.csv | column -t -s : -o :"

clear
echo "########################################################################"
echo "# Perform clustering analysis on the profiled function to know which"
echo "# invocations to extract"
echo "# "
echo "# - A cluster.json file will be generated"
echo "# - A JPG file will be generated to visualize the clusters"
echo "########################################################################"
tryme "cti_cluster instr_ipc_density --max-clusters 20 --output clusters.json --benchmark-name DAXPY --function-name daxpy --plot-path plot.daxpy profile.csv"

clear
echo "########################################################################"
echo "# Print cluster information"
echo "########################################################################"
tryme "cti_cluster_info summary clusters.json"

echo "########################################################################"
echo "# Get representative invocation of cluster 0"
echo "########################################################################"
tryme "cti_cluster_info representative --cluster 0 clusters.json"
