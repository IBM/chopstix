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
/***********************************************************
 * NAME        : client/help.cpp
 * DESCRIPTION : Implementation of the `help` command.
 *               Echos corresponding file in client/usage.
 ***********************************************************/

#include "client.h"
#include "usage.h"

#include <cstring>

#include "support/check.h"

using namespace chopstix;

#define PRINT_HELP(Cmd) \
    if (strcmp(argv[0], #Cmd) == 0) chopstix::print_help(help_##Cmd);

int run_help(int argc, char **argv) {
    PARSE_OPTIONS(help, argc, argv);
    if (argc == 0) chopstix::print_help(help_main);
    COMMAND_LIST(PRINT_HELP)
    failx("Unknown command '%s'", argv[0]);
    return 0;
}
