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
 * NAME        : support/options.h
 * DESCRIPTION : Provides options via environment variables
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace chopstix {

class Option {
  public:
    bool as_bool(bool def = false) const;
    long as_int(long def = 0) const;
    long as_hex(long def = 0) const;
    double as_float(double def = 0) const;
    std::string as_string(std::string def = "") const;
    double as_time(double def = 0) const;  // in s

    std::vector<bool> as_bool_vec() const;
    std::vector<long> as_int_vec() const;
    std::vector<long> as_hex_vec() const;
    std::vector<double> as_float_vec() const;
    std::vector<std::string> as_string_vec() const;
    std::vector<double> as_time_vec() const;

    bool is_set() const { return value_ != nullptr; }

    static Option get(const char *name);
    static void consume(int &argc, char **&argv, int count = 1);
    static Option arg(int &argc, char **&argv);

    operator bool() const { return is_set(); }
    operator const char *() const { return value_; }
    const char *value(const char *def = nullptr) const {
        return is_set() ? value_ : def;
    }

  private:
    Option(const char *val) : value_(val) {}
    const char *value_ = nullptr;
};

inline Option getopt(const char *name) { return Option::get(name); }
inline const char *getopt(const char *name, const char *def) {
    return Option::get(name).value(def);
}

struct OptionDef {
    const char *name;
    bool has_arg;
    const char *init;
};

int parse_options(int argc, char **argv, OptionDef *def);

void print_version();
void print_help(const char *help);

}  // namespace chopstix
