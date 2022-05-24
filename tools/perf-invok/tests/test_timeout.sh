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

echo "Testing timeout under the program's typical run time (timeout: 5s, program run time: > 10s)"
echo "Command: ./chop-perf-invok -o $csv_file -timeout 5 -- ./timed_vector_add"
# shellcheck disable=SC2046
if ! ./chop-perf-invok -o "$csv_file" -timeout 5 -- ./timed_vector_add > "$log_file" 2> "$log_file" ; then
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

row_count=$(wc -l "$csv_file" | cut -d' ' -f 2)

if [ "$row_count" -ne 1 ]; then
    echo "Output file contains unexpected number of rows (Expected 1, got $row_count)"
    echo "Logs:"
    cat "$log_file"
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
    cat "$log_file"
    cleanup
    exit 1
fi

echo "Output file header OK."

echo "Test passed."

cleanup
