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

/* Program Status Word.  */
#define SYSZ_PSWM 0
#define SYSZ_PSWA 0
/* General Purpose Registers.  */
#define SYSZ_R0 2
#define SYSZ_R1 3
#define SYSZ_R2 4
#define SYSZ_R3 5
#define SYSZ_R4 6
#define SYSZ_R5 7
#define SYSZ_R6 8
#define SYSZ_R7 9
#define SYSZ_R8 10
#define SYSZ_R9 11
#define SYSZ_R10 12
#define SYSZ_R11 13
#define SYSZ_R12 14
#define SYSZ_R13 15
#define SYSZ_R14 16
#define SYSZ_R15 17
/* Access Registers.  */
#define SYSZ_A0 18
#define SYSZ_A1 19
#define SYSZ_A2 20
#define SYSZ_A3 21
#define SYSZ_A4 22
#define SYSZ_A5 23
#define SYSZ_A6 24
#define SYSZ_A7 25
#define SYSZ_A8 26
#define SYSZ_A9 27
#define SYSZ_A10 28
#define SYSZ_A11 29
#define SYSZ_A12 30
#define SYSZ_A13 31
#define SYSZ_A14 32
#define SYSZ_A15 33
/* Floating Point Control Word.  */
#define SYSZ_FPC 34
/* Floating Point Registers.  */
#define SYSZ_F0 35
#define SYSZ_F1 36
#define SYSZ_F2 37
#define SYSZ_F3 38
#define SYSZ_F4 39
#define SYSZ_F5 40
#define SYSZ_F6 41
#define SYSZ_F7 42
#define SYSZ_F8 43
#define SYSZ_F9 44
#define SYSZ_F10 45
#define SYSZ_F11 46
#define SYSZ_F12 47
#define SYSZ_F13 48
#define SYSZ_F14 49
#define SYSZ_F15 50
/* General Purpose Register Upper Halves.  */
#define SYSZ_R0_UPPER 51
#define SYSZ_R1_UPPER 52
#define SYSZ_R2_UPPER 53
#define SYSZ_R3_UPPER 54
#define SYSZ_R4_UPPER 55
#define SYSZ_R5_UPPER 56
#define SYSZ_R6_UPPER 57
#define SYSZ_R7_UPPER 58
#define SYSZ_R8_UPPER 59
#define SYSZ_R9_UPPER 60
#define SYSZ_R10_UPPER 61
#define SYSZ_R11_UPPER 62
#define SYSZ_R12_UPPER 63
#define SYSZ_R13_UPPER 64
#define SYSZ_R14_UPPER 65
#define SYSZ_R15_UPPER 66
/* GNU/Linux-specific optional registers.  */
#define SYSZ_ORIG_R2 67
#define SYSZ_LAST_BREAK 68
#define SYSZ_SYSTEM_CALL 69
/* Transaction diagnostic block.  */
#define SYSZ_TDB_DWORD0 70
#define SYSZ_TDB_ABORT_CODE 71
#define SYSZ_TDB_CONFLICT_TOKEN 72
#define SYSZ_TDB_ATIA 73
#define SYSZ_TDB_R0 74
#define SYSZ_TDB_R1 75
#define SYSZ_TDB_R2 76
#define SYSZ_TDB_R3 77
#define SYSZ_TDB_R4 78
#define SYSZ_TDB_R5 79
#define SYSZ_TDB_R6 80
#define SYSZ_TDB_R7 81
#define SYSZ_TDB_R8 82
#define SYSZ_TDB_R9 83
#define SYSZ_TDB_R10 84
#define SYSZ_TDB_R11 85
#define SYSZ_TDB_R12 86
#define SYSZ_TDB_R13 87
#define SYSZ_TDB_R14 88
#define SYSZ_TDB_R15 89
/* Vector registers.  */
#define SYSZ_V0_LOWER 90
#define SYSZ_V1_LOWER 91
#define SYSZ_V2_LOWER 92
#define SYSZ_V3_LOWER 93
#define SYSZ_V4_LOWER 94
#define SYSZ_V5_LOWER 95
#define SYSZ_V6_LOWER 96
#define SYSZ_V7_LOWER 97
#define SYSZ_V8_LOWER 98
#define SYSZ_V9_LOWER 99
#define SYSZ_V10_LOWER 100
#define SYSZ_V11_LOWER 101
#define SYSZ_V12_LOWER 102
#define SYSZ_V13_LOWER 103
#define SYSZ_V14_LOWER 104
#define SYSZ_V15_LOWER 105
#define SYSZ_V16 106
#define SYSZ_V17 107
#define SYSZ_V18 108
#define SYSZ_V19 109
#define SYSZ_V20 110
#define SYSZ_V21 111
#define SYSZ_V22 112
#define SYSZ_V23 113
#define SYSZ_V24 114
#define SYSZ_V25 115
#define SYSZ_V26 116
#define SYSZ_V27 117
#define SYSZ_V28 118
#define SYSZ_V29 119
#define SYSZ_V30 120
#define SYSZ_V31 121
/* Total.  */
#define SYSZ_NUM_REGS 122

