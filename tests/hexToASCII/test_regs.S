/* hexToASCII must not change ANY register (a0,a1,t0-t6,s0-s11,sp).
 * Returns 0 = pass, 1 = a register changed. */
    .data
buf:    .space 32
    .text
    .globl main

main:
    addi sp, sp, -16
    sw   ra, 12(sp)

    la   a0, buf
    li   a1, 0xF018
    li   a2, 0x2002
    li   a3, 0x2003
    li   a4, 0x2004
    li   a5, 0x2005
    li   a6, 0x2006
    li   a7, 0x2007
    li   t0, 0x3000
    li   t1, 0x3001
    li   t2, 0x3002
    li   t3, 0x3003
    li   t4, 0x3004
    li   t5, 0x3005
    li   t6, 0x3006
    .irp n,0,1,2,3,4,5,6,7,8,9,10,11
    li   s\n, 0x1000 + \n
    .endr

    call hexToASCII

    la   ra, buf
    bne  a0, ra, fail
    li   ra, 0xF018
    bne  a1, ra, fail
    .irp n,2,3,4,5,6,7
    li   ra, 0x2000 + \n
    bne  a\n, ra, fail
    .endr
    .irp n,0,1,2,3,4,5,6
    li   ra, 0x3000 + \n
    bne  t\n, ra, fail
    .endr
    .irp n,0,1,2,3,4,5,6,7,8,9,10,11
    li   ra, 0x1000 + \n
    bne  s\n, ra, fail
    .endr

    li   a0, 0
    j    out
fail:
    li   a0, 1
out:
    lw   ra, 12(sp)         # ra was used as scratch: restore it
    addi sp, sp, 16
    ret
