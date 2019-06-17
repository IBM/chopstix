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

#include <string>
#include <vector>

namespace cxtrace {
// Sync parameters over environment variables
void setparam(const char *name, const char *value, bool ovw = true);
void setparam(const char *name, const std::string &value, bool ovw = true);
const char *getparam(const char *name, const char *def = nullptr);
bool getflag(const char *name);
std::vector<long long> getaddrs(const char *name);
}
