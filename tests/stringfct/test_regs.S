/* Calling convention: my_strlen / my_strcpy / my_strtolower must leave
 * s0..s11 (and sp) unchanged. Returns 0 = pass, N = failing check. */
    .data
buf:    .space 32
    .section .rodata
src:    .asciz "Hello"

    .text
    .globl main

    .macro SETSAVED
    .irp n,0,1,2,3,4,5,6,7,8,9,10,11
    li   s\n, 0x1000 + \n
    .endr
    .endm

    # check s0..s11 and sp; uses ra as scratch (already saved by main)
    .macro CHKSAVED code
    .irp n,0,1,2,3,4,5,6,7,8,9,10,11
    li   ra, 0x1000 + \n
    bne  s\n, ra, fail\code
    .endr
    addi ra, sp, 0
    bne  sp, ra, fail\code
    .endm

main:
    addi sp, sp, -16
    sw   ra, 12(sp)

    SETSAVED
    la   a0, buf
    la   a1, src
    call my_strcpy
    CHKSAVED 1

    SETSAVED
    la   a0, buf
    call my_strlen
    li   ra, 5
    bne  a0, ra, fail2
    CHKSAVED 3

    SETSAVED
    la   a0, buf
    call my_strtolower
    CHKSAVED 4

    li   a0, 0
    j    out
fail1: li a0, 1
    j  out
fail2: li a0, 2
    j  out
fail3: li a0, 3
    j  out
fail4: li a0, 4
out:
    lw   ra, 12(sp)         # ra was used as scratch: restore it
    addi sp, sp, 16
    ret
