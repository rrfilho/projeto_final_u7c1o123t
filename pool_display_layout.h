#include <stdio.h>
#include "pool_display.h"

static char buffer[30];

void pool_display_layout_init() {
    pool_display_init();
}

static void print_status(bool isActive) {
    sprintf(buffer, "Piscina %s", isActive ? "ativa" : "inativa");
    pool_display_draw_string(buffer, 3, 7);
    pool_display_hline(0, 127, 18);
}

static void print_temperature_and_light(bool isNight, uint temperature) {
    sprintf(buffer, "Temp %d   %s", temperature, isNight ? "Noite" : "Dia");
    pool_display_draw_string(buffer, 3, 20);
    pool_display_hline(0, 127, 30);
}

static char* heater_status(bool isActive, uint temperature) {
    if (!isActive) return "Sem aquecer";
    if (temperature < 26) return "Aquecendo";
    if (temperature == 26) return "Mantendo";
    return "Sem aquecer";
}

static void print_heater(bool isActive, uint temperature) {
    sprintf(buffer, "%s", heater_status(isActive, temperature));
    pool_display_draw_string(buffer, 3, 32);
    pool_display_hline(0, 127, 42);
}

static void print_cover(bool isActive) {
    sprintf(buffer, "%s", isActive ? "Descoberto" : "Coberto");
    pool_display_draw_string(buffer, 3, 45);
}

void pool_display_layout_print(bool isActive, bool isNight, uint temperature) {
    pool_display_clean();
    pool_display_rect(0, 0, 127, 63, true, false);
    print_status(isActive);
    print_temperature_and_light(isNight, temperature);
    print_heater(isActive, temperature);
    print_cover(isActive);
    pool_display_flush();
}