const char *sysz_regnames[] = {
    "PSWM",
    "PSWA",
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "R15",
    "A0",
    "A1",
    "A2",
    "A3",
    "A4",
    "A5",
    "A6",
    "A7",
    "A8",
    "A9",
    "A10",
    "A11",
    "A12",
    "A13",
    "A14",
    "A15",
    "FPC",
    "F0",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "R0_UPPER",
    "R1_UPPER",
    "R2_UPPER",
    "R3_UPPER",
    "R4_UPPER",
    "R5_UPPER",
    "R6_UPPER",
    "R7_UPPER",
    "R8_UPPER",
    "R9_UPPER",
    "R10_UPPER",
    "R11_UPPER",
    "R12_UPPER",
    "R13_UPPER",
    "R14_UPPER",
    "R15_UPPER",
    "ORIG_R2",
    "LAST_BREAK",
    "SYSTEM_CALL",
    "TDB_DWORD0",
    "TDB_ABORT_CODE",
    "TDB_CONFLICT_TOKEN",
    "TDB_ATIA",
    "TDB_R0",
    "TDB_R1",
    "TDB_R2",
    "TDB_R3",
    "TDB_R4",
    "TDB_R5",
    "TDB_R6",
    "TDB_R7",
    "TDB_R8",
    "TDB_R9",
    "TDB_R10",
    "TDB_R11",
    "TDB_R12",
    "TDB_R13",
    "TDB_R14",
    "TDB_R15",
    "V0_LOWER",
    "V1_LOWER",
    "V2_LOWER",
    "V3_LOWER",
    "V4_LOWER",
    "V5_LOWER",
    "V6_LOWER",
    "V7_LOWER",
    "V8_LOWER",
    "V9_LOWER",
    "V10_LOWER",
    "V11_LOWER",
    "V12_LOWER",
    "V13_LOWER",
    "V14_LOWER",
    "V15_LOWER",
    "V16",
    "V17",
    "V18",
    "V19",
    "V20",
    "V21",
    "V22",
    "V23",
    "V24",
    "V25",
    "V26",
    "V27",
    "V28",
    "V29",
    "V30",
    "V31",
};

typedef unsigned int __u32;
typedef unsigned long __u64;

typedef struct {
    unsigned long mask;
    unsigned long addr;
} __attribute__ ((aligned(8))) psw_t;

typedef union {
    float   f;
    double  d;
    __u64   ui;
    struct
    {
        __u32 hi;
        __u32 lo;
    } fp;
} freg_t;

typedef union {
    struct {
        freg_t low;
        freg_t high;
    } fld;
    __int128 vr;
} vector_t;

struct ArchZRegs {
    union {
        struct {
            psw_t psw;
            unsigned long r0;
            unsigned long r1;
            unsigned long r2;
            unsigned long r3;
            unsigned long r4;
            unsigned long r5;
            unsigned long r6;
            unsigned long r7;
            unsigned long r8;
            unsigned long r9;
            unsigned long r10;
            unsigned long r11;
            unsigned long r12;
            unsigned long r13;
            unsigned long r14;
            unsigned long r15;
            unsigned long r16;
            unsigned int ac0;
            unsigned int ac1;
            unsigned int ac2;
            unsigned int ac3;
            unsigned int ac4;
            unsigned int ac5;
            unsigned int ac6;
            unsigned int ac7;
            unsigned int ac8;
            unsigned int ac9;
            unsigned int ac10;
            unsigned int ac11;
            unsigned int ac12;
            unsigned int ac13;
            unsigned int ac14;
            unsigned int ac15;
            unsigned int ac16;
            unsigned long orig_gpr2;
        };
        unsigned long int all[27];
    } gp;
    union {
        struct {
            __u32  fpc;
            __u32  pad;
            freg_t fp0;
            freg_t fp1;
            freg_t fp2;
            freg_t fp3;
            freg_t fp4;
            freg_t fp5;
            freg_t fp6;
            freg_t fp7;
            freg_t fp8;
            freg_t fp9;
            freg_t fp10;
            freg_t fp11;
            freg_t fp12;
            freg_t fp13;
            freg_t fp14;
            freg_t fp15;
            freg_t fp16;
        };
        freg_t all[17];
    } fp;
    union {
        struct {
            vector_t vr0;
            vector_t vr1;
            vector_t vr2;
            vector_t vr3;
            vector_t vr4;
            vector_t vr5;
            vector_t vr6;
            vector_t vr7;
            vector_t vr8;
            vector_t vr9;
            vector_t vr10;
            vector_t vr11;
            vector_t vr12;
            vector_t vr13;
            vector_t vr14;
            vector_t vr15;
            vector_t vr16;
            vector_t vr17;
            vector_t vr18;
            vector_t vr19;
            vector_t vr20;
            vector_t vr21;
            vector_t vr22;
            vector_t vr23;
            vector_t vr24;
            vector_t vr25;
            vector_t vr26;
            vector_t vr27;
            vector_t vr28;
            vector_t vr29;
            vector_t vr30;
            vector_t vr31;
        };
        vector_t all[32];
        freg_t low[16];
        __int128 high[16];
    } vr;
};

#define NT_PRSTATUS 1
#define NT_PRFPREG  2
#define NT_S390_VXRS_LOW    0x309   /* s390 vector registers 0-15 upper half */
#define NT_S390_VXRS_HIGH   0x30a   /* s390 vector registers 16-31 */

#define PTRACE_GP_REGISTERS NT_PRSTATUS
#define PTRACE_FP_REGISTERS NT_PRFPREG

#define PTRACE_VXR1_REGISTERS NT_S390_VXRS_LOW
#define PTRACE_VXR2_REGISTERS NT_S390_VXRS_HIGH

#define ARCHZ_GPR_SIZE sizeof(((struct ArchZRegs *)0)->gp)
#define ARCHZ_FPR_SIZE sizeof(((struct ArchZRegs *)0)->fp)
#define ARCHZ_VXR1_SIZE sizeof(((struct ArchZRegs *)0)->vr.low)
#define ARCHZ_VXR2_SIZE sizeof(((struct ArchZRegs *)0)->vr.high)
