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
# NAME        : src/trace
# DESCRIPTION : Tracing support library
###############################################################

find_library( DL_LIBRARY libdl.so )

add_library(cxtrace SHARED
    system.cpp
    memory.cpp
    buffer.cpp
)

install (
    TARGETS cxtrace
    DESTINATION lib
)

target_link_libraries(cxtrace cx-support)

SET(SUPPORT_LIBRARY_CFLAGS "-DLOG_CHILD")
get_target_property(TEMP cxtrace COMPILE_FLAGS)
if(TEMP STREQUAL "TEMP-NOTFOUND")
  SET(TEMP "") # Set to empty string
else()
  SET(TEMP "${TEMP} ") # A space to cleanly separate from existing content
endif()
# Append our values
SET(TEMP "${TEMP}${SUPPORT_LIBRARY_CFLAGS}" )
set_target_properties(cxtrace PROPERTIES COMPILE_FLAGS ${TEMP} )

SET(SUPPORT_LIBRARY_LDFLAGS "-static-libstdc++ -static-libgcc")
get_target_property(TEMP2 cxtrace COMPILE_FLAGS)
if(TEMP2 STREQUAL "TEMP-NOTFOUND")
  SET(TEMP2 "") # Set to empty string
else()
  SET(TEMP2 "${TEMP2} ") # A space to cleanly separate from existing content
endif()
# Append our values
SET(TEMP2 "${TEMP2}${SUPPORT_LIBRARY_LDFLAGS} ${DL_LIBRARY} -ldl" )
set_target_properties(cxtrace PROPERTIES LINK_FLAGS ${TEMP2})

# install (
#     FILES cxtrace.h
#     DESTINATION include
# )
