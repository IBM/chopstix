/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2021 IBM Corporation
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

#include "debug.h"
#include "breakpoint.h"
#include <stdio.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int readBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint) {
    breakpoint->address = address;
    breakpoint->originalData = 0;
    debug_print("readBreakpoint 0x%016llX to 0x%08X (orig 0x%08llX)\n", address, 0, breakpoint->originalData);
    debug_print("readBreakpoint %p\n", (void *) address);
    errno = 0;
    breakpoint->originalData = ptrace(PTRACE_PEEKTEXT, pid, address, NULL);
    if (errno != 0) {
        switch (errno) {
            case EBUSY:
                debug_print("Error: EBUSY: %s\n", strerror(errno));
                break;
            case EFAULT:
                debug_print("Error: EFAULT: %s\n", strerror(errno));
                break;
            case EINVAL:
                debug_print("Error: EINVAL: %s\n", strerror(errno));
                break;
            case EIO:
                debug_print("Error: EIO: %s\n", strerror(errno));
                break;
            case EPERM:
                debug_print("Error: EPERM: %s\n", strerror(errno));
                break;
            case ESRCH:
                debug_print("Error: ESRCH: %s\n", strerror(errno));
                break;
            default:
                debug_print("Error: %s\n", strerror(errno));
                break;
        }
        return 1;
    };
    return 0;
}

int writeBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint) {
    breakpoint->address = address;
    debug_print("readBreakpoint 0x%016llX to 0x%08X (orig 0x%08llX)\n", address, 0, breakpoint->originalData);
    debug_print("readBreakpoint %p\n", (void *) address);
    errno = 0;
    breakpoint->originalData = ptrace(PTRACE_POKEDATA, pid, address, breakpoint->originalData);
    if (errno != 0) {
        switch (errno) {
            case EBUSY:
                debug_print("Error: EBUSY: %s\n", strerror(errno));
                break;
            case EFAULT:
                debug_print("Error: EFAULT: %s\n", strerror(errno));
                break;
            case EINVAL:
                debug_print("Error: EINVAL: %s\n", strerror(errno));
                break;
            case EIO:
                debug_print("Error: EIO: %s\n", strerror(errno));
                break;
            case EPERM:
                debug_print("Error: EPERM: %s\n", strerror(errno));
                break;
            case ESRCH:
                debug_print("Error: ESRCH: %s\n", strerror(errno));
                break;
            default:
                debug_print("Error: %s\n", strerror(errno));
                break;
        }
        return 1;
    };
    return 0;
}
