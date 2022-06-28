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

find_program(PYTHON "python3" REQUIRED)
message (STATUS "Python3 program found: ${PYTHON}")
find_program(VIRTUALENV "virtualenv" REQUIRED)
message (STATUS "Virtualenv program found: ${VIRTUALENV}")

set(SETUP_PY "${CMAKE_CURRENT_SOURCE_DIR}/setup.py")
set(DEPS     "${CMAKE_CURRENT_SOURCE_DIR}/*.py" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.py" "${CMAKE_CURRENT_SOURCE_DIR}/*.txt" "${CMAKE_CURRENT_SOURCE_DIR}/*.md")
set(OUTPUT   "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(OUTPUT ${OUTPUT}/clustering_package
                   COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/*.py ${CMAKE_CURRENT_SOURCE_DIR}/*.txt ${CMAKE_CURRENT_SOURCE_DIR}/*.md ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}/clustering_package
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   DEPENDS ${DEPS})

add_custom_target(clustering ALL DEPENDS ${OUTPUT}/clustering_package ${DEPS})

install(CODE "execute_process(COMMAND ${PYTHON} -m virtualenv -p ${PYTHON} --prompt=\\(chopstix\\) ${CMAKE_INSTALL_PREFIX})")
install(CODE "execute_process(COMMAND sh -c \". ${CMAKE_INSTALL_PREFIX}/bin/activate && pip3 install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt\")")
install(CODE "execute_process(COMMAND sh -c \". ${CMAKE_INSTALL_PREFIX}/bin/activate && pip3 install --no-deps --prefix ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}\" WORKING_DIRECTORY ${OUTPUT})")
