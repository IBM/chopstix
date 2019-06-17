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
############################################################
# NAME        : test/CMakeLists.txt
# DESCRIPTION : Sources and scripts for testing
############################################################

message (STATUS "Generating tests")

set(traceinc "${CMAKE_SOURCE_DIR}/src/trace")
set(tracelib "cxtrace")

add_subdirectory(daxpy)

set (drivers "${CMAKE_CURRENT_SOURCE_DIR}/drivers")

macro(test_case driver test)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/testenv/${driver}/${test})
    add_test(NAME ${driver}:${test}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/testenv/${driver}/${test}
        COMMAND env test=${test}
                    testdir=${CMAKE_CURRENT_SOURCE_DIR}/${test}
                    testbin=$<TARGET_FILE:test-${test}>
                    driver=${driver}
                    chop=$<TARGET_FILE:chop>
                    cxtrace=$<TARGET_FILE:cxtrace>
                ${drivers}/${driver}
    )
endmacro()

test_case(disasm daxpy)
test_case(sample daxpy)
test_case(trace daxpy)
test_case(score daxpy)
