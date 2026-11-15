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

// - -- - -- - -- - -- - -- - --

#define DEBOUNCE_MS 20	   // edges this close after an accepted edge are bounce
#define MIN_PRESS_MS 200   // presses up to this length are filtered
#define LONG_PRESS_MS 1000 // presses longer than this are long presses

// - -- - -- - -- - -- - -- - --

#define GPIO_BUTTON 0
#define GPIO_LED_RED 5
#define GPIO_LED_GREEN 3
#define GPIO_LED_BLUE 4

static int button_press = 0;
static int last_button_press = 0;

// - -- - -- - -- - -- - -- - --

/**
 * @brief  Switch the RGB LED to exactly one color (or all off).
 *
 * @param[in] red    true = red LED on
 * @param[in] green  true = green LED on
 * @param[in] blue   true = blue LED on
 */
static void set_led(bool red, bool green, bool blue)
{
	gpio_put(GPIO_LED_RED, red);
	gpio_put(GPIO_LED_GREEN, green);
	gpio_put(GPIO_LED_BLUE, blue);
}

// - -- - -- - -- - -- - -- - --

/**
 * @brief  Interrupt handler for the button edges (press and release).
 *
 * Ignores edges closer than DEBOUNCE_MS to the last accepted edge. On release
 * it classifies the press: up to 200 ms filtered (RED), up to 1 s normal
 * (GREEN), longer than 1 s long press (BLUE).
 *
 * @param[in] gpio    Pin that raised the interrupt (only GPIO_BUTTON is enabled).
 * @param[in] events  Bit mask of GPIO_IRQ_EDGE_FALL / GPIO_IRQ_EDGE_RISE.
 *
 * @note   Runs in interrupt context: no printf, no sleep, keep it short.
 * @see    DEBOUNCE_MS, set_led()
 */
static void gpio_callback(uint gpio, uint32_t events)
{
	static volatile uint32_t last_edge_ms = 0;	 // time of the last accepted edge
	static volatile uint32_t press_start_ms = 0; // time of the accepted press
	static volatile bool button_down = false;	 // true between press and release

	uint32_t now = to_ms_since_boot(get_absolute_time());

	// Debounce: ignore edges that follow an accepted edge too closely
	if ((now - last_edge_ms) < DEBOUNCE_MS)
	{
		return;
	}

	last_edge_ms = now;

	if (events & GPIO_IRQ_EDGE_FALL)
	{
		// Button is active-low: falling edge = pressed
		press_start_ms = now;
		button_down = true;
		return;
	}

	if ((events & GPIO_IRQ_EDGE_RISE) && button_down)
	{
		// Rising edge = released: classify the press by its length
		button_down = false;

		uint32_t duration = now - press_start_ms;

		if (duration <= MIN_PRESS_MS)
		{
			set_led(1, 0, 0); // filtered: only RED, not counted
		}
		else
		{
			button_press++; // registered press: count it

			if (duration <= LONG_PRESS_MS)
			{
				set_led(0, 1, 0); // normal press: only GREEN
			}
			else
			{
				set_led(0, 0, 1); // long press: only BLUE
			}
		}
	}
}

// - -- - -- - -- - -- - -- - --

void main(void)
{
	stdio_init_all();

	// - -- - -- - -- - -- - -- - --

	/********** init GPIO **********/

	// init Button
	gpio_init(GPIO_BUTTON);
	gpio_set_dir(GPIO_BUTTON, GPIO_IN);
	gpio_set_pulls(GPIO_BUTTON, true, false);

	// registers as an interrupt service request
	gpio_set_irq_enabled_with_callback(
		GPIO_BUTTON,
		GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
		true,
		&gpio_callback
	);

	// LEDs
	gpio_init(GPIO_LED_RED);
	gpio_set_dir(GPIO_LED_RED, GPIO_OUT);

	gpio_init(GPIO_LED_GREEN);
	gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);

	gpio_init(GPIO_LED_BLUE);
	gpio_set_dir(GPIO_LED_BLUE, GPIO_OUT);

	// - -- - -- - -- - -- - -- - --

	// Keep the busy loop outputting
	while (true)
	{
		if (last_button_press != button_press)
		{
			printf("Number of times button pressed:%d\n", button_press);
			last_button_press = button_press;
		}
		sleep_ms(20);
	}
}