#
# ----------------------------------------------------------------------------
#
# Copyright 2021 CHOPSTIX Authors
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

add_executable(chop-perf-invok
    src/breakpoint.c
    src/main.c
    src/sample.c
)

install (TARGETS
    chop-perf-invok
    DESTINATION bin
)

set(CMAKE_C_FLAGS_DEBUG "-O0 -no-pie -fno-pic")
set(CMAKE_C_FLAGS_RELEASE "-O0 -no-pie -fno-pic")
set(CMAKE_C_FLAGS_MINSIZEREL "-O0 -no-pie -fno-pic")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O0 -no-pie -fno-pic")

if (CHOPSTIX_BUILD_TESTS) 

    add_executable(vector_add test_programs/vector_add.c)

    if( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "riscv64" )
        add_executable(sandwich_return
            test_programs/sandwiched_return_riscv.s
            test_programs/sandwiched_return.c
        )
    elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "ppc64le" )
        add_executable(sandwich_return
            test_programs/sandwiched_return.c
            test_programs/sandwiched_return_ppc64.s
        )
    elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "s390x" )
        add_executable(sandwich_return
            test_programs/sandwiched_return.c
            test_programs/sandwiched_return_s390x.s
        )
    else ()
        message(STATUS "No perf-invok tests for architecture ${CMAKE_HOST_SYSTEM_PROCESSOR} available.")
    endif ()

    add_test(
        NAME test_generic_invocation
        COMMAND ${CMAKE_SOURCE_DIR}/tools/perf-invok/tests/test_vector_add.sh ${CMAKE_SOURCE_DIR}/tools
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tools/perf-invok
    )

    if (TARGET sandwich_return)
        add_test(
            NAME test_sandwich_return
            COMMAND ${CMAKE_SOURCE_DIR}/tools/perf-invok/tests/test_sandwich_return.sh ${CMAKE_SOURCE_DIR}/tools
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tools/perf-invok
        )
    endif()
endif()
