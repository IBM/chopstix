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


#
# Configuration options 
#
INSTALL_DIR=
OUTPUT_DIR=/tmp/trace_example
BASE_NAME=ubench_daxpy
MICROPROBE_TARGET=

# shellcheck disable=SC1091,SC3046
source "$INSTALL_DIR/share/chopstix/setup.sh"

set -e
set -x

# Compile test
make daxpy
# Test test
time ./daxpy
# Check we can get region of interest (ROI) addresses
chop-marks daxpy daxpy
# Trace ROI
# shellcheck disable=SC2046
chop trace $(chop-marks daxpy daxpy) -trace-dir "$OUTPUT_DIR/trace_data" ./daxpy
# Convert trace to MPT
chop-trace2mpt --trace-dir "$OUTPUT_DIR/trace_data" -o "$OUTPUT_DIR/$BASE_NAME"
# Convert MPT to runnable ELF
mp_mpt2elf -T "$MICROPROBE_TARGET" -t "$OUTPUT_DIR/$BASE_NAME#0.mpt" -O "$OUTPUT_DIR/$BASE_NAME#0.s" --safe-bin --raw-bin --fix-long-jump --compiler gcc --reset --wrap-endless --wrap-endless-threshold 1000
# Test generated ELF
set +e
timeout 10s "$OUTPUT_DIR/$BASE_NAME#0.elf"  
if [ $? -ne 124 ]; then
    echo "ELF not functional"
    exit 1
fi
set -e
# Trace memory accesses of generated ELF and create a new MPT with the memory
# accesse information
chop-trace-mem -output "$OUTPUT_DIR/$BASE_NAME#0" -base-mpt "$OUTPUT_DIR/$BASE_NAME#0.mpt" -output-mpt "$OUTPUT_DIR/$BASE_NAME#0#memory.mpt" -- "$OUTPUT_DIR/$BASE_NAME#0.elf"
# Convert the new MPT, with memory accesses, to runnable ELF
mp_mpt2elf -T "$MICROPROBE_TARGET" -t "$OUTPUT_DIR/$BASE_NAME#0#memory.mpt" -O "$OUTPUT_DIR/$BASE_NAME#0#memory.s" --safe-bin --raw-bin --fix-long-jump --compiler gcc --reset --wrap-endless --wrap-endless-threshold 1000
# Test generated ELF
set +e
timeout 10s "$OUTPUT_DIR/$BASE_NAME#0#memory.elf"  
if [ $? -ne 124 ]; then
    echo "ELF not functional"
    exit 1
fi
set -e
