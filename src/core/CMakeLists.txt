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
# NAME        : src/core
# DESCRIPTION : Chopstix core functionality
###############################################################

add_chopstix_library(core
    arch.cpp
    event.cpp
    sample.cpp
    perfmon.cpp
    process.cpp
    maps.cpp
    branch.cpp
    range.cpp
    instruction.cpp
    basicblock.cpp
    function.cpp
    module.cpp
    edge.cpp
    path.cpp
    search.cpp
    location.cpp
    tracer/preamble.cpp
    tracer/prologue.cpp
    tracer/regionofinterest.cpp
    tracer/state.cpp
    tracer/tracer.cpp
)

target_link_libraries(cx-core
    cx-support
    cx-database
)

