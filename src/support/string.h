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
/******************************************************************************
 * NAME        : support/string.h
 * DESCRIPTION : Utility functions for string objects
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace chopstix {
namespace string {

std::vector<std::string> splitg(const std::string &str,
                                const std::string &delim);

std::pair<std::string, std::string> split(const std::string &str,
                                          const std::string &delim);

std::string join(const std::vector<std::string> &vec,
                 const std::string &sep = ",");

void tolower(std::string &);
void toupper(std::string &);

void ltrim(std::string &str, const std::string &ws = " \t\n\r");
void rtrim(std::string &str, const std::string &ws = " \t\n\r");
void trim(std::string &str, const std::string &ws = " \t\n\r");

int distance(const std::string &s1, const std::string &s2);

std::string demangle(const std::string &s);

}  // namespace string
}  // namespace chopstix
