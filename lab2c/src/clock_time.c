/**
 * @file Time (and later date?) handling
 */
#include <pico/stdlib.h>
#include "clock_time.h"

/********************** Type definitions **********************/
typedef struct timezone_def {
    char name_short[8];
    char name[32];
    int hour_offset_UTC;
} timezone_def_t;


/********************** Local variables **********************/
static int seconds = CLOCK_TIME_START_SECOND;
static int minutes = CLOCK_TIME_START_MINUTE;
static int hours = CLOCK_TIME_START_HOUR;

static int timezone_offset_hours = CLOCK_TIME_START_OFFSET_HOUR;
static uint64_t last_tick_us = 0;

static timezone_def_t timezone_def[] = {
    {"UTC",   "Coordinated Universal Time", 0},
    {"WET",   "Western European Time", 0},
    {"WEST",  "Western European Summer Time", 1},
    {"EST",   "Eastern Standard Time", -5}
};


/********************** Time functions **********************/

bool clock_time_inc_second(uint64_t tick_us)
{
    if ((tick_us - last_tick_us) < (1000 * 1000))
        return false;

    last_tick_us += (1000 * 1000);

    if (seconds++ == 59)
    {
        seconds = 0;

        if (minutes++ == 59)
        {
            minutes = 0;

            if (hours++ == 23)
                hours = 0;
        }
    }

    return true;
}


void clock_time_set_utc(int hour, int minute, int second)
{
    assert(hour >= 0 && hour < 24);
    assert(minute >= 0 && minute < 60);
    assert(second >= 0 && second < 60);

    hours = hour;
    minutes = minute;
    seconds = second;
}


void clock_time_get_utc(int *hour, int *minute, int *second)
{
    assert(hour != NULL);
    assert(minute != NULL);
    assert(second != NULL);

    *hour = hours;
    *minute = minutes;
    *second = seconds;
}


void clock_time_get_local(int *hour, int *minute, int *second)
{
    assert(hour != NULL);
    assert(minute != NULL);
    assert(second != NULL);

    /* Calculate local time and handle negative timezone offsets */
    *hour = (hours + timezone_offset_hours + 24) % 24;

    *minute = minutes;
    *second = seconds;
}


void clock_time_set_timezone(timezones_t tz)
{
    assert(tz >= 0 && tz < TIMEZONE_COUNT);

    timezone_offset_hours = timezone_def[tz].hour_offset_UTC;
}


/********************** Joystick time editing **********************/

void clock_time_change_hour_utc(int change_value)
{
    hours = (hours + change_value) % 24;

    if (hours < 0)
        hours += 24;
}


void clock_time_change_minute_utc(int change_value)
{
    minutes = (minutes + change_value) % 60;

    if (minutes < 0) minutes += 60;
}
