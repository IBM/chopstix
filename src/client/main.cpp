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
/*************************************************************
 * NAME        : client/main.cpp
 * DESCRIPTION : Main entry point of the chopstix executable.
 *               Calls run_X() depending on the given command.
 *************************************************************/

#include "client.h"
#include "usage.h"

#include <cstring>

#include <string>
#include <vector>

#include "fmt/printf.h"
#include "support/check.h"
#include "support/string.h"

using namespace chopstix;

#define RUN_COMMAND(Cmd) \
    if (strcmp(cmd, #Cmd) == 0) return run_##Cmd(argc, argv);

#define MAYBE_COMMAND(Cmd) \
    if (string::distance(cmd, #Cmd) <= 2) maybe.push_back(#Cmd);

int main(int argc, char **argv) try {
    chopstix::Option::consume(argc, argv);
    PARSE_OPTIONS(main, argc, argv);
    if (argc < 1) chopstix::print_help(help_main);
    auto cmd = argv[0];
    chopstix::Option::consume(argc, argv);
    COMMAND_LIST(RUN_COMMAND)
    fmt::print(stderr, "Error: Unknown command '{}'\n", cmd);
    std::vector<std::string> maybe;
    COMMAND_LIST(MAYBE_COMMAND)
    if (maybe.empty()) return 1;
    fmt::print(stderr, "\nDid you mean this?\n");
    for (auto &c : maybe) fmt::print(stderr, "   {}\n", c);
    return 1;
} catch (const std::exception &e) {
    fmt::print(stderr, "Error: {}\n", e.what());
    exit(EXIT_FAILURE);
}
