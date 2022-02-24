#!/usr/bin/env sh
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 IBM Corporation
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

csv_file=$(mktemp)
log_file=$(mktemp)
header="Cycles, Time Elapsed (us), Retired Instructions, Retired Memory Instructions, Data Cache Misses, Instructions Per Cycle, Miss Percentage"

cleanup() {
    rm "$csv_file" "$log_file"
}

marks_cmd=chop-marks

if [ $# -ne 0 ]; then
    # First argument is the root of the ChopStiX/tools folder, containing the
    # necessary scripts to run these tests
    marks_cmd=$1/$marks_cmd
fi

if ! hash "$marks_cmd" > /dev/null 2> /dev/null; then
    echo "ChopStiX marks command ($marks_cmd) not found"
    cleanup
    exit 1
fi

echo "Testing basic functionality using a function with two distinct alternate behaviours (this might take a while)..."

# shellcheck disable=SC2046
if ! ./chop-perf-invok -o "$csv_file" $("$marks_cmd" ./vector_add add) -- ./vector_add > "$log_file" 2> "$log_file" ; then
    echo "chop-perf-invok returned an error exit code"
    echo "logs:"
    cat "$log_file"
    if [ "$(grep -c "Permission denied" "$log_file")" -ne 0 ]; then
        echo "Exit code due to the lack of permissions"
        cleanup
        exit 0
    fi
    cleanup
    exit 1
fi

echo "Return code OK."

if [ ! -f "$csv_file" ]; then
    echo "chop-perf-invok didn't create an output file"
    echo "logs:"
    cat "$log_file"
    cleanup
    exit 1
fi

row_count=$(wc -l "$csv_file" | cut -d' ' -f 1)

if [ "$row_count" -ne 11 ]; then
    echo "Output file contains unexpected number of rows (Expected 11, got $row_count)"
    echo "Logs:"
    echo "$log_file"
    cleanup
    exit 1
fi

echo "Row count OK."

if [ "$(head -n 1 "$csv_file")" != "$header" ]; then
    echo "Output file has an incorrect header"
    echo "Expected:"
    echo "$header"
    echo "Actual:"
    head -n 1 "$csv_file"
    echo "Logs:"
    echo "$log_file"
    cleanup
    exit 1
fi

echo "Output file header OK."

odd_invocation=$(tail -n 1 "$csv_file" | cut -d',' -f 3)
even_invocation=$(tail -n 2 "$csv_file" | head -n 1 | cut -d',' -f 3)

if [ "$(echo "$even_invocation * 100 > $odd_invocation * 0.95 && $even_invocation * 100 < $odd_invocation * 1.05" | bc -l)" -ne 1 ] ; then
    echo "Expected odd invocations to have approximately 100 times (+- 5%) instructions than even invocations"
    echo "Instructions in odd invocations: $odd_invocation"
    echo "Instructions in even invocations: $even_invocation"
    echo "Logs:"
    echo "$log_file"
    cleanup
    exit 1
fi

echo "Retired instruction counts OK."

echo "Test passed."

cleanup
