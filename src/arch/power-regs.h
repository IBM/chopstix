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

// Note: 64-bit support only

#define POWER_R0 0
#define POWER_R1 1
#define POWER_R2 2
#define POWER_R3 3
#define POWER_R4 4
#define POWER_R5 5
#define POWER_R6 6
#define POWER_R7 7
#define POWER_R8 8
#define POWER_R9 9
#define POWER_R10 10
#define POWER_R11 11
#define POWER_R12 12
#define POWER_R13 13
#define POWER_R14 14
#define POWER_R15 15
#define POWER_R16 16
#define POWER_R17 17
#define POWER_R18 18
#define POWER_R19 19
#define POWER_R20 20
#define POWER_R21 21
#define POWER_R22 22
#define POWER_R23 23
#define POWER_R24 24
#define POWER_R25 25
#define POWER_R26 26
#define POWER_R27 27
#define POWER_R28 28
#define POWER_R29 29
#define POWER_R30 30
#define POWER_R31 31
#define POWER_NIP 32
#define POWER_MSR 33
#define POWER_ORIG_R3 34
#define POWER_CTR 35
#define POWER_LNK 36
#define POWER_XER 37
#define POWER_CCR 38
#define POWER_SOFTE 39
#define POWER_TRAP 40
#define POWER_DAR 41
#define POWER_DSISR 42
#define POWER_RESULT 43
// #define POWER_DSCR 44
#define POWER_NUM_REGS 44

// Floating-Point
#define POWER_FPR0 48
#define POWER_FPSCR (POWER_FPR0 + 32)

// Vector
#define POWER_VR0 82
#define POWER_VSCR (POWER_VR0 + 32 * 2 + 1)
#define POWER_VRSAVE (POWER_VR0 * 33 * 2)

// Vector scalar
#define POWER_VSR0 150
#define POWER_VSR31 (POWER_VSR0 + 2 * 31)

const char *power_regnames[] = {
    "R0",  "R1",  "R2",  "R3",    "R4",   "R5",  "R6",    "R7",      "R8",
    "R9",  "R10", "R11", "R12",   "R13",  "R14", "R15",   "R16",     "R17",
    "R18", "R19", "R20", "R21",   "R22",  "R23", "R24",   "R25",     "R26",
    "R27", "R28", "R29", "R30",   "R31",  "NIP", "MSR",   "ORIG_R3", "CTR",
    "LNK", "XER", "CCR", "SOFTE", "TRAP", "DAR", "DSISR", "RESULT",
};
