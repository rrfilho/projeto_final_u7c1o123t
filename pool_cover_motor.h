#include "pico/stdlib.h"
#include "timer.h"

static const uint MAX_WAIT_STOP_COMMAND = 10000;
static const uint MOTOR_UNCOVERING = 11;
static const uint MOTOR_COVERING = 13;
static uint covering_alarm_id = 0;
static uint uncovering_alarm_id = 0;

void pool_motor_init() {
    gpio_init(MOTOR_UNCOVERING);
    gpio_set_dir(MOTOR_UNCOVERING, GPIO_OUT);
    gpio_init(MOTOR_COVERING);
    gpio_set_dir(MOTOR_COVERING, GPIO_OUT);
}

void pool_motor_stop_uncovering() {
    gpio_put(MOTOR_UNCOVERING, false);
}

void pool_motor_start_uncovering() {
    gpio_put(MOTOR_COVERING, false);
    gpio_put(MOTOR_UNCOVERING, true);
    timer_cancel_alarm(uncovering_alarm_id);
    uncovering_alarm_id = timer_set_alarm(MAX_WAIT_STOP_COMMAND, pool_motor_stop_uncovering);
}

void pool_motor_stop_covering() {
    gpio_put(MOTOR_COVERING, false);
}

void pool_motor_start_covering() {
    gpio_put(MOTOR_UNCOVERING, false);
    gpio_put(MOTOR_COVERING, true);
    timer_cancel_alarm(covering_alarm_id);
    covering_alarm_id = timer_set_alarm(MAX_WAIT_STOP_COMMAND, pool_motor_stop_covering);
}