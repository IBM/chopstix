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
project (sqlite3)

add_definitions (-DSQLITE_ENABLE_RTREE)

add_library (sqlite3 sqlite3.c)

add_executable (sqlite3exe shell.c)

target_link_libraries(sqlite3exe PUBLIC sqlite3 dl pthread)
target_include_directories(sqlite3exe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(sqlite3exe PROPERTIES OUTPUT_NAME sqlite3)

set (SQLITE3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
set (SQLITE3_LIBRARIES sqlite3 dl pthread PARENT_SCOPE)

set (CMAKE_C_FLAGS "-O3 -DNDEBUG")

install (TARGETS sqlite3exe DESTINATION bin RENAME sqlite3)
