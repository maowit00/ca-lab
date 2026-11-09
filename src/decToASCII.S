/*
 * decToASCII.S - signed 32-bit value to decimal string (Lab 1, Task 2.3)
 */
    .text

/**
 * \fn void decToASCII(char *string, int val)
 * \brief Convert val to a fixed-width, NUL-terminated decimal string.
 *
 * Format: sign (' ' or '-') + 10 digits (leading zeros kept) + NUL,
 * e.g. " 0000000042", "-0000000001", "-2147483648".
 *
 * \param[out] string (a0) RAM buffer of at least 12 bytes
 * \param[in]  val    (a1) -2147483648 .. 2147483647
 * \return     void
 * Modified registers: none (t0..t5 are saved/restored on the stack).
 * No global variables are accessed.
 */
    .globl  decToASCII
    .type   decToASCII, @function
decToASCII:
    addi    sp, sp, -32
    sw      t0, 0(sp)               # Save registers used
    sw      t1, 4(sp)
    sw      t2, 8(sp)
    sw      t3, 12(sp)
    sw      t4, 16(sp)
    sw      t5, 20(sp)

    mv      t0, a1                  # t0 = value to convert
    li      t5, ' '                 # Assume positive: sign = ' '
    bgez    t0, dec_sign
    li      t5, '-'                 # Negative: sign = '-'
    neg     t0, t0                  # t0 = magnitude (-2^31 stays 0x80000000,
                                    # correct when treated as unsigned below)
dec_sign:
    sb      t5, 0(a0)               # string[0] = sign

    addi    t2, a0, 1               # t2 = output pointer
    li      t1, 1000000000          # t1 = divisor, 10^9 down to 10^0
    li      t4, 10
dec_loop:
    divu    t3, t0, t1              # Q = val / divisor  (digit 0..9)
    remu    t0, t0, t1              # val = R
    addi    t3, t3, '0'             # Digit -> ASCII
    sb      t3, 0(t2)
    addi    t2, t2, 1
    divu    t1, t1, t4              # divisor /= 10
    bnez    t1, dec_loop            # Until 10^0 has been processed

    sb      zero, 0(t2)             # string[11] = '\0'

    lw      t0, 0(sp)               # Restore registers
    lw      t1, 4(sp)
    lw      t2, 8(sp)
    lw      t3, 12(sp)
    lw      t4, 16(sp)
    lw      t5, 20(sp)
    addi    sp, sp, 32
    ret
    .size   decToASCII, .-decToASCII
