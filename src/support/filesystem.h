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
 * NAME        : support/filesystem.h
 * DESCRIPTION : Helper functions for filesystem operations.
 ******************************************************************************/

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace chopstix {
namespace filesystem {

// Basic file stats
bool exists(const std::string &path);
bool isfile(const std::string &path);
bool isreg(const std::string &path);
bool islink(const std::string &path);
bool isdir(const std::string &path);

bool isexe(const std::string &cmd);

// Path operations
std::string realpath(const std::string &path);
std::string basename(const std::string &path);
std::string dirname(const std::string &path);
std::string rootname(const std::string &path);
std::string extname(const std::string &path);
std::pair<std::string, std::string> split(const std::string &path);
std::pair<std::string, std::string> splitext(const std::string &path);
std::string join(std::string path_a, std::string path_b);

// File system
void mkdir(std::string path, int mode = 0777);
void rmdir(const std::string &path);
void chdir(const std::string &path);
void rename(const std::string &from, const std::string &to);
void remove(const std::string &path);

void copy(const std::string &from, const std::string &to);
std::vector<std::string> list(const std::string &path);

void deep_remove(const std::string &path);
void deep_copy(const std::string &from, const std::string &to);

}  // namespace filesystem
}  // namespace chopstix
