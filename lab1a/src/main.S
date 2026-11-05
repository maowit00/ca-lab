        .section const_section, "a"                        # Here comes a true read-only (const) data section for LED_PINS
        .align  2                                          # Aligned to 2^2 Bytes (aka 4 Bytes)
        .global .LED_PIN
        .type   .LED_PIN, @object                          # Mark the symbol as being a data object
.LED_PIN:                                                  # Define the array with the LED's GPIO pins
        .word   5
.equ    LED_PIN_BYTES, (. -.LED_PIN)                       # Constant! GAS trick: "." stores the current address (aka offset from beginning) in Bytes

        .text                                              # Mark the next section as .text/.code section
        .align  1                                          # Aligned to 2^1 Bytes (aka 2 Bytes)
        .global  main                                      # Defines a globally-visible (exported) symbol "main", visible to the linker ld
main:                                                      # The location of the Symbol "main": here, the function starts
        # 0. Safe the used callee-saved registers ra, s0-s2 onto the stack
        addi    sp, sp, -12                                # Allocate data on the stack
        sw      ra, 0(sp)                                  # Store the return address (ra) register on the stack
        sw      s1, 4(sp)                                  # Store the saved register s1 on the stack
        sw      s2, 8(sp)                                  # Store the saved register s2 on the stack

        # 1. Initialization: Use s1 as pointer to LED_PINS, while s2 is the end-pointer (both unchanged below)
        lui     s1, %hi(.LED_PIN)                          # Initialize with Pointer to first element by loading upper 20-bit
        addi    s1, s1, %lo(.LED_PIN)                      # and adding the lower 12 bit with GAS macros %hi() and %lo()
        lw      a0, 0(s1)                                  # Load the word at s1[0], aka dereference s1 into a0
        jal     led_init                                   # Jump-and-link to function led_init

        # Initialize the counter in s2 register
        li      s2, 0

        # 3. Loop forever and blink these LEDs
.main_loop:                                                # This symbol starting with "." is by definition local

        andi    t0, s2, 1       # use lower bit of counter as the LED status

        lw      a0, 0(s1)       # read call to set led

        bnez    t0, .turn_led_on  # if 1 skip to turn led on

        .turn_led_off:
                call led_off
                j .delay

        .turn_led_on:
                call led_on

        .delay:
                call my_delay

        # - -- - -- - -- - --

        ## Setup next loop

        addi    s2, s2, 1        # increment counter
        andi    s2, s2, 63       # mask lower bits to roll over for 64

        # - -- - -- - -- - --

        j       .main_loop                                 # Loop forever


        # ####################################### #
        # In fact, the following clean-up code will never be executed... Still keep it, to show best practice
        lw      s2, 8(sp)                                  # Restore the saved register s2 from the stack
        lw      s1, 4(sp)                                  # Restore the saved register s1 from the stack
        lw      ra, 0(sp)                                  # Restore the saved register ra from the stack
        addi    sp, sp, 12                                 # Clean up the stack-pointer
        ret                                                # Return to the caller
        
