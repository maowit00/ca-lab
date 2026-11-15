/**
 * @file Register a callback to the GPIO of a button, which You need to
 * initialized to either a external pull-down or an internal pull-up.
 *
 * This button is not debounced (no RC or RL filter in hardware).
 * Therefore You will need to debounce in software, using Pico's
 * low-level ticks interface or Pico's high-level time functions.
 *
 * The loop must count correctly the number of button presses.
 * Any button press > 200ms should be registered, any event shorter
 * than that filtered:
 * - Filtered button events should trigger light up only the RED LED
 * - Normal button presses should trigger light up only the GREEN LED
 * - Long button presses (>1s) should trigger light up only the BLUE LED
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_BUTTON    0
#define GPIO_LED_RED   3
#define GPIO_LED_GREEN 4
#define GPIO_LED_BLUE  5

int button_press = 0;
int last_button_press = 0;

/** TODO: You will need more data to communicate from Interrupt handler gpio_callback */

void gpio_callback(uint gpio, uint32_t events)
{
    /** TODO: Program filter routine for the evenst */
}

void main(void)
{
    stdio_init_all();

    /** TODO: Fill in Initialization */

    // Keep the busy loop outputting
    while (true)
    {
        if (last_button_press != button_press)
        {
            printf("Number of times button pressed:%d\n", button_press);
            last_button_press = button_press;
        }
        sleep_ms (20);
    }
}
