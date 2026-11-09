/*
 * stringfct.S - ASCIIZ string utility functions (Lab 1, Task 2.1)
 *
 * Functions: my_strtolower, my_strlen, my_strcpy
 * All use only caller-saved registers (a0-a7, t0-t6). ra, sp and s0..s11
 * are never touched. No global variables are used.
 */
    .text

/**
 * \fn int my_strlen(char *string)
 * \brief Number of characters in an ASCIIZ string (without the NUL byte).
 *
 * \param[in]  string  (a0) pointer to NUL-terminated string
 * \return     (a0) length of the string
 * Modified registers: a0, t0, t1
 */
    .globl  my_strlen
    .type   my_strlen, @function
my_strlen:
    mv      t0, a0                  # t0 = running pointer
strlen_loop:
    lbu     t1, 0(t0)               # Fetch next character
    beqz    t1, strlen_done         # NUL byte -> end of string
    addi    t0, t0, 1
    j       strlen_loop
strlen_done:
    sub     a0, t0, a0              # Length = end pointer - start pointer
    ret
    .size   my_strlen, .-my_strlen

/**
 * \fn int my_strtolower(char *string)
 * \brief Convert 'A'..'Z' to lower case in place; all other bytes unchanged.
 *
 * Only the range 'A'..'Z' is converted (a blind OR 0x20 would corrupt
 * characters such as '@', '[' or '_').
 *
 * \param[in,out] string (a0) pointer to NUL-terminated string in RAM
 * \return        (a0) length of the string
 * Modified registers: a0, t0, t1, t2, t3
 */
    .globl  my_strtolower
    .type   my_strtolower, @function
my_strtolower:
    mv      t0, a0                  # t0 = running pointer
    li      t3, 'Z' - 'A'           # Width of the upper case block
tolower_loop:
    lbu     t1, 0(t0)               # Fetch next character
    beqz    t1, tolower_done        # NUL byte -> end of string
    addi    t2, t1, -'A'            # Offset into the upper case block
    bgtu    t2, t3, tolower_next    # Not 'A'..'Z' -> leave unchanged
    ori     t1, t1, 0x20            # Set bit 5 -> lower case
    sb      t1, 0(t0)               # Store converted character
tolower_next:
    addi    t0, t0, 1               # Handle next character
    j       tolower_loop
tolower_done:
    sub     a0, t0, a0              # Return length
    ret
    .size   my_strtolower, .-my_strtolower

/**
 * \fn void my_strcpy(char *destination, char *source)
 * \brief Copy an ASCIIZ string including the terminating NUL.
 *
 * \param[out] destination (a0) RAM buffer, at least strlen(source)+1 bytes
 * \param[in]  source      (a1) pointer to NUL-terminated string
 * \return     void (a0 and a1 are left unchanged)
 * Modified registers: t0, t1, t2
 */
    .globl  my_strcpy
    .type   my_strcpy, @function
my_strcpy:
    mv      t0, a0                  # t0 = destination pointer
    mv      t1, a1                  # t1 = source pointer
strcpy_loop:
    lbu     t2, 0(t1)               # Fetch next character
    sb      t2, 0(t0)               # Store it (also the final NUL)
    addi    t0, t0, 1
    addi    t1, t1, 1
    bnez    t2, strcpy_loop         # Continue until NUL was copied
    ret
    .size   my_strcpy, .-my_strcpy
