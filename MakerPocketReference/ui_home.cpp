// ui_home.cpp
#include "ui_home.h"
#include "ui_boards.h"
#include "ui_components.h"
#include "ui_calcs.h"
#include "ui_tutorials.h"
#include "ui_common.h"
#include "colours.h"
#include "sound.h"
#include "lvgl.h"
#include "lv_vendor.h"

static void home_btn_cb(lv_event_t *e) {
    sound_play_navigate();
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_BOARD_LIST:     show_board_list();      break;
        case SCREEN_COMPONENT_LIST: show_component_list();  break;
        case SCREEN_CALC_MENU:      show_calc_menu();       break;
        case SCREEN_TUTORIALS:      show_tutorials();       break;
        default: break;
    }
}

void show_home_screen() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Maker's Pocket Reference");
    lv_obj_set_style_text_color(title, COL_ACCENT, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(title, 300);
    lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "tinkertailor.ca");
    lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 76);

    struct {
        const char *icon;
        const char *label;
        AppScreen   target;
        lv_color_t  col;
    } btns[] = {
        {LV_SYMBOL_LIST,    "Boards",      SCREEN_BOARD_LIST,     lv_color_make(50, 120, 220)},
        {LV_SYMBOL_CHARGE,  "Components",  SCREEN_COMPONENT_LIST, lv_color_make(50, 180, 80)},
        {LV_SYMBOL_SHUFFLE, "Calculators", SCREEN_CALC_MENU,      lv_color_make(200, 120, 0)},
        {LV_SYMBOL_PLAY,    "Tutorials",   SCREEN_TUTORIALS,      lv_color_make(220, 50, 80)},
    };

    for (int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_button_create(scr);
        lv_obj_set_size(btn, 280, 68);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 100 + i * 80);
        lv_obj_set_style_bg_color(btn, COL_CARD, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(btn, btns[i].col, 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_radius(btn, 12, 0);
        lv_obj_add_event_cb(btn, home_btn_cb, LV_EVENT_CLICKED,
                            (void*)(intptr_t)btns[i].target);

        lv_obj_t *icon = lv_label_create(btn);
        lv_label_set_text(icon, btns[i].icon);
        lv_obj_set_style_text_color(icon, btns[i].col, 0);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 16, 0);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, btns[i].label);
        lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 60, 0);

        lv_obj_t *arrow = lv_label_create(btn);
        lv_label_set_text(arrow, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_color(arrow, COL_TEXT_DIM, 0);
        lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -12, 0);
    }

    // Mute button bottom-right (home screen has no header)
    add_mute_button(scr);

    lv_vendor_disp_unlock();
}
