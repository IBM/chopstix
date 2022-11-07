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

typedef struct {
    unsigned long long address;
    unsigned long long originalData;
    unsigned int init;
} Breakpoint;

void setBreakpoint(unsigned long pid, unsigned long long address,
                   Breakpoint *breakpoint);
void resetBreakpoint(unsigned long pid, Breakpoint *breakpoint);
void compute_base_address(unsigned long pid, char* module, char* mainmodule);

#if defined(__s390x__)
#define NT_PRSTATUS 1
void displace_pc(long pid, long displ);
#endif
long get_current_pc(long pid);
