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

export TIMEOUT=300s

check_trace() {
    echo "> check trace"
    num_pages=$(get_value "$1" num_pages)
    # echo num_pages $num_pages
    num_iter=$(get_value "$1" num_iter)
    # echo num_iter $num_iter
    num_access=$(grep -ce 'System::record_segv: segv' cxtrace.log)
    # echo num_access $num_access
    min_access=$((num_pages * 2 * num_iter))
    # echo min_access $min_access

    test "$num_access" -ge "$min_access" || \
        die "Error: less than $min_access pages accessed ($num_access)"

    vec_x=$(get_value "$1" vec_x)
    # echo vec_x $vec_x
    vec_y=$(get_value "$1" vec_y)
    # echo vec_y $vec_y

    count_x=$(grep -ce "segv at $vec_x" cxtrace.log)
    # echo count_x $count_x
    count_y=$(grep -ce "segv at $vec_y" cxtrace.log)
    # echo count_y $count_y

    test "$count_x" -eq "$num_iter" || \
        echo "> Warning: vector x should be accessed $num_iter times (accessed $count_x)"

    test "$count_y" -eq "$num_iter" || \
        echo "> Warning: vector y should be accessed $num_iter times (accessed $count_y)"

    echo "> check trace ok"
}

check_content() {
    num_pages=$(get_value "$1" num_pages)
    # echo "num_pages $num_pages"
    num_pages=$(( num_pages * 2 ))
    # echo "num_pages $num_pages"
    num_iter=$(get_value "$1" num_iter)
    # echo "num_iter $num_iter"

    it=0;
    while [ "$it" -lt "$num_iter" ]; do
        # shellcheck disable=SC2012
        count=$(ls "$CHOPSTIX_OPT_TRACE_DIR"/page.$it.* | wc -l)
        test "$count" -ge $num_pages || \
            die "Error: expected $num_pages pages for iteration $it (found $count)"
        test -f "$CHOPSTIX_OPT_TRACE_DIR/maps.$it" || \
            die "Error: no maps file for iteration $it"
        test -f "$CHOPSTIX_OPT_TRACE_DIR/info.$it" || \
            die "Error: no info file for iteration $it"
        test -f "$CHOPSTIX_OPT_TRACE_DIR/regs.$it" || \
            die "Error: no regs file for iteration $it"
        it=$((it+1))
    done
    echo "> check content ok"
}

test_daxpy() {
    export CHOPSTIX_OPT_LOG_PATH=cxtrace.log
    export CHOPSTIX_OPT_LOG_LEVEL=verbose
    export CHOPSTIX_OPT_TRACE_DIR=cxtrace_data


    # Generate reference output
    test_run normal "$1" iter "$2"

    # Test default
    echo "> test default"
    name=trace-default
    test_trace_function func_daxpy $name "$1" iter "$2"
    validate_output normal $name
    check_trace $name
    check_content $name
    mv cxtrace.log cxtrace.$name

    # Test no dump pages
    echo "> test no dump pages"
    export CHOPSTIX_OPT_SAVE=no
    export CHOPSTIX_OPT_CODE=no
    export CHOPSTIX_OPT_REGISTERS=no
    export CHOPSTIX_OPT_MAPS=no
    export CHOPSTIX_OPT_INFO=no
    name=trace-nosave
    test_trace_function func_daxpy $name "$1" iter "$2"

    validate_output normal $name
    check_trace $name
    mv cxtrace.log cxtrace.$name
    
    # Test no save
    echo "> test no save"
    unset CHOPSTIX_OPT_SAVE
    unset CHOPSTIX_OPT_CODE
    unset CHOPSTIX_OPT_REGISTERS
    unset CHOPSTIX_OPT_MAPS
    unset CHOPSTIX_OPT_INFO
    export CHOPSTIX_OPT_TRACE=no
    name=trace-notrace
    test_trace_function func_daxpy $name "$1" iter "$2"
    validate_output normal $name
    mv cxtrace.log cxtrace.$name

    unset CHOPSTIX_OPT_TRACE
}

machine="$(uname -m)"
if [ "$machine" = "x86_64" ]; then
    exit 0
fi

test_daxpy 10000 1
test_daxpy 1000 10
test_daxpy 100 100
test_daxpy 10 1000
