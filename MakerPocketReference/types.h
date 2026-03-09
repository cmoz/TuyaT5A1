#pragma once
#include "lvgl.h"

// ============================================================
// DATA STRUCTURES
// Shared types for boards.h and components.h
// ============================================================

typedef struct {
    const char *name;
    const char *label;
    lv_color_t  color;
} PinInfo;

typedef struct {
    const char *label;
    lv_color_t  color;
} LegendEntry;

typedef struct {
    const char  *name;
    const char  *subtitle;
    int          pin_count;
    PinInfo      left_pins[28];   // increased from 20 to support larger boards
    PinInfo      right_pins[28];
} BoardInfo;

typedef struct {
    const char *name;
    const char *description;
    const char *voltage;
    const char *dataline;
    const char *power_note;
    const char *wiring[6];
    const char *tips[4];
} ComponentInfo;

typedef enum {
    SCREEN_HOME,
    SCREEN_BOARD_LIST,
    SCREEN_COMPONENT_LIST,
    SCREEN_CALC_MENU,
    SCREEN_CALC_RESISTOR,
    SCREEN_CALC_POWER,
    SCREEN_CALC_COLOUR,
    SCREEN_CALC_COLOUR_4BAND,
    SCREEN_CALC_COLOUR_5BAND,
    SCREEN_TUTORIALS
} AppScreen;
