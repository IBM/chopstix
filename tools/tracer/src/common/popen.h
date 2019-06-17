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

// Language headers
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

namespace cxtrace {

// Launch external process
class Popen {
    friend std::ostream &operator<<(std::ostream &os, const Popen &);

  public:
    Popen(std::string cmd, size_t buf_size = 512)
        : cmd_(std::move(cmd)), buf_size_(buf_size) {}

  private:
    std::string cmd_;
    size_t buf_size_;
};
}
