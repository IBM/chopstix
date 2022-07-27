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
############################################################
# NAME        : daxpy/CMakeLists.txt
# DESCRIPTION : Test ChopStiX on a simple daxpy kernel
############################################################

add_executable(example-tracing
    daxpy.c
)

set_target_properties(example-tracing PROPERTIES
    OUTPUT_NAME "daxpy"
    COMPILE_FLAGS "-O0 -Wall -Werror -pedantic"
)

install (
    TARGETS example-tracing
    DESTINATION ${EXAMPLES}/tracing
)

install (
    PROGRAMS tryme.sh README run_example.sh
    DESTINATION ${EXAMPLES}/tracing
)

