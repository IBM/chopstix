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
# NAME        : src/arch
# DESCRIPTION : Architecture-specific implementations
###############################################################

set (sources "")

if (CHOPSTIX_POWER_SUPPORT OR CHOPSTIX_POWERLE_SUPPORT)
    set (sources ${sources} power.cpp)
endif ()

if (CHOPSTIX_SYSZ_SUPPORT)
    set (sources ${sources} sysz.cpp)
endif ()

if (CHOPSTIX_X86_SUPPORT)
    set (sources ${sources} x86.cpp)
endif ()

if (CHOPSTIX_RISCV_SUPPORT)
    set (sources ${sources} riscv.cpp)
endif ()

add_chopstix_library(arch
    ${sources}
)

target_link_libraries(cx-arch
    cx-core
    cx-database
    cx-support
)
