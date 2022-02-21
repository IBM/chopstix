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
cmake_minimum_required (VERSION 2.8)
project(chopstix CXX C ASM)

enable_testing()

set (CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
)

set (CHOPSTIX_PACKAGE ${PROJECT_NAME})
set (CHOPSTIX_VERSION_MAJOR 1)
set (CHOPSTIX_VERSION_MINOR 1)
set (CHOPSTIX_VERSION_PATCH 0)
set (CHOPSTIX_VERSION ${CHOPSTIX_VERSION_MAJOR}.${CHOPSTIX_VERSION_MINOR}.${CHOPSTIX_VERSION_PATCH})

message (STATUS "Building ${CHOPSTIX_PACKAGE} v${CHOPSTIX_VERSION}")

set (SCRIPT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build)
set (EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

include (BuildConfig)
include (CheckArchSupport)
include (CompileFlags)
include (Safeguards)
include (ChopstixLibrary)
include (ClangFormat)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.sh
)

file (GLOB_RECURSE all_sources src/*.cpp src/*.h)
add_clang_format (format ${all_sources})

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.sh
    DESTINATION share/chopstix
)

add_subdirectory (external)

set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${EXTERNAL_INCLUDE_DIRS}
)

add_subdirectory (src)

add_subdirectory (tools)

if (CHOPSTIX_BUILD_TESTS)
    enable_testing()
    add_subdirectory (test)
endif ()

if (CHOPSTIX_BUILD_EXAMPLES)
    add_subdirectory (examples)
endif ()
