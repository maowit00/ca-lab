/*
 * hexToASCII.S - 32-bit value to "0xXXXXXXXX" string (Lab 1, Task 2.2)
 */
    .section .rodata
    .globl  H2A
H2A:
    .asciz  "0123456789ABCDEF"      # Nibble -> ASCII lookup table (ROM)

    .text

/**
 * \fn void hexToASCII(char *string, unsigned int val)
 * \brief Convert val into the NUL-terminated string "0x" + 8 hex digits.
 *
 * \param[out] string (a0) RAM buffer of at least 11 bytes
 * \param[in]  val    (a1) any 32-bit value
 * \return     void
 * Modified registers: none (t0..t3 are saved/restored on the stack).
 * Writes exactly 11 bytes to the buffer.
 */
    .globl  hexToASCII
    .type   hexToASCII, @function
hexToASCII:
    addi    sp, sp, -16
    sw      t0, 0(sp)               # Save registers used
    sw      t1, 4(sp)
    sw      t2, 8(sp)
    sw      t3, 12(sp)

    li      t0, '0'
    sb      t0, 0(a0)               # string[0] = '0'
    li      t0, 'x'
    sb      t0, 1(a0)               # string[1] = 'x'

    la      t1, H2A                 # t1 = table base
    addi    t3, a0, 2               # t3 = output pointer
    li      t2, 28                  # t2 = shift, starts at the top nibble
hex_loop:
    srl     t0, a1, t2              # Bring nibble to bits 3..0
    andi    t0, t0, 0xF
    add     t0, t0, t1
    lbu     t0, 0(t0)               # t0 = H2A[nibble]
    sb      t0, 0(t3)
    addi    t3, t3, 1
    addi    t2, t2, -4              # Next lower nibble
    bgez    t2, hex_loop

    sb      zero, 0(t3)             # string[10] = '\0'

    lw      t0, 0(sp)               # Restore registers
    lw      t1, 4(sp)
    lw      t2, 8(sp)
    lw      t3, 12(sp)
    addi    sp, sp, 16
    ret
    .size   hexToASCII, .-hexToASCII
