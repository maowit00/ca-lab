


    .globl _start

# - -- - -- - --


    .data
msg: .ascii "Hello, RISC-V\n"

# - -- - -- - -- - -- - -- - -- - -- - --

    .text

_start:

    li t0, 10            # init counter

loop:
    li   a7, 64          # syscall: write
    li   a0, 1           # fd = stdout
    la   a1, msg
    li   a2, 14          # length
    ecall

    addi t0, t0, -1
    bnez t0, loop

    # - -- - -- - -- - -- - -- - --

    li   a7, 93          # syscall: exit
    li   a0, 0
    ecall
