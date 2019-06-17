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
 * NAME        : core/arch.cpp
 * DESCRIPTION : Abstract base class for architecture-specific implementations
 ******************************************************************************/

#include "arch.h"

#include "fmt/format.h"
#include "support/check.h"
#include "support/filesystem.h"
#include "support/string.h"

#include <sys/utsname.h>

#if defined(CHOPSTIX_POWER_SUPPORT) || defined(CHOPSTIX_POWERLE_SUPPORT)
#include "arch/power.h"
#endif
#ifdef CHOPSTIX_SYSZ_SUPPORT
#include "arch/sysz.h"
#endif
#ifdef CHOPSTIX_X86_SUPPORT
#include "arch/x86.h"
#endif

using namespace chopstix;

namespace fs = filesystem;

namespace {

bool anyof(const std::string &str, const std::vector<std::string> &opts) {
    for (auto &opt : opts) {
        if (str.find(opt) != std::string::npos) {
            return true;
        }
    }
    return false;
}

}  // namespace

std::string Arch::get_machine() {
    struct utsname buf;
    uname(&buf);
    return buf.machine;
}

Arch::impl_ptr Arch::get_impl(std::string name) {
    string::tolower(name);

#if defined(CHOPSTIX_POWER_SUPPORT) || defined(CHOPSTIX_POWERLE_SUPPORT)
    if (anyof(name, {"ppc", "power", "powerpc", "ppc64", "ppc64le"})) {
        return impl_ptr(new ArchPower());
    } else
#endif
#ifdef CHOPSTIX_SYSZ_SUPPORT
        if (anyof(name, {"system", "z", "s360", "s390"})) {
        return impl_ptr(new ArchZ());
    } else
#endif
#ifdef CHOPSTIX_X86_SUPPORT
        if (anyof(name, {"x86", "amd"})) {
        return impl_ptr(new ArchX86());
    } else
#endif
    {
        failx("No support for '%s'", name);
    }
}

Popen Arch::objdump(const std::string &filename) const {
    std::string cmd = "objdump";
    for (auto &pre : prefix()) {
        std::string tst = fmt::format("{}-linux-gnu-objdump", pre);
        if (fs::isexe(tst)) {
            cmd = tst;
            break;
        }
    }

    std::string run = fmt::format("{} -j .text -d {}", cmd, filename);
    return Popen(run);
}
