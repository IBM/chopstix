#pragma once

struct RiscVRegs {
    union {
        struct {
            unsigned long int pc;
            unsigned long int ra;
            unsigned long int sp;
            unsigned long int gp;
            unsigned long int tp;
            unsigned long int t0;
            unsigned long int t1;
            unsigned long int t2;
            unsigned long int s0;
            unsigned long int s1;
            unsigned long int a0;
            unsigned long int a1;
            unsigned long int a2;
            unsigned long int a3;
            unsigned long int a4;
            unsigned long int a5;
            unsigned long int a6;
            unsigned long int a7;
            unsigned long int s2;
            unsigned long int s3;
            unsigned long int s4;
            unsigned long int s5;
            unsigned long int s6;
            unsigned long int s7;
            unsigned long int s8;
            unsigned long int s9;
            unsigned long int s10;
            unsigned long int s11;
            unsigned long int t3;
            unsigned long int t4;
            unsigned long int t5;
            unsigned long int t6;
        };
        unsigned long int all[32];
    } gp;
    union {
        unsigned long int all[32];
    } fp;
};

#define RISCV_GPR_SIZE sizeof(((struct  RiscVRegs *)0)->gp)
#define RISCV_FPR_SIZE sizeof(((struct  RiscVRegs *)0)->fp)
