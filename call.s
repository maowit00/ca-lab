
    .globl _start

# - -- - -- - --


    .macro exit code
        li   a0, \code       # \code is replaced by the argument
        li   a7, 93          # sys_exit
        ecall
    .endm

# - -- - -- - --

    .data

msg: .ascii "Hello, RISC-V\n"
.equ msg_len, . - msg        # length computed by the assembler (14)

# *CLC (Current Location Counter) ;)

# - -- - -- - -- - -- - -- - -- - -- - --

    .text

print:                       # a0 = ptr, a1 = len
    mv   a2, a1              # len -> a2
    mv   a1, a0              # ptr -> a1
    li   a0, 1               # fd = stdout
    li   a7, 64              # syscall: write
    ecall

    ret

# - -- - -- - -- -

_start:

    la   a0, msg
    li   a1, msg_len
    call print

    # - -- - -- - -- - -- - -- - --

    exit 0
