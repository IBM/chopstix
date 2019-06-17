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

set -eu

get_value() { awk -v key="$2" '$2 == key{print $3}' "output.$1" ; }

export TIMEOUT=10s

check_trace() {
    num_pages=$(get_value "$1" num_pages)
    num_iter=$(get_value "$1" num_iter)
    num_access=$(grep -ce 'access' cxtrace.log)
    min_access=$((num_pages * 2 * num_iter))

    test "$num_access" -ge "$min_access" || \
        die "Error: less than $min_access pages accessed ($num_access)"

    vec_x=$(get_value "$1" vec_x)
    vec_y=$(get_value "$1" vec_y)

    count_x=$(grep -ce "access $vec_x" cxtrace.log)
    count_y=$(grep -ce "access $vec_y" cxtrace.log)

    test "$count_x" -eq "$num_iter" || \
        die "Error: vector x should be accessed $num_iter times (accessed $count_x)"

    test "$count_y" -eq "$num_iter" || \
        die "Error: vector y should be accessed $num_iter times (accessed $count_y)"
}

check_content() {
    num_pages=$(get_value "$1" num_pages)
    num_pages=$(( num_pages * 2 ))
    num_iter=$(get_value "$1" num_iter)

    it=0;
    while [ "$it" -lt "$num_iter" ]; do
        it=$((it+1))
        # shellcheck disable=SC2012
        count=$(ls cxtrace.data/page.$it.* | wc -l)
        test "$count" -ge $num_pages || \
            die "Error: expected $num_pages pages for iteration $it (found $count)"
        test -f cxtrace.data/maps.$it || \
            die "Error: no maps file for iteration $it"
    done
}

test_daxpy() {
    export CHOPSTIX_OPT_LOG_PATH=cxtrace.log
    export CHOPSTIX_OPT_SAVE_CONTENT=no
    export CHOPSTIX_OPT_WITH_PERM='rw-,r*x'

    test_run normal "$1" iter "$2"

    test_run trace-quiet "$1" inst "$2"
    validate_output normal trace-quiet
    check_trace trace-quiet
    mv cxtrace.log cxtrace.trace-quiet

    export CHOPSTIX_OPT_SAVE_CONTENT=yes
    export CHOPSTIX_OPT_TRACE_PATH=cxtrace.data

    rm -rf cxtrace.data
    test_run trace-save "$1" inst "$2"
    validate_output normal trace-save
    check_trace trace-save
    check_content trace-save
    mv cxtrace.log cxtrace.trace-save

    export CHOPSTIX_OPT_SAVE_CONTENT=no

    test_trace daxpy "$1" iter "$2"
    validate_output normal trace-daxpy
    check_trace trace-daxpy
    mv cxtrace.log cxtrace.trace-daxpy
}

test_daxpy 100 1
test_daxpy 100 2
