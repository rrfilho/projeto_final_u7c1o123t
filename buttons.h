#pragma once

#include "pico/stdlib.h"

#define TOTAL_PINS 26
#define DEBOUNCE_TIME_LIMIT_US 200000

typedef void (*handler)();

static handler handlers[TOTAL_PINS];

uint now_us() {
    return to_us_since_boot(get_absolute_time());
}

bool has_wait_time_reached(uint last_time) {
    return now_us() - last_time >= DEBOUNCE_TIME_LIMIT_US;
}

static void handler_adapter(uint gpio, uint32_t event_mask) {
    handlers[gpio-1]();
}

void button_init(uint button_pin, handler handler) {
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);
    handlers[button_pin-1] = handler;
    gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_FALL, true, handler_adapter);
}