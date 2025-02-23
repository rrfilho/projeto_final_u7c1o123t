#pragma once

#include "pico/stdlib.h"
#include "hardware/timer.h"

typedef void (*handler)();

static struct repeating_timer timer;
static handler timer_handler;
static handler alarm_handler;

static bool timer_handler_adapter(repeating_timer_t *t) {
    timer_handler();
    return true;
}

void timer_set_repeating(uint interval_ms, handler handler) {
    timer_handler = handler;
    add_repeating_timer_ms(interval_ms, timer_handler_adapter, NULL, &timer);
}

static int64_t alarm_handler_adapter(alarm_id_t id, void *user_data) {
    printf("alarm id = %d\n", id);
    alarm_handler();
    return 0;
}

uint timer_set_alarm(uint delay, handler handler) {
    alarm_handler = handler;
    return add_alarm_in_ms(delay, alarm_handler_adapter, NULL, false);
}

void timer_cancel_alarm(uint alarm_id) {
    cancel_alarm(alarm_id);
}
