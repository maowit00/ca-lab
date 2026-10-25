    .globl _start

    .equ LIMIT, 16               # print 0x00 .. LIMIT)

# ------------------------------------------------------------

    .macro exit code
        li   a0, \code           # \code is replaced by the argument
        li   a7, 93              # sys_exit
        ecall
    .endm

# ------------------------------------------------------------

    .section .rodata

# ASCII digits lookup table (no null terminator)
DIGITS:
    .ascii "0123456789"

# ------------------------------------------------------------

    .text

print:                           # a0 = ptr, a1 = len
    mv   a2, a1                  # len -> a2
    mv   a1, a0                  # ptr -> a1
    li   a0, 1                   # fd = stdout
    li   a7, 64                  # syscall: write
    ecall
    ret

# ------------------------------------------------------------

print_dec:
    # Like printf("%0*d") for one number.
    # a0 = where to start writing
    # a1 = value to write
    # a2 = number of decimal digits (assumes > 0)
    # returns a0 = next position after the written digits
    # clobbers a1, a2, t0-t5

    la   t3, DIGITS              # t3 = table base
    li   t5, 10                  # t5 = divisor
    add  a0, a0, a2              # a0 = one past the last digit
    mv   t4, a0                  # remember it as the return value

write_digit:
    remu t0, a1, t5              # t0 = a1 % 10 (lowest digit)
    divu a1, a1, t5              # a1 /= 10 (discard that digit)

    add  t1, t3, t0              # &DIGITS[t0]
    lbu  t2, 0(t1)               # t2 = DIGITS[t0]
    addi a0, a0, -1              # move back one slot
    sb   t2, 0(a0)               # write the digit
    addi a2, a2, -1
    bnez a2, write_digit         # loop until all digits are written

    mv   a0, t4                  # return next free position
    ret

# ------------------------------------------------------------

_start:
    addi sp, sp, -16             # reserve buffer (kept 16-byte aligned)

    li   t0, '\n'
    sb   t0, 2(sp)               # bytes 0..1 are filled with the digits

    li   s1, 0                   # s1 = loop counter (print/print_dec don't touch it)

loop:
    mv   a0, sp                  # digits go at offset 0
    mv   a1, s1                  # value to print
    li   a2, 2                   # two decimal digits
    call print_dec

    mv   a0, sp
    li   a1, 3                   # "00\n" = 3 bytes
    call print

    addi s1, s1, 1
    li   t0, LIMIT
    blt  s1, t0, loop

    addi sp, sp, 16              # release the stack buffer

    exit 0