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
# NAME        : src/database
# DESCRIPTION : Wrapper functionality for a SQLite database
###############################################################

set(sql_sources
    create_map
    create_sample
    insert_map
    insert_sample
    create_session

    create_module
    insert_module
    select_module

    create_func
    insert_func
    select_func
    select_func_by_score_and_size

    create_block
    insert_block
    select_block

    create_inst
    insert_inst
    select_inst

    create_edge
    insert_edge
    select_edge

    add_event

    find_by_rowid
    list_by_count
    list_by_score

    common_pragmas

    create_cfg

    create_path
    insert_path
    select_path
    insert_path_node
    select_path_node

    group_samples
    count_insts_pre
    count_insts
    count_insts_debug
    count_blocks
    count_funcs
    count_func_size
    count_modules
    score_insts
    score_blocks
    score_funcs
    score_modules
    norm_blocks
)

set(sql_dir ${CMAKE_BINARY_DIR}/sql)
set(queries ${CMAKE_BINARY_DIR}/queries.h)

string(REGEX REPLACE "([^;]+)" "${sql_dir}/\\1.h"
    sql_parsed "${sql_sources}")

foreach (sql_file ${sql_sources})
    add_custom_command(
        OUTPUT ${sql_dir}/${sql_file}.h
        COMMAND ${SCRIPT_DIR}/parse_sql.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/sql/${sql_file}.sql ${sql_dir}/${sql_file}.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sql/${sql_file}.sql ${SCRIPT_DIR}/parse_sql.sh
        COMMENT "Parsed ${sql_file}.sql"
        PRE_BUILD
    )
endforeach()

add_custom_command(
    OUTPUT ${queries}
    COMMAND ${SCRIPT_DIR}/compile_queries.sh ${queries} ${sql_sources}
    DEPENDS ${SCRIPT_DIR}/compile_queries.sh ${sql_parsed}
    COMMENT "Compiled queries"
    PRE_BUILD
)

add_custom_target(
    compile_queries ALL
    DEPENDS ${queries}
)

add_chopstix_library(database
    connection.cpp
    query.cpp
    record.cpp
    utils.cpp
)

add_dependencies(cx-database compile_queries)

set_property(TARGET cx-database PROPERTY CXX_STANDARD 11)
set_property(TARGET cx-database PROPERTY CXX_STANDARD_REQUIRED ON)

