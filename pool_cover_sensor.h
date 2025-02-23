// sensor de indução
#include "buttons.h"

static const uint POOL_COVER_SENSOR = 22;

static handler _pool_cover_sensor_handler;
static volatile uint pool_cover_sensor_last_time_us = 0;

static void pool_cover_sensor_handler_with_debounce() {
    if (!has_wait_time_reached(pool_cover_sensor_last_time_us)) return;
    _pool_cover_sensor_handler();
    pool_cover_sensor_last_time_us = now_us();
}

void pool_cover_sensor_init(handler pool_cover_sensor_handler) {
    _pool_cover_sensor_handler = pool_cover_sensor_handler;
    button_init(POOL_COVER_SENSOR, pool_cover_sensor_handler_with_debounce);
}