.section .text
.align 2
.globl sandwich_return
.type sandwich_return,@function
sandwich_return:
    b .L2
.L1:
    blr
    .4byte 0xdeadbeef
.L2:
    lis 3,.L1@ha
    la 3,.L1@l(3)
    ld 3, 0(3)
    b .L1
