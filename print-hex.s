
    .globl _start

    .equ LIMIT, 16             # print 0x00 .. LIMIT)

# ------------------------------------------------------------

    .macro exit code
        li   a0, \code           # \code is replaced by the argument
        li   a7, 93              # sys_exit
        ecall
    .endm

# ------------------------------------------------------------

    .section .rodata

# Hex-print lookup table (no null terminator)
HEX_SPEECH:
    .ascii "0123456789ABCDEF"

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

print_hex:
    # Like printf("%0*x") for one number.
    # a0 = where to start writing
    # a1 = value to write
    # a2 = number of hex digits (assumes > 0)
    # returns a0 = next position after the written digits
    # clobbers a1, a2, t0-t4

    la   t3, HEX_SPEECH          # t3 = table base
    add  a0, a0, a2              # a0 = one past the last digit
    mv   t4, a0                  # remember it as the return value

    write_nibble:
        andi t0, a1, 0xF             # lowest 4 bits
        srli a1, a1, 4               # discard them for the next iteration
        add  t1, t3, t0              # &HEX_SPEECH[t0]
        lbu  t2, 0(t1)               # t2 = HEX_SPEECH[t0]
        addi a0, a0, -1              # move back one slot
        sb   t2, 0(a0)               # write the digit
        addi a2, a2, -1
        bnez a2, write_nibble        # loop back if digits required is not 0 yet

    mv   a0, t4                  # return next free position
    ret

# ------------------------------------------------------------

_start:
    addi sp, sp, -16             # reserve buffer (kept 16-byte aligned)

    li   t0, '0'
    sb   t0, 0(sp)
    li   t0, 'x'
    sb   t0, 1(sp)
    li   t0, '\n'
    sb   t0, 4(sp)               # bytes 2..3 are filled with the digits

    li   s1, 0                   # s1 = loop counter (print/print_hex don't touch it)

loop:
    addi a0, sp, 2               # digits go at offset 2
    mv   a1, s1                  # value to print
    li   a2, 2                   # two hex digits
    call print_hex

    mv   a0, sp
    li   a1, 5                   # "0x00\n" = 5 bytes
    call print

    addi s1, s1, 1
    li   t0, LIMIT
    blt  s1, t0, loop

    addi sp, sp, 16              # release the stack buffer

    exit 0