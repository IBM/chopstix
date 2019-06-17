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
 * NAME        : support/options.cpp
 * DESCRIPTION : Provides options via environment variables
 ******************************************************************************/

#include "options.h"

#include <cstdlib>
#include <cstring>

#include "check.h"
#include "config.h"
#include "string.h"

#include "fmt/printf.h"

using namespace chopstix;

namespace {
std::string toenv(std::string name) {
    string::toupper(name);
    std::replace(name.begin(), name.end(), '-', '_');
    return "CHOPSTIX_OPT_" + name;
}
}  // namespace

Option Option::get(const char *name) { return getenv(toenv(name).c_str()); }

bool Option::as_bool(bool def) const {
    if (!is_set()) return def;
    std::string val(value_);
    string::tolower(val);
    if (val == "0") return false;
    if (val == "no") return false;
    if (val == "false") return false;
    if (val == "off") return false;
    if (val == "1") return true;
    if (val == "yes") return true;
    if (val == "true") return true;
    if (val == "on") return true;
    fail("Bad option: '%s'", value_);
}

long Option::as_int(long def) const {
    if (!is_set()) return def;
    char *end;
    long val = strtol(value_, &end, 10);
    check(value_ != end, "Expected integer: '%s'", value_);
    return val;
}

long Option::as_hex(long def) const {
    if (!is_set()) return def;
    char *end;
    long val = strtol(value_, &end, 16);
    check(value_ != end, "Expected hexadecimal: '%s'", value_);
    return val;
}

double Option::as_float(double def) const {
    if (!is_set()) return def;
    char *end;
    long double val = strtod(value_, &end);
    check(value_ != end, "Expected real number: '%s'", value_);
    if (*end == '%') return val * 1e-2;
    return val;
}

std::string Option::as_string(std::string def) const {
    if (!is_set()) return def;
    return value_;
}

double Option::as_time(double def) const {
    if (!is_set()) return def;
    char *end;
    double val = strtod(value_, &end);
    check(value_ != end, "Expected time value: '%s'", value_);
    std::string unit = end;
    if (unit == "" || unit == "s") return val;
    if (unit == "us") return val * 1e-6;
    if (unit == "ms") return val * 1e-3;
    if (unit == "m") return val * 60;
    if (unit == "h") return val * 3600;
    if (unit == "d") return val * 3600 * 24;
    return val;
}

std::vector<bool> Option::as_bool_vec() const {
    std::vector<bool> vals;
    auto parsed = string::splitg(as_string(), ",; \t\n");
    vals.reserve(parsed.size());
    for (auto &str : parsed) {
        vals.push_back(Option(str.c_str()).as_bool());
    }
    return vals;
}

std::vector<long> Option::as_int_vec() const {
    std::vector<long> vals;
    auto parsed = string::splitg(as_string(), ",; \t\n");
    vals.reserve(parsed.size());
    for (auto &str : parsed) {
        vals.push_back(Option(str.c_str()).as_int());
    }
    return vals;
}

std::vector<long> Option::as_hex_vec() const {
    std::vector<long> vals;
    auto parsed = string::splitg(as_string(), ",; \t\n");
    vals.reserve(parsed.size());
    for (auto &str : parsed) {
        vals.push_back(Option(str.c_str()).as_hex());
    }
    return vals;
}

std::vector<double> Option::as_float_vec() const {
    std::vector<double> vals;
    auto parsed = string::splitg(as_string(), ",; \t\n");
    vals.reserve(parsed.size());
    for (auto &str : parsed) {
        vals.push_back(Option(str.c_str()).as_float());
    }
    return vals;
}

std::vector<std::string> Option::as_string_vec() const {
    return string::splitg(as_string(), ",; \t\n");
}

std::vector<double> Option::as_time_vec() const {
    std::vector<double> vals;
    auto parsed = string::splitg(as_string(), ",; \t\n");
    vals.reserve(parsed.size());
    for (auto &str : parsed) {
        vals.push_back(Option(str.c_str()).as_time());
    }
    return vals;
}

void Option::consume(int &argc, char **&argv, int count) {
    argc -= count;
    argv += count;
}

Option Option::arg(int &argc, char **&argv) {
    if (argc > 0 && argv[0][0] != '-') {
        consume(argc, argv);
        return argv[-1];
    } else {
        return nullptr;
    }
}

int chopstix::parse_options(int argc, char **argv, OptionDef *def) {
    for (OptionDef *opt = def; opt->name; ++opt) {
        if (opt->init) setenv(toenv(opt->name).c_str(), opt->init, 0);
    }
    int parsed = 0;
    while (parsed < argc) {
        char *arg = argv[parsed];
        if (arg[0] != '-') break;
        ++parsed;
        while (*arg == '-') ++arg;
        if (*arg == '\0') break;
        OptionDef *opt = def;
        for (; opt->name; ++opt) {
            if (!opt->has_arg) {
                if (strcmp(arg, opt->name) == 0) {
                    setenv(toenv(opt->name).c_str(), "yes", 1);
                    break;
                } else if (strncmp(arg, "no-", 3) == 0 &&
                           strcmp(arg + 3, opt->name) == 0) {
                    setenv(toenv(opt->name).c_str(), "no", 1);
                    break;
                }
            } else {
                if (strcmp(arg, opt->name) == 0) break;
            }
        }
        checkx(opt->name, "Unknown option '%s'", arg);
        if (!opt->has_arg) continue;
        checkx(parsed < argc, "Expected argument for option '%s'", arg);
        setenv(toenv(opt->name).c_str(), argv[parsed++], 1);
    }
    return parsed;
}

void chopstix::print_version() {
    printf(CHOPSTIX_PACKAGE " v" CHOPSTIX_VERSION "\n");
    exit(EXIT_SUCCESS);
}

void chopstix::print_help(const char *help) {
    printf("%s", help);
    exit(EXIT_SUCCESS);
}
