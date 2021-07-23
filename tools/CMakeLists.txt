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

add_executable(chop-trace2mpt
    trace2mpt.c
)

find_package(ZLIB REQUIRED)
target_link_libraries(chop-trace2mpt ZLIB::ZLIB)

install(PROGRAMS
    chop-marks-ppc64
    chop-marks-sysz
    chop-score-table
    DESTINATION bin)

install (TARGETS
    chop-detrace
    chop-trace2mpt
    DESTINATION bin
)
