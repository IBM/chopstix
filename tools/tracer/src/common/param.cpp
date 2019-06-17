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
#include "param.h"

#include "stringutils.h"

#include <algorithm>
#include <cctype>

namespace {
static std::string toenv(const char *name) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "cxtrace_%s", name);
    std::string real_name{buf};
    std::replace_if(real_name.begin(), real_name.end(),
                    [](char c) { return !std::isalnum(c); }, '_');
    for (auto &c : real_name) c = std::toupper(c);
    return std::move(real_name);
}
}

namespace cxtrace {
void setparam(const char *name, const char *value, bool ovw) {
    std::string env{toenv(name)};
    setenv(env.c_str(), value, ovw ? 1 : 0);
}
void setparam(const char *name, const std::string &value, bool ovw) {
    setparam(name, value.c_str(), ovw);
}
const char *getparam(const char *name, const char *def) {
    std::string env{toenv(name)};
    const char *value = getenv(env.c_str());
    return value ? value : def;
}
bool getflag(const char *name) { return atoi(getparam(name, "0")) != 0; }
std::vector<long long> getaddrs(const char *name) {
    std::vector<long long> as;
    std::vector<std::string> split{ssplitg(getparam(name, ""), ":;,")};
    as.reserve(split.size());
    for (auto &s : split) {
        as.push_back(stoaddr(s));
    }
    return std::move(as);
}
}
