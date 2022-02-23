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

// ~~~ SiFive FU540 specific values ~~~

// ~~ PMU Events ~~

// Instruction commit events
#define EVENT_BIT_INT_LOAD_RETIRED  (1 << 9)
#define EVENT_BIT_INT_STORE_RETIRED (1 << 10)
#define EVENT_BIT_FLT_LOAD_RETIRED  (1 << 19)
#define EVENT_BIT_FLT_STORE_RETIRED (1 << 20)

// Microarchitectural events
#define EVENT_BIT_DATA_CACHE_BUSY ((1 << 12) | (1 & 0xFF))

// Memory system events
#define EVENT_BIT_DATA_CACHE_MISS ((1 << 9) | (2 & 0xFF))

// ~~ RTC Clock Multiplier ~~
#define TIME_TO_US 1
