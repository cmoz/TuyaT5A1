// boards.cpp — board data definitions
// NOTE: No #pragma once here — this is a .cpp file, not a header!
#include "boards.h"

// ============================================================
// ESP32 DEVKIT V1
// ============================================================
static BoardInfo board_esp32_devkit = {
    "ESP32 DevKit V1",
    "38-pin | 3.3V logic | Dual-core 240MHz",
    19,
    {
        {"3V3",       "3V3",   COL_POWER},
        {"GND",       "GND",   COL_GND},
        {"D15/ADC2_3","IO15",  COL_ADC},
        {"D2/ADC2_2", "IO2",   COL_ADC},
        {"D4/ADC2_0", "IO4",   COL_ADC},
        {"RX2",       "RX2",   COL_UART},
        {"TX2",       "TX2",   COL_UART},
        {"D5/SS",     "IO5",   COL_SPI},
        {"D18/SCK",   "SCK",   COL_SPI},
        {"D19/MISO",  "MISO",  COL_SPI},
        {"D21/SDA",   "SDA",   COL_I2C},
        {"RX0",       "RX0",   COL_UART},
        {"TX0",       "TX0",   COL_UART},
        {"D22/SCL",   "SCL",   COL_I2C},
        {"D23/MOSI",  "MOSI",  COL_SPI},
        {"GND",       "GND",   COL_GND},
        {"D1/TX",     "TX",    COL_UART},
        {"D3/RX",     "RX",    COL_UART},
        {"D36/A0",    "VP",    COL_ADC},
    },
    {
        {"EN",        "EN",    COL_SPECIAL},
        {"D36/VP",    "VP",    COL_ADC},
        {"D39/VN",    "VN",    COL_ADC},
        {"D34",       "IO34",  COL_GPIO},
        {"D35",       "IO35",  COL_GPIO},
        {"D32",       "IO32",  COL_ADC},
        {"D33",       "IO33",  COL_ADC},
        {"D25/DAC1",  "DAC1",  COL_ADC},
        {"D26/DAC2",  "DAC2",  COL_ADC},
        {"D27",       "IO27",  COL_GPIO},
        {"D14",       "IO14",  COL_GPIO},
        {"D12",       "IO12",  COL_GPIO},
        {"GND",       "GND",   COL_GND},
        {"D13",       "IO13",  COL_GPIO},
        {"SD2",       "SD2",   COL_SPECIAL},
        {"SD3",       "SD3",   COL_SPECIAL},
        {"CMD",       "CMD",   COL_SPECIAL},
        {"5V",        "5V",    COL_POWER},
        {"GND",       "GND",   COL_GND},
    }
};

// ============================================================
// ESP32-S3 DEVKIT-C
// ============================================================
static BoardInfo board_esp32s3 = {
    "ESP32-S3 DevKit-C",
    "44-pin | 3.3V | 240MHz | USB-OTG",
    20,
    {
        {"3V3",       "3V3",   COL_POWER},
        {"3V3",       "3V3",   COL_POWER},
        {"RST",       "RST",   COL_SPECIAL},
        {"IO4/ADC",   "IO4",   COL_ADC},
        {"IO5/ADC",   "IO5",   COL_ADC},
        {"IO6/ADC",   "IO6",   COL_ADC},
        {"IO7/ADC",   "IO7",   COL_ADC},
        {"IO15",      "IO15",  COL_GPIO},
        {"IO16",      "IO16",  COL_GPIO},
        {"IO17",      "IO17",  COL_GPIO},
        {"IO18",      "IO18",  COL_GPIO},
        {"IO8/SDA",   "SDA",   COL_I2C},
        {"IO3",       "IO3",   COL_GPIO},
        {"IO46",      "IO46",  COL_GPIO},
        {"IO9",       "IO9",   COL_GPIO},
        {"IO10/MOSI", "MOSI",  COL_SPI},
        {"IO11/MISO", "MISO",  COL_SPI},
        {"IO12/SCK",  "SCK",   COL_SPI},
        {"IO13/SS",   "SS",    COL_SPI},
        {"GND",       "GND",   COL_GND},
    },
    {
        {"GND",       "GND",   COL_GND},
        {"5V",        "5V",    COL_POWER},
        {"IO43/TX",   "TX",    COL_UART},
        {"IO44/RX",   "RX",    COL_UART},
        {"IO1/ADC",   "IO1",   COL_ADC},
        {"IO2/ADC",   "IO2",   COL_ADC},
        {"IO42/SCL",  "SCL",   COL_I2C},
        {"IO41",      "IO41",  COL_GPIO},
        {"IO40",      "IO40",  COL_GPIO},
        {"IO39",      "IO39",  COL_GPIO},
        {"IO38",      "IO38",  COL_GPIO},
        {"IO37",      "IO37",  COL_GPIO},
        {"IO36",      "IO36",  COL_GPIO},
        {"IO35",      "IO35",  COL_GPIO},
        {"IO0/BOOT",  "BOOT",  COL_SPECIAL},
        {"IO45",      "IO45",  COL_GPIO},
        {"IO48",      "IO48",  COL_GPIO},
        {"IO47",      "IO47",  COL_GPIO},
        {"IO21",      "IO21",  COL_GPIO},
        {"GND",       "GND",   COL_GND},
    }
};

