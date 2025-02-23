#include "buttons.h"

static const uint POOL_SYSTEM_BUTTON = 5;

static handler _pool_system_button_handler;
static volatile uint pool_system_button_last_time_us = 0;

static void pool_system_button_handler_with_debounce() {
    if (!has_wait_time_reached(pool_system_button_last_time_us)) return;
    _pool_system_button_handler();
    pool_system_button_last_time_us = now_us();
}

void pool_system_button_init(handler pool_system_button_handler) {
    _pool_system_button_handler = pool_system_button_handler;
    button_init(POOL_SYSTEM_BUTTON, pool_system_button_handler_with_debounce);
}