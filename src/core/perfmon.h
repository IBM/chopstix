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
 * NAME        : core/perfmon.h
 * DESCRIPTION : Singleton to initialize libpfm
 ******************************************************************************/

#pragma once

// Library headers
#include <perfmon/pfmlib.h>

namespace chopstix {

// Singleton object
// Call Perfmon::init/terminate
class Perfmon {
  public:
    static Perfmon &Get_instance();
    void initialize();
    void terminate();
    pfm_pmu_t get_pmu();

  private:
    bool init_ = false;

    Perfmon() = default;
    ~Perfmon();
    Perfmon(const Perfmon &) = delete;
    Perfmon &operator=(const Perfmon &) = delete;
};

}  // namespace chopstix
