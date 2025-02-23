#include "pico/stdlib.h"
#include "hardware/adc.h"

static const uint HEAT_SELECTOR = 1;
static const uint HEAT_PIN = 27;
static const uint MEASUREMENT_CORRECTION = 10;
static const float CENTER_VALUE = 2048.0;

void heat_sensor_init() {
    adc_init();
    adc_gpio_init(HEAT_PIN);
    adc_select_input(HEAT_SELECTOR);
}

uint heat_sensor_read() {
    sleep_us(2);
    uint value = adc_read();
    return 25 + (int) 3*(value - CENTER_VALUE)/CENTER_VALUE;
}