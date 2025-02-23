#include <stdio.h>
#include "pico/stdlib.h"
#include "pool_system_button.h"
#include "day_night_sensor.h"
#include "pool_cover_sensor.h"
#include "pool_lights.h"
#include "pool_cover_motor.h"
#include "heat_sensor.h"
#include "pool_heater.h"
#include "pool_display_layout.h"

volatile bool isActive = false;
volatile bool isNight = false;
uint temperature;

void pool_system_on() {
    pool_motor_start_uncovering();
    pool_lights_set_status(isNight);
}

void pool_system_off() {
    pool_motor_start_covering();
    pool_lights_set_status(false);
    pool_heater_off();
}

void on_pool_system_button_click() {
    isActive = !isActive;
    if (isActive) pool_system_on();
    else pool_system_off();
}

void on_day_night_sensor_change() {
    isNight = !isNight;
    pool_lights_set_status(isActive && isNight);
}

void on_pool_cover_sensor() {
    if (isActive) pool_motor_stop_uncovering();
    else pool_motor_stop_covering();
}

void init() {
    stdio_init_all();
    pool_system_button_init(on_pool_system_button_click);
    day_night_sensor_init(on_day_night_sensor_change);
    pool_cover_sensor_init(on_pool_cover_sensor);
    pool_lights_init();
    pool_motor_init();
    heat_sensor_init();
    pool_heater_init();
    pool_display_layout_init();
    pool_lights_set_status(false);
    pool_heater_off();
    temperature = heat_sensor_read();
}

void temperature_regulate() {
    if (!isActive) return;
    temperature = heat_sensor_read();
    if (temperature < 26) pool_heater_warm();
    if (temperature == 26) pool_heater_keep();
    if (temperature > 26) pool_heater_off();
}

int main() {
    init();
    while (true) {
        temperature_regulate();
        pool_display_layout_print(isActive, isNight, temperature);
        sleep_ms(100);
    }
    return 0;
}