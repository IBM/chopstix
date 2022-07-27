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

OUTPUT_DIR="./output_data"
BASE_NAME=ubench_daxpy

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

flags=""
if [ "$(uname -m)" = "s390x" ]; then
    target="z16-z16-z64_linux_gcc"
    flags="--compiler-flags='-march=arch13'"
else
    die "Unable to detect target for $(uname -m)"
fi

MICROPROBE_TARGET="$target"

chop -version >/dev/null || die "ChopStiX not setup correctly"

rm -fr chop.db daxpy.1.mpt cluster.json profile.csv $OUTPUT_DIR

clear
echo "########################################################################"
echo "# Execute all the steps to find the hot function and the representative"
echo "# invocation of the hot function to trace."
echo "########################################################################"
tryme chop sample -events task-clock ./daxpy
tryme chop disasm ./daxpy
tryme chop count
tryme chop annotate
tryme "chop-perf-invok -o profile.csv $(echo $(chop-marks ./daxpy daxpy)) -max 1000000 -- ./daxpy"
tryme "cti_cluster instr_ipc_density --max-clusters 20 --output clusters.json --benchmark-name DAXPY --function-name daxpy --plot-path plot.daxpy profile.csv"
tryme "cti_cluster_info representative --cluster 0 clusters.json"

clear
echo "########################################################################"
echo "# Trace the representative invocation of the hottest function"
echo "########################################################################"
tryme "chop trace $(echo $(chop-marks daxpy daxpy)) -indices $(cti_cluster_info representative --cluster 0 clusters.json) -trace-dir $OUTPUT_DIR/trace_data -max-traces 1 ./daxpy"

echo "########################################################################"
echo "# Convert the RAW trace into an MPT"
echo "########################################################################"
tryme chop-trace2mpt --trace-dir "$OUTPUT_DIR/trace_data" -o "$OUTPUT_DIR/$BASE_NAME"
tryme ls -l "$OUTPUT_DIR/$BASE_NAME"*

clear
echo "########################################################################"
echo "# Convert MPT to runnable ELF"
echo "########################################################################"
tryme mp_mpt2elf -T "$MICROPROBE_TARGET" -t "$OUTPUT_DIR/$BASE_NAME#0.mpt" -O "$OUTPUT_DIR/$BASE_NAME#0.s" --safe-bin --raw-bin --fix-long-jump --compiler gcc --reset --wrap-endless --wrap-endless-threshold 1000 $flags

echo "########################################################################"
echo "# Test generated ELF in $OUTPUT_DIR/$BASE_NAME#0.elf"
echo "########################################################################"
set +e
timeout 10s "$OUTPUT_DIR/$BASE_NAME#0.elf"  
if [ $? -ne 124 ]; then
    echo "ELF not functional"
    exit 1
fi
set -e

clear
echo "########################################################################"
echo "# Trace memory accesses of generated ELF and create a new MPT with the memory"
echo "# accesse information"
echo "########################################################################"
tryme chop-trace-mem -output "$OUTPUT_DIR/$BASE_NAME#0" -base-mpt "$OUTPUT_DIR/$BASE_NAME#0.mpt" -output-mpt "$OUTPUT_DIR/$BASE_NAME#0#memory.mpt" -- "$OUTPUT_DIR/$BASE_NAME#0.elf"

echo "########################################################################"
echo "# Convert the new MPT, with memory accesses, to runnable ELF"
echo "########################################################################"
tryme mp_mpt2elf -T "$MICROPROBE_TARGET" -t "$OUTPUT_DIR/$BASE_NAME#0#memory.mpt" -O "$OUTPUT_DIR/$BASE_NAME#0#memory.s" --safe-bin --raw-bin --fix-long-jump --compiler gcc --reset --wrap-endless --wrap-endless-threshold 1000 $flags

echo "########################################################################"
echo "# Test generated ELF in $OUTPUT_DIR/$BASE_NAME#0#memory.elf"
echo "########################################################################"
set +e
timeout 10s "$OUTPUT_DIR/$BASE_NAME#0#memory.elf"  
if [ $? -ne 124 ]; then
    echo "ELF not functional"
    exit 1
fi
set -e
