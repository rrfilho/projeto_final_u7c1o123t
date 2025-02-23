#include "hardware/pwm.h"

static const uint HEATER_PIN = 12;
static const float CLOCK_DIVIDER = 10.0;
static const uint WRAP = 1024;

void pool_heater_init() {
    gpio_set_function(HEATER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(HEATER_PIN);
    pwm_set_clkdiv(slice, CLOCK_DIVIDER);
    pwm_set_wrap(slice, WRAP);
    pwm_set_gpio_level(HEATER_PIN, 0);
    pwm_set_enabled(slice, true);
}

static void pool_heater_set_power(float power) {
    uint level = WRAP * power/100.0;
    pwm_set_gpio_level(HEATER_PIN, level);
}

void pool_heater_warm() {
    pool_heater_set_power(50.0f);
}

void pool_heater_keep() {
    pool_heater_set_power(10.0f);
}

void pool_heater_off() {
    pool_heater_set_power(0.0f);
}