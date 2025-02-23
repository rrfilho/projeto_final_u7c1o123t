#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pool_display_fonts.h"

const uint WIDTH = 128;
const uint HEIGHT = 64;
const uint ADDRESS = 0x3C;
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
const uint BAUDRATE = 400 * 1000;

typedef enum {
    SET_CONTRAST = 0x81,
    SET_ENTIRE_ON = 0xA4,
    SET_NORM_INV = 0xA6,
    SET_DISP = 0xAE,
    SET_MEM_ADDR = 0x20,
    SET_COL_ADDR = 0x21,
    SET_PAGE_ADDR = 0x22,
    SET_DISP_START_LINE = 0x40,
    SET_SEG_REMAP = 0xA0,
    SET_MUX_RATIO = 0xA8,
    SET_COM_OUT_DIR = 0xC0,
    SET_DISP_OFFSET = 0xD3,
    SET_COM_PIN_CFG = 0xDA,
    SET_DISP_CLK_DIV = 0xD5,
    SET_PRECHARGE = 0xD9,
    SET_VCOM_DESEL = 0xDB,
    SET_CHARGE_PUMP = 0x8D
} display_command_t;

struct display_t {
    uint8_t width, height, pages, address;
    i2c_inst_t *i2c_port;
    bool external_vcc;
    uint8_t *ram_buffer;
    size_t bufsize;
    uint8_t port_buffer[2];
} display;

static void pool_display_init_ports() {
    i2c_init(i2c1, BAUDRATE);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

static void pool_display_command(uint8_t command) {
    display.port_buffer[1] = command;
    i2c_write_blocking(display.i2c_port, display.address, display.port_buffer, 2, false);
}

static void pool_display_config() {
    pool_display_command(SET_DISP | 0x00);
    pool_display_command(SET_MEM_ADDR);
    pool_display_command(0x01);
    pool_display_command(SET_DISP_START_LINE | 0x00);
    pool_display_command(SET_SEG_REMAP | 0x01);
    pool_display_command(SET_MUX_RATIO);
    pool_display_command(HEIGHT - 1);
    pool_display_command(SET_COM_OUT_DIR | 0x08);
    pool_display_command(SET_DISP_OFFSET);
    pool_display_command(0x00);
    pool_display_command(SET_COM_PIN_CFG);
    pool_display_command(0x12);
    pool_display_command(SET_DISP_CLK_DIV);
    pool_display_command(0x80);
    pool_display_command(SET_PRECHARGE);
    pool_display_command(0xF1);
    pool_display_command(SET_VCOM_DESEL);
    pool_display_command(0x30);
    pool_display_command(SET_CONTRAST);
    pool_display_command(0xFF);
    pool_display_command(SET_ENTIRE_ON);
    pool_display_command(SET_NORM_INV);
    pool_display_command(SET_CHARGE_PUMP);
    pool_display_command(0x14);
    pool_display_command(SET_DISP | 0x01);
}

static void pool_display_setup() {
    display.width = WIDTH;
    display.height = HEIGHT;
    display.pages = HEIGHT / 8U;
    display.address = ADDRESS;
    display.i2c_port = i2c1;
    display.bufsize = display.pages * display.width + 1;
    display.ram_buffer = calloc(display.bufsize, sizeof(uint8_t));
    display.ram_buffer[0] = 0x40;
    display.port_buffer[0] = 0x80;
}

static void pool_display_pixel(uint8_t x, uint8_t y, bool value) {
  uint16_t index = (y >> 3) + (x << 3) + 1;
  uint8_t pixel = (y & 0b111);
  if (value) display.ram_buffer[index] |= (1 << pixel);
  else display.ram_buffer[index] &= ~(1 << pixel);
}

static void pool_display_draw_char(char c, uint8_t x, uint8_t y) {
    uint16_t index = 0;
    char ver = c;
    if (c >= '0' && c <= '9') index = (c - '0' + 1) * 8;
    if (c >= 'A' && c <= 'Z') index = (c - 'A' + 11) * 8;
    if (c >= 'a' && c <= 'z') index = (c - 'a' + 37) * 8;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t line = fonts[index + i];
        for (uint8_t j = 0; j < 8; ++j) {
            pool_display_pixel(x + j, y + i, line & (1 << j));
        }
    }
}

void pool_display_clean() {
    for (uint8_t y = 0; y < display.height; ++y) {
        for (uint8_t x = 0; x < display.width; ++x) {
            pool_display_pixel(x, y, false);
        }
    }
}

void pool_display_flush() {
    pool_display_command(SET_COL_ADDR);
    pool_display_command(0);
    pool_display_command(display.width - 1);
    pool_display_command(SET_PAGE_ADDR);
    pool_display_command(0);
    pool_display_command(display.pages - 1);
    i2c_write_blocking(display.i2c_port, display.address, display.ram_buffer, display.bufsize, false);
}

void pool_display_init() {
    pool_display_init_ports();
    pool_display_setup();
    pool_display_config();
    pool_display_clean();
    pool_display_flush();
}

void pool_display_rect(uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill) {
    for (uint8_t x = left; x < left + width; ++x) {
        pool_display_pixel(x, top, value);
        pool_display_pixel(x, top + height - 1, value);
    }
    for (uint8_t y = top; y < top + height; ++y) {
        pool_display_pixel(left, y, value);
        pool_display_pixel(left + width - 1, y, value);
    }
    if (fill) {
        for (uint8_t x = left + 1; x < left + width - 1; ++x) {
            for (uint8_t y = top + 1; y < top + height - 1; ++y) {
                pool_display_pixel(x, y, value);
            }
        }
    }
}

void pool_display_hline(uint8_t x0, uint8_t x1, uint8_t y) {
    for (uint8_t x = x0; x <= x1; ++x)
        pool_display_pixel(x, y, true);
}

void pool_display_draw_string(const char *str, uint8_t x, uint8_t y) {
    while (*str) {
        pool_display_draw_char(*str++, x, y);
        x += 8;
        if (x + 8 >= display.width) {
            x = 0;
            y += 8;
        }
        if (y + 8 >= display.height) break;
    }
}