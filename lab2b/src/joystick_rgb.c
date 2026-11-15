/**
 * \file Make the Joystick steer the RGB-LED.
 * The Joystick has two potentionmeter for the X- and the Y-axis.
 * These may be read out using the GPIO pins that support converting
 * voltages into digital values using the ADC component with it's
 * 12-bit quantization using successive-approximation ADC (SAR).
 *
 * Read out the X-axis to color-code the RGB LED in a loop.
 * 
 * With Pulse-Width-Modulation (PWM) one may define a duty-cycle
 * from 0 to 100% (0 to 254) on a GPIO initialized to output using
 * a digital signal with PWM. Here the actual timing is not relevant
 * (in comparison to servo-motors and alike).
 * You shall light the PWM from RED to BLUE: 100% RED when the X-axis
 * is at the maximum deflection, and 100% BLUE when the X-axis is at 
 * the lowest/minimum deflection.
 * Instead of any color-correction / brightness-correction, just linearly
 * scale between those two value, as in BLUE=100-RED.
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define GPIO_X         26
#define GPIO_Y         27
#define GPIO_LED_RED 5
#define GPIO_LED_GREEN 3
#define GPIO_LED_BLUE 4

// - -- - -- - -- - -- - -- - -- - -- - -- - --

int main()
{
	stdio_init_all(); // init Serial

	/******* Initialize ADC *******/

	adc_init();
	adc_gpio_init(GPIO_X);

	// - -- - -- - -- - -- - -- - --

	/** Initialize PWM for the LEDs **/

	gpio_set_function(GPIO_LED_RED, GPIO_FUNC_PWM);
	gpio_set_function(GPIO_LED_GREEN, GPIO_FUNC_PWM);
	gpio_set_function(GPIO_LED_BLUE, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(GPIO_LED_RED);
	pwm_set_wrap(slice_num, ((1 << 12) - 1));
	pwm_set_enabled(slice_num, true);

	slice_num = pwm_gpio_to_slice_num(GPIO_LED_GREEN);
	pwm_set_wrap(slice_num, ((1 << 12) - 1));
	pwm_set_enabled(slice_num, true);

	slice_num = pwm_gpio_to_slice_num(GPIO_LED_BLUE);
	pwm_set_wrap(slice_num, ((1 << 12) - 1));
	pwm_set_enabled(slice_num, true);

	// - -- - -- - -- - -- - -- - --

	while (true)
	{
		uint16_t x;
		// 12-bit conversion, assume max value == ADC_VREF == 3.3 V
		const float conversion_factor = 3.3f / ((1 << 12) - 1);
		

		/* Read out the joysticks X-Value */
		adc_select_input(0);
		x = adc_read();

		/*
			Change the PWM-Level
			from 100% RED at maximum X - Level
			to 100% BLUE at minium
		*/
		pwm_set_gpio_level(GPIO_LED_RED, x);

		pwm_set_gpio_level(
			GPIO_LED_BLUE,
			((1 << 12) - 1) - x
		);

		// console debug over UART
		printf(
			"Raw joystick value: x:%4d (%1.3f V)\n",
			x, x * conversion_factor
		);
		sleep_ms(500);
	}
}
