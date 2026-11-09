
    .text
    .globl _start
_start:
    call main          # test's main returns 0 = pass, nonzero = fail
    li   a7, 93        # Linux exit syscall
    ecall
