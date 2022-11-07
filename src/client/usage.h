/*
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
*/
#pragma once

/***********************************************************
 * NAME        : client/usage.cpp
 * DESCRIPTION : Macros for parsing command line arguments.
 ***********************************************************/

#include "fmt/format.h"
#include "fmt/printf.h"
#include "support/options.h"

#define CHECK_USAGE(Cmd, C, ...)                                         \
    do {                                                                 \
        if (!(C)) {                                                      \
            fmt::print(stderr, "Error: {}\n", fmt::format(__VA_ARGS__)); \
            fmt::print(stderr, help_##Cmd);                              \
            exit(EXIT_FAILURE);                                          \
        }                                                                \
    } while (0)

#define PARSE_OPTIONS(Cmd, Argc, Argv)                                 \
    do {                                                               \
        try {                                                          \
            int parsed = parse_options(Argc, Argv, opts_##Cmd);        \
            Argc -= parsed;                                            \
            Argv += parsed;                                            \
            if (Option::get("help").as_bool()) print_help(help_##Cmd); \
            if (Option::get("version").as_bool()) print_version();     \
        } catch (const std::exception &e) {                            \
            fmt::print(stderr, "Error: {}\n", e.what());               \
            fmt::print(stderr, help_##Cmd);                            \
            exit(EXIT_FAILURE);                                        \
        }                                                              \
    } while (0)
