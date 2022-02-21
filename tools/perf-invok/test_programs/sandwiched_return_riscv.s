.global sandwich_return
sandwich_return:
    j .L2
.L1:
    ret
    .word 0xdeadbeef
.L2:
    la a0, .L1
    ld a0, 0(a0)
    j .L1
