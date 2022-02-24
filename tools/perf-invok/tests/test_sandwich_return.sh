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
log_stdout_file=$(mktemp)
log_stderr_file=$(mktemp)
machine=$(uname -m)

cleanup() {
    rm "$csv_file" "$log_stdout_file" "$log_stderr_file"
}

marks_cmd=chop-marks
if [ "$machine" = "riscv64" ]; then
    return_instr="8082"
    return_instr_len=4
elif [ "$machine" = "ppc64le" ]; then
    return_instr="4e800020"
    return_instr_len=8
elif [ "$machine" = "s390x" ]; then
    return_instr="07fe"
    return_instr_len=4
else
    echo "Unsupported architecture: $machine"
    cleanup
    exit 1
fi

string_offset=$((8 - return_instr_len))
string_length=$((8 + return_instr_len))

mkbreakpoint() {
    # shellcheck disable=SC2034
    for i in $(seq 1 "$1"); do printf "0"; done
}

string_breakpoint=$(mkbreakpoint $return_instr_len)

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

echo "Testing accurate breakpoint positioning by placing a breakpoint sandwiched in between function code. Only the return instruction should be \"occluded\" by the breakpoint. The instructions before and after should be O.K."

if ! ./sandwich_return 1> "$log_stdout_file"; then
    echo "Couldn't execute test program"
    echo "logs:"
    cat "$log_stdout_file"
    cleanup
    exit 1
fi

echo "Binary contents of the test program are OK."

output=$(cat "$log_stdout_file")

if [ "$machine" = "s390x" ]; then
    if [ "${output}" != "${return_instr}deadbeefdead" ]; then
        # "deadbeef" is the hard-coded sentinel in the binary
        # Because there is no breakpoint, the instruction appears in the binary dump
        echo "Unexpected binary contents in the test program. Did something from the test bench break?"
        echo "Expected: ${return_instr}deadbeefdead"
        echo "Actual  : ${output}"
        cleanup
        exit 1
    fi
else
    string_offset=$((string_offset+1))
    echo "cut -c $string_offset-$((string_offset+string_length))"
    if [ "$(echo "${output}" | cut -c $string_offset-$((string_offset+string_length)))" != "deadbeef$return_instr" ]; then
        # "deadbeef" is the hard-coded sentinel in the binary
        # Because there is no breakpoint, the instruction appears in the binary dump
        echo "Unexpected binary contents in the test program. Did something from the test bench break?"
        echo "Expected: deadbeef$return_instr"
        echo "Actual  : $(echo "${output}" | cut -c $string_offset-$((string_offset+string_length)))" 
        cleanup
        exit 1
    fi
fi

# shellcheck disable=SC2046
timeout 5 ./chop-perf-invok -o "$csv_file" $("$marks_cmd" ./sandwich_return sandwich_return) -- ./sandwich_return 2> "$log_stderr_file" 1> "$log_stdout_file"

ret_val=$?
if [ $ret_val -ne 0 ]; then
    if [ $ret_val -eq 124 ]; then
        echo "chop-perf-invok timed out (5 seconds). This probably means it got stuck in an infinite loop."
    else
        echo "chop-perf-invok returned an error exit code"
    fi
    echo "logs:"
    cat "$log_stderr_file"
    if [ "$(grep -c "Permission denied" "$log_stderr_file")" -ne 0 ]; then
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
    cat "$log_stderr_file"
    cleanup
    exit 1
fi

row_count=$(wc -l "$csv_file" | cut -d' ' -f 1)

if [ "$row_count" -ne 2 ]; then
    echo "Output file contains unexpected number of rows (Expected 2, got $row_count)"
    echo "Logs:"
    echo "$log_stderr_file"
    cleanup
    exit 1
fi

echo "Row count OK."

output=$(cat "$log_stdout_file")

if [ "$machine" = "s390x" ]; then
    if [ "${output}" != "${string_breakpoint}deadbeefdead" ]; then
        # Because this time the binary was run with a breakpoint in place, the
        # return instruction isn't present anymore and instead we find a bunch of
        # zeroes (the breakpoint). 0xdeadbeef should still be present. If it isn't,
        # that means the program incorrectly sets breakpoints to the code further
        # below the instruction where the breakpoint should actually be.
        # NOTE: This test probably only works on little-endian architectures?

        echo "The breakpoint encompases more than the targetted return instruction."
        echo "This could cause unexpected problems when, within the same function,"
        echo "there are instructions to be executed below the return instruction."
        echo "Expected: ${string_breakpoint}deadbeefdead"
        echo "Actual: ${output}"
        cleanup
        exit 1
    fi
else
    if [ "$(echo "${output}" | cut -c $string_offset-$((string_offset+string_length)))" != "deadbeef$string_breakpoint" ]; then
        # Because this time the binary was run with a breakpoint in place, the
        # return instruction isn't present anymore and instead we find a bunch of
        # zeroes (the breakpoint). 0xdeadbeef should still be present. If it isn't,
        # that means the program incorrectly sets breakpoints to the code further
        # below the instruction where the breakpoint should actually be.
        # NOTE: This test probably only works on little-endian architectures?

        echo "The breakpoint encompases more than the targetted return instruction."
        echo "This could cause unexpected problems when, within the same function,"
        echo "there are instructions to be executed below the return instruction."
        echo "Expected: deadbeef$string_breakpoint"
        echo "Actual  : $(echo "${output}" | cut -c $string_offset-$((string_offset+string_length)))"
        cleanup
        exit 1
    fi
fi

echo "Breakpoint positioning OK."

echo "Test passed."

cleanup

