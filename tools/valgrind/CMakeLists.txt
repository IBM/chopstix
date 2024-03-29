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

set(DEPS1    "${CMAKE_CURRENT_SOURCE_DIR}/src/valgrind-3.19.0.tar.bz2")
set(DEPS2    "${CMAKE_CURRENT_SOURCE_DIR}/chopstix/*" valgrind1)
set(OUTPUT   "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(OUTPUT ${OUTPUT}/valgrind_chopstix1
                   COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/
                   COMMAND tar xf ${CMAKE_CURRENT_SOURCE_DIR}/src/valgrind-3.19.0.tar.bz2
                   COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/chopstix ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/
                   COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/ && patch Makefile.am < ${CMAKE_CURRENT_SOURCE_DIR}/src/Makefile.am.patch
                   COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/ && patch configure.ac < ${CMAKE_CURRENT_SOURCE_DIR}/src/configure.ac.patch
                   COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/ && ./autogen.sh
                   COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/ && ./configure --prefix ${CMAKE_INSTALL_PREFIX} --program-prefix chop-
                   COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}/valgrind_chopstix1
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${DEPS1})

add_custom_command(OUTPUT ${OUTPUT}/valgrind_chopstix2
                   COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/chopstix ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/
                   COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/ && make -j
                   COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}/valgrind_chopstix2
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${DEPS2})

add_custom_target(valgrind1 ALL DEPENDS ${OUTPUT}/valgrind_chopstix1 ${DEPS1})
add_custom_target(valgrind2 ALL DEPENDS ${OUTPUT}/valgrind_chopstix1 ${OUTPUT}/valgrind_chopstix2 ${DEPS2})

install(CODE "execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/valgrind-3.19.0/)")
