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
    eval "$@"
    printf ">>>> end command\\n"
    printf ">>>> (press return to continue)" 
    read -r _ 
}

chop -version >/dev/null || die "ChopStiX not setup correctly"

rm -f chop.db daxpy.1.mpt

clear
echo "########################################################################"
echo "# Start by sampling an invocation of daxpy"
echo "########################################################################"
tryme chop sample -events task-clock ./daxpy

clear
echo "########################################################################"
echo "# List sampling sessions"
echo "########################################################################"
tryme chop list sessions

# clear
# echo "########################################################################"
# echo "# List samples" 
# echo "########################################################################"
# tryme chop list samples 

clear
echo "########################################################################"
echo "# Build the control flow graph (CFG) for daxpy program"
echo "########################################################################"
tryme chop disasm ./daxpy

clear
echo "########################################################################"
echo "# List modules (main binary, and any shared library)" 
echo "########################################################################"
tryme chop list modules

clear
echo "########################################################################"
echo "# Show the text (code) for daxpy function"
echo "########################################################################"
tryme chop text function -name daxpy

clear
echo "########################################################################"
echo "# Group/count samples" 
echo "########################################################################"
tryme chop count

clear
echo "########################################################################"
echo "# Annotate the CFG (link the samples to the instructions)"
echo "########################################################################"
tryme chop annotate

clear
echo "########################################################################"
echo "# Show the annotated text for daxpy (output similar to perf)"
echo "########################################################################"
tryme chop text function -name daxpy -fmt annotate

clear
echo "########################################################################"
echo "# Show the list of functions and their score (# of samples)" 
echo "########################################################################"
tryme "chop list functions | column -t"

clear
echo "########################################################################"
echo "# Show the top 10 basic blocks and their score (# of samples)" 
echo "########################################################################"
tryme "chop list blocks | column -t | head -n 11"


