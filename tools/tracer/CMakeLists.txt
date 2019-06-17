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
cmake_minimum_required(VERSION 2.8)
project(cxtrace)

if (NOT ARCH)
    set (ARCH zarch)
endif ()

string(TOUPPER ${ARCH} ARCH_UC)

add_definitions("-D_GNU_SOURCE")
add_definitions("-DARCH=\"${ARCH}\"")
add_definitions("-DARCH_${ARCH_UC}")
add_definitions("-std=gnu++11")

add_library(libcxtrace SHARED
    src/lib/system.cpp
    src/lib/memory.cpp
    src/lib/buffer.cpp
    src/common/log.cpp
    src/common/check.cpp
    src/common/safestring.cpp
    src/common/format.cpp
    src/common/param.cpp
    src/common/filesystem.cpp
    src/common/misc.cpp
    src/common/stringutils.cpp
)
set_property(TARGET libcxtrace PROPERTY OUTPUT_NAME "cxtrace")
set_property(TARGET libcxtrace PROPERTY CXX_STANDARD 11)

include_directories(
    src
)

add_executable(cxtrace
    src/tool/main.cpp
    src/tool/client.cpp
    src/tool/process.cpp
    src/tool/arch-${ARCH}.cpp
    src/common/check.cpp
    src/common/format.cpp
    src/common/safestring.cpp
    src/common/log.cpp
    src/common/misc.cpp
    src/common/param.cpp
    src/common/maps.cpp
    src/common/location.cpp
    src/common/popen.cpp
    src/common/stringutils.cpp
    src/common/filesystem.cpp
)
set_property(TARGET cxtrace PROPERTY CXX_STANDARD 11)

install(TARGETS cxtrace DESTINATION bin)
install(TARGETS libcxtrace DESTINATION lib)
install(PROGRAMS tools/cxprocess tools/cxmarks DESTINATION bin)

add_executable(detrace
    tools/detrace.c)
install(TARGETS detrace DESTINATION bin)
