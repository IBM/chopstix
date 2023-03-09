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

add_executable(chop-detrace
    detrace.c
)

add_executable(chop-detrace-mem
    detrace-mem.c
)

add_library(chop-marks-dyn-addr-lib SHARED chop-marks-dyn-addr-lib.c)
target_link_libraries(chop-marks-dyn-addr-lib dl)

# Custom zlib
if (EXISTS "${CHOPSTIX_ZLIB_PREFIX}")
    include (CheckZlib)
    message (STATUS "Using zlib in ${CHOPSTIX_ZLIB_PREFIX}")
    set (ZLIB_INCLUDE_DIRS "${CHOPSTIX_ZLIB_PREFIX}/include")
    set (ZLIB_LIBRARIES "${CHOPSTIX_ZLIB_PREFIX}/lib/libz.a")

    add_executable(chop-trace2mpt
        trace2mpt.c ${ZLIB_INCLUDE_DIRS}/zlib.h
    )

    target_include_directories(chop-trace2mpt PUBLIC ${ZLIB_INCLUDE_DIRS})
else ()
    find_package(ZLIB REQUIRED)

    add_executable(chop-trace2mpt
        trace2mpt.c
    )
endif ()

# find_package(cJSON REQUIRED)

target_link_libraries(chop-trace2mpt ${ZLIB_LIBRARIES})

add_executable(chop-weight microweight.c)

set (CJSON_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/../external/cjson/libcjson.so")

add_dependencies(chop-weight cjson)
target_link_libraries(chop-weight ${CJSON_LIBRARIES})
target_include_directories(chop-weight PUBLIC ${CJSON_INCLUDE_DIRS} ${EXTERNAL_INCLUDE_DIRS})

add_executable(chop-perf-avg perfavg.c)

target_link_libraries(chop-perf-avg m)

install(PROGRAMS
    chop-marks-dyn-addr
    chop-marks-x86_64
    chop-marks-ppc64
    chop-marks-sysz
    chop-marks-riscv
    chop-marks
    chop-score-table
    chop-trace-mem
    DESTINATION bin)

install (TARGETS
    chop-detrace
    chop-detrace-mem
    chop-trace2mpt
    chop-weight
    chop-perf-avg
    DESTINATION bin
)

install (TARGETS
    chop-marks-dyn-addr-lib
    DESTINATION lib
)

add_subdirectory (clustering)
add_subdirectory (perf-invok)
if (CHOPSTIX_VALGRIND)
    add_subdirectory (valgrind)
endif ()
add_subdirectory (bp-checker)
# add_subdirectory (loader)
