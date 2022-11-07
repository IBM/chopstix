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
include_directories (${COMMON_INCLUDE_DIRS})

# Helper functions and macros
# - Utilities (strings, streams)
# - Error checking
# - Options (environment)
add_subdirectory (support)

# Database scripts and helpers
# - Wrapper classes
# - SQL (transpiled to header)
add_subdirectory (database)

# Core functionalities
# - CFG (Modules, Functions, ...)
# - Process control
# - Sampling interface
# - Path finding
add_subdirectory (core)

# Architecture specific files
# -
add_subdirectory (arch)


# Support library for tracing
add_subdirectory (trace)

# Main CX executable client
# - Implements individual commands
add_subdirectory (client)

