// ui_splash.cpp
// Retro terminal boot splash — green on black, lines appear one by one.
#include "ui_splash.h"
#include "ui_home.h"
#include "colours.h"
#include "lv_vendor.h"
#include "lvgl.h"

// ============================================================
// BOOT LINES
// ============================================================
static const char *boot_lines[] = {
    " ",
    " ",
    " ",
    "  ================================",
    "   MAKER'S POCKET REFERENCE v1",
    "   CMozMaker  //  Tinker Tailor",
    "  ================================",
    " ",
    "  > BOARDS.................. [OK]",
    "  > COMPONENTS.............. [OK]",
    "  > CALCULATORS............. [OK]",
    "  > TUTORIALS............... [OK]",
    " ",
    "  > ALL SYSTEMS GO.",
    "  > LET'S BUILD SOMETHING.",
    " ",
    NULL
};

// How long each line takes to appear (ms)
#define LINE_DELAY_MS   130
// How long to pause on the last line before switching (ms)
#define FINAL_PAUSE_MS  2000

static int        splash_line_idx  = 0;
static lv_obj_t  *splash_cont      = NULL;
static lv_timer_t *splash_timer    = NULL;

// Accent teal  — matches COL_ACCENT
#define SPLASH_GREEN  lv_color_make(0, 200, 180)
// Dimmer colour for the border/title lines
#define SPLASH_DIM    lv_color_make(0, 120, 110)

// ============================================================
// TIMER CALLBACK — adds one line per tick
// ============================================================
static void splash_tick(lv_timer_t *tmr) {
    if (boot_lines[splash_line_idx] == NULL) {
        // All lines shown — delete timer and go home
        lv_timer_delete(splash_timer);
        splash_timer = NULL;
        show_home_screen();
        return;
    }

    const char *line = boot_lines[splash_line_idx];
    splash_line_idx++;

    // Choose colour: separator lines dimmer, "[OK]" lines brighter
    lv_color_t col = SPLASH_GREEN;
    if (line[0] == ' ' && line[1] == ' ' && line[2] == '=') col = SPLASH_DIM;
    if (line[0] == ' ' && line[1] == '\0')                   col = SPLASH_DIM;

    lv_obj_t *lbl = lv_label_create(splash_cont);
    lv_label_set_text(lbl, line);
    lv_obj_set_style_text_color(lbl, col, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_width(lbl, 300);

    // Slow down on the final "LET'S BUILD" line for dramatic effect
    if (boot_lines[splash_line_idx] == NULL) {
        lv_timer_set_period(splash_timer, FINAL_PAUSE_MS);
    }
}

// ============================================================
// SHOW SPLASH
// ============================================================
void show_splash_screen() {
    lv_vendor_disp_lock();

    splash_line_idx = 0;

    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    // Pure black background
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    // Blinking cursor label (top-left, decorative)
    lv_obj_t *cursor = lv_label_create(scr);
    lv_label_set_text(cursor, "_");
    lv_obj_set_style_text_color(cursor, SPLASH_GREEN, 0);
    lv_obj_set_style_text_font(cursor, &lv_font_montserrat_14, 0);
    lv_obj_align(cursor, LV_ALIGN_BOTTOM_LEFT, 12, -12);

    // Scrollable flex-column container for the lines
    splash_cont = lv_obj_create(scr);
    lv_obj_set_size(splash_cont, 320, 480);
    lv_obj_set_pos(splash_cont, 0, 40);   // start 40px down for breathing room
    lv_obj_set_style_bg_color(splash_cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(splash_cont, 0, 0);
    lv_obj_set_style_pad_left(splash_cont, 0, 0);
    lv_obj_set_style_pad_top(splash_cont, 0, 0);
    lv_obj_set_flex_flow(splash_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(splash_cont, 2, 0);
    lv_obj_set_scroll_dir(splash_cont, LV_DIR_NONE);

    // Small "tinkertailor.ca" at very top
    lv_obj_t *url = lv_label_create(scr);
    lv_label_set_text(url, "  tinkertailor.ca");
    lv_obj_set_style_text_color(url, lv_color_make(0, 70, 60), 0);
    lv_obj_set_style_text_font(url, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(url, 0, 14);

    lv_vendor_disp_unlock();

    // Start typewriter timer
    splash_timer = lv_timer_create(splash_tick, LINE_DELAY_MS, NULL);
}
