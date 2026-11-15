/**
 * @file Implements the cursor allowing to set the time:
 *  - when moving the Joystick, a cursor (two lines) should appear
 *    (see clock_gui.c)
 *  Using the joystack, one shall edit (change) the hour and minute:
 *  - when not in editing mode:
 *    - Pulling the joystick position of the cursor should change:
 *       pull up: decrease y  /  pull down: increase y
 *       pull left: decrease x / pull right: increase x
 *    - when clicking on hour / minute: Select Edit mode for hour or minute
 *  - when in editing mode:
 *    - Pulling the joystick should change the hour / minute:
 *       pull up: increase hour or minute / pull down: decrease hour or minute
 *    - When clicking: Get out of Editing mode
 */
#include <stdio.h>
#include <stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

#include "clock.h"
#include "clock_time.h"
#include "clock_cursor.h"

#define CURSOR_VISIBLE_TIME (5 * 1000 * 1000) // 5 seconds

/*
 * ADC *input channels*, NOT gpio numbers!
 * GPIO 26 (JOYSTICK_GPIO_X) is ADC channel 0, GPIO 27 (JOYSTICK_GPIO_Y) is
 * channel 1. adc_gpio_init() takes the GPIO number, adc_select_input() takes
 * the channel (see lab2b: adc_select_input(0)).
 */
#define JOYSTICK_ADC_X 0
#define JOYSTICK_ADC_Y 1

/*
 * Button wiring. The working example uses no pull resistor and treats HIGH
 * as "pressed". If your button reads 0 when pressed instead, set
 * BUTTON_PRESSED_LEVEL to 0 and BUTTON_PULL_UP to 1.
 */
#define BUTTON_PRESSED_LEVEL 1
#define BUTTON_PULL_UP       0

/* Which way the stick changes the time: +1 = down increases, -1 = up increases */
#define EDIT_DIRECTION (+1)

/* ADC counts per step: smaller = faster. Time editing needs a bigger push. */
#define CURSOR_ADC_DIVISOR   200
#define TIMECHANGE_ADC_DIVISOR 800

/*
 * The cursor's current state: position, mode of edit, visibility and
 * when the stick was last moved.
 */
static pos_t cursor_pos = {120, 120};
static clock_cursor_edit_t cursor_edit = CURSOR_EDIT_NONE;
static bool cursor_visible = false;
static uint64_t last_change = 0;

static box_t hour_area;
static box_t minute_area;

/* -------------------------------------------------- */

static bool is_in_box(pos_t pos, box_t box)
{
    return (pos.x > box.upper_left.x && pos.y > box.upper_left.y &&
            pos.x < box.lower_right.x && pos.y < box.lower_right.y);
}

static int clamp(int value, int low, int high)
{
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

/* Wrap value into 0 .. range-1, also for negative values */
static int wrap(int value, int range)
{
    return ((value % range) + range) % range;
}

/*
 * Convert an ADC readout (0..4095, centre ~2048) into a number of steps.
 * Small deflections (stick noise) give 0: the first step needs a deflection
 * of 100 + divisor counts (300 for the cursor, 900 for time editing).
 * Negative = stick pulled to the low side.
 */
static int adc_to_steps(uint16_t adc, int divisor)
{
    int delta = (int)adc - 2048;
    int magnitude = abs(delta);

    if (magnitude < 100)
        return 0;

    int steps = (magnitude - 100) / divisor;
    return (delta < 0) ? -steps : steps;
}

static bool button_pressed(void)
{
    return gpio_get(JOYSTICK_GPIO_BUTTON) == (BUTTON_PRESSED_LEVEL != 0);
}

/* -------------------------------------------------- */

void clock_cursor_init(box_t box_hour, box_t box_minute)
{
    adc_init();
    adc_gpio_init(JOYSTICK_GPIO_X);
    adc_gpio_init(JOYSTICK_GPIO_Y);

    gpio_init(JOYSTICK_GPIO_BUTTON);
    gpio_set_dir(JOYSTICK_GPIO_BUTTON, GPIO_IN);
    gpio_set_pulls(JOYSTICK_GPIO_BUTTON, BUTTON_PULL_UP, false);

    cursor_pos.x = 120;
    cursor_pos.y = 120;
    cursor_edit = CURSOR_EDIT_NONE;
    cursor_visible = false;
    last_change = 0;

    hour_area = box_hour;
    minute_area = box_minute;
}

/* -------------------------------------------------- */

/* Normal mode: move the cursor, fade it, and enter edit mode by hovering */
static bool update_cursor(uint64_t tick_us)
{
    bool changed = false;

    adc_select_input(JOYSTICK_ADC_X);
    int delta_x = adc_to_steps(adc_read(), CURSOR_ADC_DIVISOR);

    adc_select_input(JOYSTICK_ADC_Y);
    int delta_y = adc_to_steps(adc_read(), CURSOR_ADC_DIVISOR);

    if (delta_x != 0 || delta_y != 0)
    {
        last_change = tick_us;
        changed = true;
    }

    /* Fading out after a few seconds needs a redraw, too */
    if (cursor_visible && (tick_us - last_change > CURSOR_VISIBLE_TIME))
        changed = true;
    cursor_visible = (tick_us - last_change <= CURSOR_VISIBLE_TIME);

    cursor_pos.x = clamp(cursor_pos.x + delta_x, 0, 240);
    cursor_pos.y = clamp(cursor_pos.y + delta_y, 0, 240);

    /* Cursor over HH or MM selects that field for editing */
    if (is_in_box(cursor_pos, hour_area))
        cursor_edit = CURSOR_EDIT_HOUR;
    if (is_in_box(cursor_pos, minute_area))
        cursor_edit = CURSOR_EDIT_MINUTE;

    /* The field now has to be redrawn in blue */
    if (cursor_edit != CURSOR_EDIT_NONE)
        changed = true;

    return changed;
}

/* Edit mode: change hour / minute with the stick, leave with the button */
static bool update_time_edit(void)
{
    bool changed = false;

    adc_select_input(JOYSTICK_ADC_Y);
    int step = EDIT_DIRECTION * adc_to_steps(adc_read(), TIMECHANGE_ADC_DIVISOR);

    /* Park the cursor in the middle and hide it while editing */
    if (cursor_visible || cursor_pos.x != 120 || cursor_pos.y != 120)
        changed = true;
    cursor_pos.x = 120;
    cursor_pos.y = 120;
    cursor_visible = false;

    if (step != 0)
    {
        int hour, minute, second;
        clock_time_get_utc(&hour, &minute, &second);

        if (cursor_edit == CURSOR_EDIT_HOUR)
            hour = wrap(hour + step, 24);
        else if (cursor_edit == CURSOR_EDIT_MINUTE)
            minute = wrap(minute + step, 60);

        clock_time_set_utc(hour, minute, second);
        changed = true;
    }

    if (button_pressed())
    {
        cursor_edit = CURSOR_EDIT_NONE;
        changed = true;
    }

    return changed;
}

/* -------------------------------------------------- */

bool clock_cursor_update(uint64_t tick_us)
{
    if (cursor_edit != CURSOR_EDIT_NONE)
        return update_time_edit();

    return update_cursor(tick_us);
}

clock_cursor_edit_t clock_cursor_get_state(pos_t *pos, bool *visible)
{
    if (NULL != pos)
    {
        pos->x = cursor_pos.x;
        pos->y = cursor_pos.y;
    }
    if (NULL != visible)
    {
        *visible = cursor_visible;
    }
    return cursor_edit;
}