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
# ----------------------------#
# Build external dependencies #
# ----------------------------#

include (ExternalProject)

# Sqlite3 https://www.sqlite.org
if (EXISTS "${CHOPSTIX_SQLITE_PREFIX}")
    include (CheckSqlite3)
    message (STATUS "Using sqlite3 in ${CHOPSTIX_SQLITE_PREFIX}")
    set (SQLITE3_INCLUDE_DIRS "${CHOPSTIX_SQLITE_PREFIX}/include")
    set (SQLITE3_LIBRARIES "${CHOPSTIX_SQLITE_PREFIX}/lib/libsqlite3.a")
elseif (CHOPSTIX_BUILD_SQLITE)
    message (STATUS "Building custom version of sqlite3")
    add_subdirectory ("sqlite3")
else ()
    find_package(SQLite3)
    if (NOT SQLITE3_FOUND)
        message (STATUS "Building custom version of sqlite3")
        add_subdirectory ("sqlite3")
    endif()
endif ()

# Perfmon2 http://perfmon2.sourceforge.net
if (EXISTS "${CHOPSTIX_PERFMON_PREFIX}")
    include (CheckPerfmon2)
    message (STATUS "Using perfmon2 in ${CHOPSTIX_PERFMON_PREFIX}")
    set (PERFMON2_INCLUDE_DIRS "${CHOPSTIX_PERFMON_PREFIX}/include")
    set (PERFMON2_LIBRARIES "${CHOPSTIX_PERFMON_PREFIX}/lib/libpfm.a" dl)
else ()
    find_package(Perfmon2 REQUIRED)
endif ()



# {fmt} https://github.com/fmtlib/fmt
add_subdirectory (fmt)

set (EXTERNAL_INCLUDE_DIRS
    ${EXTERNAL_DIR}/fmt
    ${PERFMON2_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    PARENT_SCOPE
)

set (extra_libs)

if (CHOPSTIX_PROFILE_CPU)
    set (extra_libs ${extra_libs} profiler)
endif ()

set (EXTERNAL_LIBRARIES
    ${extra_libs}
    fmt
    dl
    ${PERFMON2_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    PARENT_SCOPE
)
