// ui_components.cpp
#include "ui_components.h"
#include "components.h"
#include "ui_common.h"
#include "colours.h"
#include "sound.h"
#include "lv_vendor.h"
#include "lvgl.h"
#include <cstdio>    // snprintf, etc.

// ============================================================
// COMPONENT LIST SCREEN
// ============================================================
static void comp_list_cb(lv_event_t *e) {
    sound_play_click();
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    show_component_detail(idx);
}

void show_component_list() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "Components", SCREEN_HOME);

    for (int i = 0; i < component_count; i++) {
        lv_obj_t *btn = lv_button_create(scr);
        lv_obj_set_size(btn, 290, 64);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 60 + i * 76);
        lv_obj_set_style_bg_color(btn, COL_CARD, 0);
        lv_obj_set_style_border_color(btn, COL_GPIO, 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, comp_list_cb, LV_EVENT_CLICKED,
                            (void*)(intptr_t)i);

        lv_obj_t *name = lv_label_create(btn);
        lv_label_set_text(name, components[i]->name);
        lv_obj_set_style_text_color(name, COL_TEXT, 0);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_14, 0);
        lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, -8);

        lv_obj_t *sub = lv_label_create(btn);
        lv_label_set_text(sub, components[i]->description);
        lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
        lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
        lv_obj_align(sub, LV_ALIGN_LEFT_MID, 10, 12);

        lv_obj_t *arr = lv_label_create(btn);
        lv_label_set_text(arr, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_color(arr, COL_TEXT_DIM, 0);
        lv_obj_align(arr, LV_ALIGN_RIGHT_MID, -12, 0);
    }
    lv_vendor_disp_unlock();
}

// ============================================================
// COMPONENT DETAIL SCREEN
// FIX: Use flex-column layout so text doesn't overlap.
// The old approach used lv_obj_get_height() before LVGL had
// calculated sizes, so everything stacked at y=0.
// Flex layout lets LVGL handle vertical stacking automatically.
// ============================================================
void show_component_detail(int index) {
    selected_component = index;
    ComponentInfo *comp = components[index];

    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, comp->name, SCREEN_COMPONENT_LIST);

    // Scrollable flex-column container
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 320, 428);
    lv_obj_set_pos(cont, 0, 52);
    lv_obj_set_style_bg_color(cont, COL_BG, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_hor(cont, 10, 0);
    lv_obj_set_style_pad_top(cont, 8, 0);
    lv_obj_set_style_pad_bottom(cont, 12, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    // Flex column — LVGL stacks each child below the last automatically
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 3, 0);

    // Helper: add a label to the flex container
    auto add_lbl = [&](const char *text, lv_color_t col, bool heading) {
        lv_obj_t *lbl = lv_label_create(cont);
        lv_label_set_text(lbl, text);
        lv_obj_set_style_text_color(lbl, col, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_width(lbl, 296);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
        if (heading) {
            // Extra top margin before each section heading
            lv_obj_set_style_pad_top(lbl, 6, 0);
        }
    };

    add_lbl("VOLTAGE", COL_ACCENT, true);
    add_lbl(comp->voltage, COL_TEXT, false);

    add_lbl("DATA", COL_ACCENT, true);
    add_lbl(comp->dataline, COL_TEXT, false);

    add_lbl("POWER", COL_ACCENT, true);
    add_lbl(comp->power_note, COL_TEXT, false);

    add_lbl("WIRING", COL_ACCENT, true);
    for (int i = 0; i < 6; i++) {
        if (!comp->wiring[i] || comp->wiring[i][0] == '\0') break;
        char buf[128];
        snprintf(buf, sizeof(buf), "- %s", comp->wiring[i]);
        add_lbl(buf, COL_TEXT, false);
    }

    add_lbl("TIPS", COL_ACCENT, true);
    for (int i = 0; i < 4; i++) {
        if (!comp->tips[i] || comp->tips[i][0] == '\0') break;
        char buf[128];
        snprintf(buf, sizeof(buf), "- %s", comp->tips[i]);
        add_lbl(buf, COL_TEXT_DIM, false);
    }

    lv_vendor_disp_unlock();
}