// ============================================================
// ESP32-S2 MINI
// pin_count = 16 (number of rows = max of left/right side lengths)
// ============================================================
static BoardInfo board_esp32_s2_mini = {
    "ESP32-S2 Mini",
    "27-pin | 3.3V | Single-core 240MHz | Native USB",
    16,
    {
        {"3V3",    "3V3",   COL_POWER},
        {"5V",     "5V",    COL_POWER},
        {"GND",    "GND",   COL_GND},
        {"IO1",    "IO1",   COL_ADC},
        {"IO2",    "IO2",   COL_ADC},
        {"IO3",    "IO3",   COL_ADC},
        {"IO4",    "IO4",   COL_ADC},
        {"IO5",    "IO5",   COL_ADC},
        {"IO6",    "IO6",   COL_ADC},
        {"IO7",    "IO7",   COL_ADC},
        {"IO8",    "IO8",   COL_ADC},
        {"IO9",    "IO9",   COL_ADC},
        {"TX",     "IO43",  COL_UART},
        {"RX",     "IO44",  COL_UART},
        {"SDA",    "IO33",  COL_I2C},
        {"SCL",    "IO35",  COL_I2C},
    },
    {
        {"MOSI",   "IO11",  COL_SPI},
        {"MISO",   "IO13",  COL_SPI},
        {"SCK",    "IO12",  COL_SPI},
        {"SS",     "IO10",  COL_SPI},
        {"USB_D+", "IO20",  COL_USB},
        {"USB_D-", "IO19",  COL_USB},
        {"BOOT",   "IO0",   COL_OTHER},
        {"EN",     "EN",    COL_OTHER},
        {"",       "",      COL_GND},   // padding rows to match left side count
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
        {"",       "",      COL_GND},
    }
};

// ============================================================
// ESP32-S3 MINI
// pin_count = 13
// ============================================================
static BoardInfo board_esp32_s3_mini = {
    "ESP32-S3 Mini",
    "27-pin | 3.3V | Dual-core 240MHz | USB + BLE5",
    13,
    {
        {"3V3",   "3V3",   COL_POWER},
        {"5V",    "5V",    COL_POWER},
        {"GND",   "GND",   COL_GND},
        {"IO1",   "IO1",   COL_ADC},
        {"IO2",   "IO2",   COL_ADC},
        {"IO3",   "IO3",   COL_ADC},
        {"IO4",   "IO4",   COL_ADC},
        {"IO5",   "IO5",   COL_ADC},
        {"IO6",   "IO6",   COL_ADC},
        {"IO7",   "IO7",   COL_ADC},
        {"IO8",   "IO8",   COL_ADC},
        {"TX",    "IO43",  COL_UART},
        {"RX",    "IO44",  COL_UART},
    },
    {
        {"SDA",    "IO8",  COL_I2C},
        {"SCL",    "IO9",  COL_I2C},
        {"MOSI",   "IO11", COL_SPI},
        {"MISO",   "IO13", COL_SPI},
        {"SCK",    "IO12", COL_SPI},
        {"SS",     "IO10", COL_SPI},
        {"USB_D+", "IO20", COL_USB},
        {"USB_D-", "IO19", COL_USB},
        {"BOOT",   "IO0",  COL_OTHER},
        {"EN",     "EN",   COL_OTHER},
        {"",       "",     COL_GND},
        {"",       "",     COL_GND},
        {"",       "",     COL_GND},
    }
};

