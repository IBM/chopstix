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
###############################################################
# NAME        : src/client
# DESCRIPTION : The main chopstix executable
###############################################################

find_package (Threads)

# List of supported commands
set (cmd_list
    help
    sample
    disasm
    count
    annotate
    search
    list
    text
    trace
    view
    main
)

# Helper variables
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(hdr_dir ${CMAKE_BINARY_DIR}/cmd)
set(doc_dir ${CMAKE_CURRENT_SOURCE_DIR}/usage)

string(REGEX REPLACE "([^;]+)" "${src_dir}/\\1.cpp" src_files "${cmd_list}")
string(REGEX REPLACE "([^;]+)" "${hdr_dir}/\\1.h" hdr_files "${cmd_list}")
string(REGEX REPLACE "([^;]+)" "${doc_dir}/\\1" doc_files "${cmd_list}")

# Parse help for each command
foreach (cmd ${cmd_list})
    add_custom_command(
        OUTPUT ${hdr_dir}/${cmd}.h
        COMMAND ${SCRIPT_DIR}/parse_cmd.sh
            ${doc_dir}/${cmd} ${hdr_dir}/${cmd}.h
        DEPENDS ${SCRIPT_DIR}/parse_cmd.sh ${doc_dir}/${cmd}
        COMMENT "Parsed client/${cmd}"
        PRE_BUILD
    )
endforeach()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/client.h
    COMMAND ${SCRIPT_DIR}/compile_client.sh
        ${CMAKE_BINARY_DIR}/client.h ${cmd_list}
    DEPENDS ${SCRIPT_DIR}/compile_client.sh ${hdr_files}
    COMMENT "Compiled client.h"
    PRE_BUILD
)

add_custom_target(
    compile_client ALL
    DEPENDS ${hdr_files} ${CMAKE_BINARY_DIR}/client.h
)

# Main chopstix executable
add_executable(chop
    ${src_files}
    text_format.cpp
)

add_dependencies(chop compile_client)

target_link_libraries (chop
    cx-support
    cx-database
    cx-core
    cx-arch
    ${EXTERNAL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

set_property(TARGET chop PROPERTY CXX_STANDARD 11)
set_property(TARGET chop PROPERTY CXX_STANDARD_REQUIRED ON)

install (
    TARGETS chop
    DESTINATION bin
)
