#!/bin/bash

csv_file=$(mktemp)
log_file=$(mktemp)
machine=$(uname -m)
header="Cycles, Time Elapsed (us), Retired Instructions, Retired Memory Instructions, Data Cache Misses, Instructions Per Cycle, Miss Percentage"

cleanup() {
    rm "$csv_file" "$log_file"
}

if [ "$machine" = "riscv64" ]; then
    marks_cmd=chop-marks-riscv
elif [ "$machine" = "ppc64le" ]; then
    marks_cmd=chop-marks-ppc64
elif [ "$machine" = "s390x" ]; then
    marks_cmd=chop-marks-sysz
else
    echo "Unsupported architecture: $machine"
    cleanup
    exit 1
fi

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
