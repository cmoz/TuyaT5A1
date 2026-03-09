// ui_common.h
#pragma once

#include "lvgl.h"
#include "types.h"

// Coloured pin box used on board detail screen
void create_pin_box(lv_obj_t *parent, int x, int y, int w,
                    const char *label, lv_color_t color);

// Standard header bar with back button + mute toggle
// Returns the header object
lv_obj_t* create_header(lv_obj_t *parent, const char *title,
                         AppScreen back_target);

// Standalone mute button for screens without a header (e.g. home)
// Placed at bottom-right corner of parent
void add_mute_button(lv_obj_t *parent);
