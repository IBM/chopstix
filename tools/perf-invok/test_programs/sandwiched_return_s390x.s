.text
	.align	8
	.align	16
.globl sandwich_return
	.type	sandwich_return, @function
sandwich_return:
    j .L2
.L1: 
	br	%r14
    .byte 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad
.L2:
    larl %r2, .L1
    lg %r2, 0(%r2)
    j .L1
