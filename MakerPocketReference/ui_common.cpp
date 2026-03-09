// ui_common.cpp
#include "ui_common.h"
#include "ui_home.h"
#include "ui_boards.h"
#include "ui_components.h"
#include "ui_calcs.h"
#include "colours.h"
#include "sound.h"
#include "lvgl.h"
#include "lv_vendor.h"

// ============================================================
// HELPER: Colour-coded pin box
// ============================================================
void create_pin_box(lv_obj_t *parent, int x, int y, int w,
                    const char *label, lv_color_t color) {
    lv_obj_t *box = lv_obj_create(parent);
    lv_obj_set_size(box, w, 22);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_style_bg_color(box, color, 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(box, 0, 0);
    lv_obj_set_style_radius(box, 3, 0);
    lv_obj_set_style_pad_all(box, 0, 0);

    lv_obj_t *lbl = lv_label_create(box);
    if (!label || label[0] == '\0') label = " ";
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);
}

// ============================================================
// MUTE BUTTON CALLBACK
// ============================================================
static void mute_btn_cb(lv_event_t *e) {
    sound_toggle_mute();
    // Play a click if we just UN-muted
    if (!sound_is_muted()) sound_play_click();

    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *lbl = lv_obj_get_child(btn, 0);
    lv_label_set_text(lbl, sound_is_muted() ? LV_SYMBOL_MUTE : LV_SYMBOL_AUDIO);
}

// ============================================================
// HELPER: Back button callback
// ============================================================
static void back_btn_cb(lv_event_t *e) {
    sound_play_navigate();
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_HOME:           show_home_screen();      break;
        case SCREEN_BOARD_LIST:     show_board_list();       break;
        case SCREEN_COMPONENT_LIST: show_component_list();   break;
        case SCREEN_CALC_MENU:      show_calc_menu();        break;
        default:                    show_home_screen();      break;
    }
}

// ============================================================
// HELPER: Header bar (includes mute button top-right)
// ============================================================
lv_obj_t* create_header(lv_obj_t *parent, const char *title,
                          AppScreen back_target) {
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, 320, 48);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, COL_HEADER, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);

    // Back button (left)
    lv_obj_t *back_btn = lv_button_create(header);
    lv_obj_set_size(back_btn, 44, 40);
    lv_obj_set_pos(back_btn, 4, 4);
    lv_obj_set_style_bg_color(back_btn, COL_ACCENT, 0);
    lv_obj_set_style_radius(back_btn, 8, 0);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED,
                        (void*)(intptr_t)back_target);
    lv_obj_t *back_lbl = lv_label_create(back_btn);
    lv_label_set_text(back_lbl, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(back_lbl, COL_TEXT, 0);
    lv_obj_align(back_lbl, LV_ALIGN_CENTER, 0, 0);

    // Title (centred, offset right to clear back button)
    lv_obj_t *title_lbl = lv_label_create(header);
    lv_label_set_text(title_lbl, title);
    lv_obj_set_style_text_color(title_lbl, COL_TEXT, 0);
    lv_obj_set_style_text_font(title_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(title_lbl, LV_ALIGN_CENTER, 0, 0);

    // Mute button (top-right corner)
    lv_obj_t *mute_btn = lv_button_create(header);
    lv_obj_set_size(mute_btn, 40, 40);
    lv_obj_set_pos(mute_btn, 276, 4);   // 320 - 40 - 4 = 276
    lv_obj_set_style_bg_color(mute_btn, COL_CARD, 0);
    lv_obj_set_style_border_width(mute_btn, 0, 0);
    lv_obj_set_style_radius(mute_btn, 8, 0);
    lv_obj_add_event_cb(mute_btn, mute_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *mute_lbl = lv_label_create(mute_btn);
    lv_label_set_text(mute_lbl, sound_is_muted() ? LV_SYMBOL_MUTE : LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(mute_lbl, COL_TEXT_DIM, 0);
    lv_obj_align(mute_lbl, LV_ALIGN_CENTER, 0, 0);

    // Accent underline
    lv_obj_t *line = lv_obj_create(parent);
    lv_obj_set_size(line, 320, 2);
    lv_obj_set_pos(line, 0, 48);
    lv_obj_set_style_bg_color(line, COL_ACCENT, 0);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 0, 0);

    return header;
}

// ============================================================
// HELPER: Standalone mute button for home screen (bottom-right)
// ============================================================
void add_mute_button(lv_obj_t *parent) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, 44, 44);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -8, -8);
    lv_obj_set_style_bg_color(btn, COL_CARD, 0);
    lv_obj_set_style_border_color(btn, COL_TEXT_DIM, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_radius(btn, 22, 0);
    lv_obj_add_event_cb(btn, mute_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, sound_is_muted() ? LV_SYMBOL_MUTE : LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(lbl, COL_TEXT_DIM, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);
}