// ============================================================
// ESP32-C3 SUPERMINI
// pin_count = 10
// ============================================================
static BoardInfo board_esp32_c3_supermini = {
    "ESP32-C3 SuperMini",
    "14-pin | 3.3V | RISC-V 160MHz | WiFi + BLE5",
    10,
    {
        {"3V3",  "3V3",  COL_POWER},
        {"5V",   "5V",   COL_POWER},
        {"GND",  "GND",  COL_GND},
        {"IO0",  "IO0",  COL_ADC},
        {"IO1",  "IO1",  COL_ADC},
        {"IO2",  "IO2",  COL_ADC},
        {"IO3",  "IO3",  COL_ADC},
        {"IO4",  "IO4",  COL_ADC},
        {"TX",   "IO21", COL_UART},
        {"RX",   "IO20", COL_UART},
    },
    {
        {"SDA",  "IO8",  COL_I2C},
        {"SCL",  "IO9",  COL_I2C},
        {"MOSI", "IO6",  COL_SPI},
        {"MISO", "IO7",  COL_SPI},
        {"SCK",  "IO5",  COL_SPI},
        {"BOOT", "IO9",  COL_OTHER},
        {"",     "",     COL_GND},
        {"",     "",     COL_GND},
        {"",     "",     COL_GND},
        {"",     "",     COL_GND},
    }
};

// ============================================================
// ESP32-C6 DEVKIT
// pin_count = 10
// ============================================================
static BoardInfo board_esp32_c6 = {
    "ESP32-C6 DevKit",
    "22-pin | 3.3V | RISC-V 160MHz | WiFi6+BLE5+Zigbee",
    10,
    {
        {"3V3",  "3V3",  COL_POWER},
        {"5V",   "5V",   COL_POWER},
        {"GND",  "GND",  COL_GND},
        {"IO0",  "IO0",  COL_ADC},
        {"IO1",  "IO1",  COL_ADC},
        {"IO2",  "IO2",  COL_ADC},
        {"IO3",  "IO3",  COL_ADC},
        {"IO4",  "IO4",  COL_ADC},
        {"TX",   "IO16", COL_UART},
        {"RX",   "IO17", COL_UART},
    },
    {
        {"SDA",  "IO8",  COL_I2C},
        {"SCL",  "IO9",  COL_I2C},
        {"MOSI", "IO6",  COL_SPI},
        {"MISO", "IO7",  COL_SPI},
        {"SCK",  "IO5",  COL_SPI},
        {"SS",   "IO10", COL_SPI},
        {"BOOT", "IO9",  COL_OTHER},
        {"EN",   "EN",   COL_OTHER},
        {"",     "",     COL_GND},
        {"",     "",     COL_GND},
    }
};

// ============================================================
// BOARD LIST + GLOBALS
// ============================================================
BoardInfo* boards[] = {
    &board_esp32_devkit,
    &board_esp32s3,
    &board_esp32_s2_mini,
    &board_esp32_s3_mini,
    &board_esp32_c3_supermini,
    &board_esp32_c6
};

int board_count   = sizeof(boards) / sizeof(boards[0]);
int selected_board = 0;

LegendEntry legend[8] = {
    {"PWR",  COL_POWER},
    {"GND",  COL_GND},
    {"GPIO", COL_GPIO},
    {"ADC",  COL_ADC},
    {"SPI",  COL_SPI},
    {"I2C",  COL_I2C},
    {"UART", COL_UART},
    {"USB",  COL_USB},
};
