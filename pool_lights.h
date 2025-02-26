#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.h"
#include "timer.h"

#define PIXELS 25
#define COLOR 6
#define TWINKLES 32
#define FIRST_BYTE 24
#define SECOND_BYTE 16
#define THIRD_BYTE 8
static const char OFF[] = "000000";
static const float BRIGHTNESS_FACTOR = 0.1;
static const uint LIGHTS_PIN = 7;
static const PIO pio = pio0;

static uint sm;
static const char *twinkle[] = {
    "02D8E8", "02C0CE", "01A8B4", "01909A", "017880",
    "016067", "00484D", "003033", "001819", "003033",
    "00484D", "016067", "017880", "01909A", "01A8B4",
    "02C0CE", "02D8E8", "1CDCEA", "36E0ED", "51E5EF",
    "6DE9F2", "89EDF4", "A5F1F7", "C2F6F9", "E0FAFC",
    "C2F6F9", "A5F1F7", "89EDF4", "6DE9F2", "51E5EF",
    "36E0ED", "1CDCEA"
};
static volatile int current_twinkle = 0;
static volatile bool _status = false;

static uint to_decimal(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    return 0;
}

static uint to_rgb(const char hex[COLOR]) {
    uint r = ((to_decimal(hex[0]) * 16) + to_decimal(hex[1])) * BRIGHTNESS_FACTOR;
    uint g = ((to_decimal(hex[2]) * 16) + to_decimal(hex[3])) * BRIGHTNESS_FACTOR;
    uint b = ((to_decimal(hex[4]) * 16) + to_decimal(hex[5])) * BRIGHTNESS_FACTOR;
    return (g << FIRST_BYTE) | (r << SECOND_BYTE) | (b << THIRD_BYTE);
}

static void pool_lights_set_color() {
    for (uint i = 0; i < PIXELS; i++) {
        uint led_value = to_rgb(!_status ? OFF : twinkle[current_twinkle]);
        pio_sm_put_blocking(pio, sm, led_value);
    }
    current_twinkle = ++current_twinkle%TWINKLES;
}

void pool_lights_init() {
    set_sys_clock_khz(128000, false);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, LIGHTS_PIN);
    timer_set_repeating(100, pool_lights_set_color);
}

void pool_lights_set_status(bool status) {
    _status = status;
}
