    .globl _start

# ------------------------------------------------------------

    .macro exit code
        li   a0, \code           # \code is replaced by the argument
        li   a7, 93              # sys_exit
        ecall
    .endm

# Print the unsigned number in \reg as decimal followed by a newline.
# Uses the 16-byte buffer at sp. Clobbers a0-a2, t0-t5.
    .macro print_num reg
        mv   a0, sp              # write position
        mv   a1, \reg            # value
        call print_dec           # returns a0 = next free position
        li   t0, '\n'
        sb   t0, 0(a0)           # append newline
        addi a0, a0, 1
        sub  a1, a0, sp          # length = end - start
        mv   a0, sp
        call print
    .endm

# ------------------------------------------------------------

    .data

    # read-write (nothing yet)

# ------------------------------------------------------------

    .section .rodata

# ASCII digits lookup table (no null terminator)
DIGITS:
    .ascii "0123456789"

# Test string
message:
    .asciz "Hello, World"

message_length = . - message - 1 # excludes the null terminator (12)

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
    # Prints a1 as unsigned decimal, no leading zeros ("0" for 0).
    # a0 = where to start writing
    # a1 = value to write
    # returns a0 = next position after the written digits
    # clobbers a1, a2, t0-t5

    li   t5, 10                  # t5 = divisor
    la   t3, DIGITS              # t3 = table base

    # Pass 1: count the digits (at least one)
    mv   t0, a1
    li   a2, 0
1:  divu t0, t0, t5
    addi a2, a2, 1
    bnez t0, 1b                  # a2 = number of digits

    # Pass 2: write the digits right to left
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

str_len:
    # a0 = pointer to null-terminated string
    # returns a0 = length (without the terminator)
    # clobbers t0, t1

    mv   t0, a0                  # t0 = cursor
1:  lbu  t1, 0(t0)
    addi t0, t0, 1               # increments first
    beqz t1, 2f                  # null already counted
    j    1b
2:  sub  a0, t0, a0              # length = cursor - start
    ret

# ------------------------------------------------------------

_start:
    addi sp, sp, -16             # reserve buffer (kept 16-byte aligned)

    # Length computed at runtime (s1 survives the calls below)
    la   a0, message
    call str_len
    mv   s1, a0

    # Length computed by the assembler
    li   s2, message_length

    # Print the message followed by a newline
    la   a0, message
    li   a1, message_length
    call print

    li   t0, '\n'
    sb   t0, 0(sp)
    mv   a0, sp
    li   a1, 1
    call print

    # Print both lengths
    print_num s1                 # from str_len
    print_num s2                 # from the location counter

    addi sp, sp, 16              # release the stack buffer

    exit 0
