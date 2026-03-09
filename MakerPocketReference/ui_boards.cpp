#include "ui_boards.h"
#include "boards.h"
#include "ui_common.h"   // for create_header, create_pin_box, etc.
#include "ui_home.h"
#include "colours.h"
#include "lv_vendor.h"
#include <cstdio>    // snprintf, etc.

static void board_list_cb(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  show_board_detail(idx);
}


// ============================================================
// BOARD LIST SCREEN
// ============================================================


void show_board_list() {
  lv_vendor_disp_lock();
  lv_obj_clean(lv_screen_active());
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, COL_BG, 0);
  create_header(scr, "Boards", SCREEN_HOME);

  for (int i = 0; i < board_count; i++) {
    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, 290, 64);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 60 + i * 76);
    lv_obj_set_style_bg_color(btn, COL_CARD, 0);
    lv_obj_set_style_border_color(btn, lv_color_make(50, 120, 220), 0);
    lv_obj_set_style_border_width(btn, 2, 0);
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_add_event_cb(btn, board_list_cb, LV_EVENT_CLICKED,
                        (void *)(intptr_t)i);

    lv_obj_t *name = lv_label_create(btn);
    lv_label_set_text(name, boards[i]->name);
    lv_obj_set_style_text_color(name, COL_TEXT, 0);
    lv_obj_set_style_text_font(name, &lv_font_montserrat_14, 0);
    lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, -10);

    lv_obj_t *sub = lv_label_create(btn);
    lv_label_set_text(sub, boards[i]->subtitle);
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
// BOARD DETAIL SCREEN
// ============================================================
void show_board_detail(int index) {
  selected_board = index;
  BoardInfo *board = boards[index];

  lv_vendor_disp_lock();
  lv_obj_clean(lv_screen_active());
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, COL_BG, 0);
  create_header(scr, board->name, SCREEN_BOARD_LIST);

  lv_obj_t *cont = lv_obj_create(scr);
  lv_obj_set_size(cont, 320, 428);
  lv_obj_set_pos(cont, 0, 52);
  lv_obj_set_style_bg_color(cont, COL_BG, 0);
  lv_obj_set_style_border_width(cont, 0, 0);
  lv_obj_set_style_pad_all(cont, 0, 0);
  lv_obj_set_scroll_dir(cont, LV_DIR_VER);

  lv_obj_t *sub = lv_label_create(cont);
  lv_label_set_text(sub, board->subtitle);
  lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
  lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
  lv_obj_set_pos(sub, 10, 8);

  lv_obj_t *ptitle = lv_label_create(cont);
  lv_label_set_text(ptitle, "PINOUT");
  lv_obj_set_style_text_color(ptitle, COL_ACCENT, 0);
  lv_obj_set_style_text_font(ptitle, &lv_font_montserrat_14, 0);
  lv_obj_set_pos(ptitle, 10, 28);

  int pin_start_y = 50;
  int row_h = 24;

  for (int i = 0; i < board->pin_count; i++) {
    create_pin_box(cont, 8, pin_start_y + i * row_h, 130,
                   board->left_pins[i].label, board->left_pins[i].color);
    create_pin_box(cont, 180, pin_start_y + i * row_h, 130,
                   board->right_pins[i].label, board->right_pins[i].color);
  }

  int legend_y = pin_start_y + board->pin_count * row_h + 10;
  lv_obj_t *leg_title = lv_label_create(cont);
  lv_label_set_text(leg_title, "LEGEND");
  lv_obj_set_style_text_color(leg_title, COL_ACCENT, 0);
  lv_obj_set_style_text_font(leg_title, &lv_font_montserrat_14, 0);
  lv_obj_set_pos(leg_title, 10, legend_y);

  for (int i = 0; i < 8; i++) {
    create_pin_box(cont, 8 + (i % 4) * 76, legend_y + 20 + (i / 4) * 26,
                   70, legend[i].label, legend[i].color);
  }
  lv_vendor_disp_unlock();
}



