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
    "$@"
    printf ">>>> end command\\n"
}

chop -version >/dev/null || die "ChopStiX not setup correctly"

rm -f chop.db daxpy.1.mpt

echo "########################################################################"
echo "# Start by sampling an invocation of daxpy"
echo "########################################################################"
tryme chop sample -events task-clock ./daxpy

echo "########################################################################"
echo "# List sessions" 
echo "########################################################################"
tryme chop list sessions

# echo "########################################################################"
# echo "# List samples" 
# echo "########################################################################"
# tryme chop list samples 

echo "########################################################################"
echo "# Build the control flow graph (CFG) for daxpy"
echo "########################################################################"
tryme chop disasm ./daxpy

echo "########################################################################"
echo "# Show the text for daxpy"
echo "########################################################################"
tryme chop text function -name daxpy

echo "########################################################################"
echo "# Group/count samples" 
echo "########################################################################"
tryme chop count

echo "########################################################################"
echo "# Annotate the CFG"
echo "########################################################################"
tryme chop annotate

echo "########################################################################"
echo "# Show the annotated text for daxpy"
echo "########################################################################"
tryme chop text function -name daxpy -fmt annotate

echo "########################################################################"
echo "# Search snippet paths"
echo "########################################################################"
tryme chop search -target-coverage 90%

echo "########################################################################"
echo "# List snippets"
echo "########################################################################"
tryme chop list paths 

echo "########################################################################"
echo "# Generate mpt for Microprobe in stdout"
echo "########################################################################"
tryme chop text path -id 1 -fmt mpt

echo "########################################################################"
echo "# Generate mpt for Microprobe in a file"
echo "########################################################################"
tryme chop text path -id 1 -fmt mpt -out daxpy.1.mpt
