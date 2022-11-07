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

if [ "$(uname -m)" = "s390x" ]; then
    target="z16-z16-z64_linux_gcc"
else
    die "Unable to detect target for $(uname -m)"
fi

rm -f chop.db daxpy.1.mpt daxpy_snippet.c

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
echo "# Search snippet paths. The command below searches for basic block"
echo "# execution paths accounting for 90% of total sampling coverage"
echo "########################################################################"
tryme chop search -target-coverage 90%

clear
echo "########################################################################"
echo "# List snippets (i.e. execution paths) found"
echo "########################################################################"
tryme chop list paths
echo "########################################################################"
echo "# Show the top 10 basic blocks and their score (# of samples)"
echo "########################################################################"
tryme "chop list blocks | column -t | head -n 11"

clear
echo "########################################################################"
echo "# Generate an MPT of the execution snippet for Microprobe in stdout"
echo "########################################################################"
tryme chop text path -id 1 -fmt mpt -arch "$(uname -m)"

clear
echo "########################################################################"
echo "# Generate an MPT of the execution snippet for Microprobe in a file"
echo "########################################################################"
tryme chop text path -id 1 -fmt mpt -out daxpy.1.mpt -arch "$(uname -m)"
tryme ls -l

clear
echo "########################################################################"
echo "# Generate ELF binary from the MPT path."
echo "#"
echo "# Note that this is an execution snippet, we are just reproducing the"
echo "# instruction execution sequence (not the addresses nor the data). "
echo "# Therefore fixes are required for the snippet to be functionally "
echo "# correct. Still, the program may fail."
echo "########################################################################"
tryme "mp_mpt2test -T $target -t daxpy.1.mpt -O daxpy_snippet.c --fix-indirect-branches  --fix-branch-next --fix-memory-references --fix-memory-registers --endless"
tryme "gcc daxpy_snippet.c -o daxpy_snippet"
tryme "timeout 10s ./daxpy_snippet"


