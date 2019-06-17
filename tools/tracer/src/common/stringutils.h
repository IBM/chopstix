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
#include <utility>
#include <vector>

namespace cxtrace {
std::pair<std::string, std::string> ssplit(const std::string &str,
                                           const std::string &delim);
std::vector<std::string> ssplitg(const std::string &str,
                                 const std::string &delim);

long long stoaddr(const std::string &str);
}
