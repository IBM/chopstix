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

pfm_pmu_t Perfmon::get_pmu() {
    if (!init_) {
        Perfmon::initialize();
    }

    unsigned int pmu = PFM_PMU_NONE;
    pfm_pmu_info_t pinfo;
    for (pmu = PFM_PMU_NONE; pmu < PFM_PMU_MAX ; pmu=pmu+1) {
        memset(&pinfo, 0, sizeof(pinfo));
        int ret = pfm_get_pmu_info((pfm_pmu_t) pmu, &pinfo);
        log::debug("Testing PMU: %d", pmu);
        if (ret == PFM_SUCCESS) break;
        if (ret == PFM_ERR_NOINIT) {
            log::debug("pfm library has not been initialized properly");
        }
        if (ret == PFM_ERR_NOTSUPP) {
            log::debug("PMU model is not supported by the library");
        }
        if (ret == PFM_ERR_INVAL) {
            log::debug("Invalid argument");
        }
    }
    checkx(pmu != PFM_PMU_MAX, "cannot get pmu info (tested all)");

    return (pfm_pmu_t) pmu;
}

void Perfmon::terminate() {
    if (init_) {
        pfm_terminate();
    }
}

Perfmon::~Perfmon() { terminate(); }
