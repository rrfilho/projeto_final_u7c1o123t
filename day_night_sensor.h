// ou LIGHT DEPENDENT SWITCH
#include "buttons.h"

static const uint DAY_NIGHT_SENSOR = 6;

static handler _day_night_sensor_handler;
static volatile uint day_night_sensor_last_time_us = 0;

static void day_night_sensor_handler_with_debounce() {
    if (!has_wait_time_reached(day_night_sensor_last_time_us)) return;
    _day_night_sensor_handler();
    day_night_sensor_last_time_us = now_us();
}

void day_night_sensor_init(handler day_night_sensor_handler) {
    _day_night_sensor_handler = day_night_sensor_handler;
    button_init(DAY_NIGHT_SENSOR, day_night_sensor_handler_with_debounce);
}