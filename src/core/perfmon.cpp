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
 * NAME        : core/perfmon.cpp
 * DESCRIPTION : Singleton to initialize libpfm
 ******************************************************************************/

#include "perfmon.h"

// Library headers
#include <perfmon/pfmlib.h>

// Private headers
#include "support/check.h"

using namespace chopstix;

Perfmon &Perfmon::Get_instance() {
    static Perfmon instance;
    return instance;
}

void Perfmon::initialize() {
    if (!init_) {
        auto ret = pfm_initialize();
        checkx(ret == PFM_SUCCESS, "perfmon: %s", pfm_strerror(ret));
        init_ = true;
    }
}

void Perfmon::terminate() {
    if (init_) {
        pfm_terminate();
    }
}

Perfmon::~Perfmon() { terminate(); }
