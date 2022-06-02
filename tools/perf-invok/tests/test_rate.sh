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

echo "Testing with a rate of 10Hz and a timeout of 10s"
echo "Command: ./chop-perf-invok -o $csv_file -timeout 10 -rate 10.0 -- ./timed_vector_add"
# shellcheck disable=SC2046
if ! ./chop-perf-invok -o "$csv_file" -timeout 10 -rate 10.0 -- ./timed_vector_add > "$log_file" 2> "$log_file" ; then
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

if [ "$row_count" -lt 101 ] || [ "$row_count" -gt 102 ]; then
    # There should be 100 or 101 entries in the CSV plus the header. The range
    # is because it is impossible wether the timeout timer or the rate timer
    # will fire first, thus this test is allowing for an extra sample to be
    # taken.

    echo "Output file contains unexpected number of rows (Expected to be in range [101,102] , got $row_count)"
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
