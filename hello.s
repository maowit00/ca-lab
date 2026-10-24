
    .globl _start
    .text

_start:
    li   a7, 64          # syscall: write
    li   a0, 1           # fd = stdout
    la   a1, msg
    li   a2, 14          # length
    ecall
 
    li   a7, 93          # syscall: exit
    li   a0, 0
    ecall
 
    .data

msg: .ascii "Hello, RISC-V\n"
