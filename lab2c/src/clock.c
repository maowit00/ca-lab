/**
 * @file Implements the main loop initializing components
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "hardware/ticks.h"

#include "clock.h"
#include "clock_cursor.h"
#include "clock_gui.h"
#include "clock_time.h"

int main()
{
    static uint64_t tick_us;

    static uint64_t last_print = 0;

    /* Initialize display */
    if (DEV_Module_Init() != 0)
        ERROR("DEV_Module_Init", -1);

    /* Initialize GUI */
    clock_gui_init();

    /* Define clickable areas for hours and minutes */
    box_t box_hour = {
        {75, 175},
        {110, 205}
    };

    box_t box_minute = {
        {115, 175},
        {150, 205}
    };

    /* Initialize cursor and joystick */
    clock_cursor_init(box_hour, box_minute);

    /* Draw initial GUI */
    clock_gui_update(0);

    /* Main loop */
    while (true)
    {
        bool update_gui = false;

        /* Get current time since boot */
        tick_us = time_us_64();

        /* Update cursor and check joystick input */
        if (clock_cursor_update(tick_us))
        {
            update_gui = true;
        }

        /* Get current editing state */
        clock_cursor_edit_t edit =
            clock_cursor_get_state(NULL, NULL);

        /* Update seconds only when not editing */
        if (edit == CURSOR_EDIT_NONE)
        {
            if (clock_time_inc_second(tick_us))
            {
                update_gui = true;
            }
        }

        /* Update GUI only when something has changed */
        if (update_gui)
        {
            clock_gui_update(tick_us);
        }


        if (tick_us - last_print > 300000) {          // every 300 ms, not every loop
            last_print = tick_us;
            pos_t p;
            clock_cursor_get_state(&p, NULL);
            printf("btn=%d cursor=(%d,%d) edit=%d\n",
                gpio_get(JOYSTICK_GPIO_BUTTON), p.x, p.y, edit);
        }
    }

    DEV_Module_Exit();

    return 0;
}
