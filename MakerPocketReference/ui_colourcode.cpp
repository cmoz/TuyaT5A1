// ui_colourcode.cpp

#include "ui_colourcode.h"   // this module’s own header
#include "ui_common.h"       // create_header, etc.
#include "colours.h"         // colour constants
#include "types.h"           // PinInfo, etc.
#include "lvgl.h"
#include "lv_vendor.h"
#include <cstdio>    // snprintf, etc.

// ============================================================
// RESISTOR COLOUR CODE CALCULATOR
// ============================================================

// Roller and swatch widget pointers
static lv_obj_t *cc_roller[5]  = { NULL, NULL, NULL, NULL, NULL };
static lv_obj_t *cc_swatch[5]  = { NULL, NULL, NULL, NULL, NULL };
static lv_obj_t *cc_visual[5]  = { NULL, NULL, NULL, NULL, NULL };
static lv_obj_t *cc_result_lbl = NULL;
static int current_band_count  = 4;

// ============================================================
// FORWARD DECLARATIONS
// ============================================================
static void update_colour_result();
static void build_colour_calc_ui(lv_obj_t *scr, int band_count);

// Colour names for each roller type
static const char *const DIGIT_OPTIONS = "Black\nBrown\nRed\nOrange\nYellow\nGreen\nBlue\nViolet\nGrey\nWhite";
static const char *const MULT_OPTIONS = "Black\nBrown\nRed\nOrange\nYellow\nGreen\nBlue\nViolet\nGrey\nWhite\nGold\nSilver";
static const char *const TOL_OPTIONS = "Brown\nRed\nOrange\nYellow\nGreen\nBlue\nViolet\nGrey\nGold\nSilver\nNone";

// Multiplier values (matches MULT_OPTIONS order)
static const double mult_values[] = {
  1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
  100000000, 1000000000, 0.1, 0.01
};

// Tolerance strings (matches TOL_OPTIONS order)
static const char *const tol_strings[] = {
  "1%", "2%", "3%", "4%", "0.5%", "0.25%", "0.1%", "0.05%", "5%", "10%", "20%"
};

// Colour swatches (RGB) for digit/mult colours 0-11
static const uint8_t band_r[] = { 20, 139, 210, 255, 255, 50, 50, 148, 160, 255, 212, 192 };
static const uint8_t band_g[] = { 20, 69, 50, 140, 210, 180, 100, 0, 160, 255, 175, 192 };
static const uint8_t band_b[] = { 20, 19, 50, 0, 0, 80, 220, 211, 160, 255, 55, 192 };

// Tolerance swatch colours (Brown,Red,Orange,Yellow,Green,Blue,Violet,Grey,Gold,Silver,None)
static const uint8_t tol_r[] = { 139, 210, 255, 255, 50, 50, 148, 160, 212, 192, 60 };
static const uint8_t tol_g[] = { 69, 50, 140, 210, 180, 100, 0, 160, 175, 192, 60 };
static const uint8_t tol_b[] = { 19, 50, 0, 0, 80, 220, 211, 160, 55, 192, 60 };



// ============================================================
// RESISTOR COLOUR CALLBACK
// ============================================================


static void cc_roller_cb(lv_event_t *e) {
    lv_vendor_disp_lock();

    lv_obj_t *roller = (lv_obj_t *)lv_event_get_target(e);

	
	// Clear all highlights
    for (int i = 0; i < current_band_count; i++) {
        if (cc_visual[i]) {
            lv_obj_set_style_shadow_opa(cc_visual[i], LV_OPA_0, 0);
        }
    }

    // Highlight the active band
    for (int i = 0; i < current_band_count; i++) {
        if (cc_roller[i] == roller) {
            lv_obj_set_style_shadow_width(cc_visual[i], 10, 0);
            lv_obj_set_style_shadow_color(cc_visual[i], lv_color_white(), 0);
            lv_obj_set_style_shadow_opa(cc_visual[i], LV_OPA_40, 0);
        }
    }

    update_colour_result();
    lv_vendor_disp_unlock();
}


// --- shared UI builder ---





static void update_colour_result() {
    if (!cc_result_lbl) return;

    // --- Read rollers based on band count ---
    uint16_t b1 = lv_roller_get_selected(cc_roller[0]);
    uint16_t b2 = lv_roller_get_selected(cc_roller[1]);

    uint16_t b3 = 0;
    uint16_t mult = 0;
    uint16_t tol = 0;

    if (current_band_count == 4) {
        // 4-band: digit1, digit2, multiplier, tolerance
        mult = lv_roller_get_selected(cc_roller[2]);
        tol  = lv_roller_get_selected(cc_roller[3]);
    }
    else { // 5-band
        // 5-band: digit1, digit2, digit3, multiplier, tolerance
        b3   = lv_roller_get_selected(cc_roller[2]);
        mult = lv_roller_get_selected(cc_roller[3]);
        tol  = lv_roller_get_selected(cc_roller[4]);
    }

    // --- Update swatches and band visuals ---
    // Band 1
    lv_obj_set_style_bg_color(cc_swatch[0], lv_color_make(band_r[b1], band_g[b1], band_b[b1]), 0);
    lv_obj_set_style_bg_color(cc_visual[0], lv_color_make(band_r[b1], band_g[b1], band_b[b1]), 0);

    // Band 2
    lv_obj_set_style_bg_color(cc_swatch[1], lv_color_make(band_r[b2], band_g[b2], band_b[b2]), 0);
    lv_obj_set_style_bg_color(cc_visual[1], lv_color_make(band_r[b2], band_g[b2], band_b[b2]), 0);

    if (current_band_count == 5) {
        // Band 3
        lv_obj_set_style_bg_color(cc_swatch[2], lv_color_make(band_r[b3], band_g[b3], band_b[b3]), 0);
        lv_obj_set_style_bg_color(cc_visual[2], lv_color_make(band_r[b3], band_g[b3], band_b[b3]), 0);

        // Band 4 (multiplier)
        lv_obj_set_style_bg_color(cc_swatch[3], lv_color_make(band_r[mult], band_g[mult], band_b[mult]), 0);
        lv_obj_set_style_bg_color(cc_visual[3], lv_color_make(band_r[mult], band_g[mult], band_b[mult]), 0);

        // Band 5 (tolerance)
        lv_obj_set_style_bg_color(cc_swatch[4], lv_color_make(tol_r[tol], tol_g[tol], tol_b[tol]), 0);
        lv_obj_set_style_bg_color(cc_visual[4], lv_color_make(tol_r[tol], tol_g[tol], tol_b[tol]), 0);
    }
    else {
        // 4-band: multiplier + tolerance
        lv_obj_set_style_bg_color(cc_swatch[2], lv_color_make(band_r[mult], band_g[mult], band_b[mult]), 0);
        lv_obj_set_style_bg_color(cc_visual[2], lv_color_make(band_r[mult], band_g[mult], band_b[mult]), 0);

        lv_obj_set_style_bg_color(cc_swatch[3], lv_color_make(tol_r[tol], tol_g[tol], tol_b[tol]), 0);
        lv_obj_set_style_bg_color(cc_visual[3], lv_color_make(tol_r[tol], tol_g[tol], tol_b[tol]), 0);
    }

    // --- Calculate resistor value ---
    double value = 0;

    if (current_band_count == 4) {
        value = (b1 * 10 + b2) * mult_values[mult];
    } else {
        value = (b1 * 100 + b2 * 10 + b3) * mult_values[mult];
    }

    // --- Format nicely ---
    char val_buf[32];
    if (value >= 1000000) snprintf(val_buf, sizeof(val_buf), "%.2f MΩ", value / 1000000.0);
    else if (value >= 1000) snprintf(val_buf, sizeof(val_buf), "%.2f kΩ", value / 1000.0);
    else if (value < 1) snprintf(val_buf, sizeof(val_buf), "%.2f Ω", value);
    else snprintf(val_buf, sizeof(val_buf), "%.0f Ω", value);

    char buf[64];
    snprintf(buf, sizeof(buf), "%s  %s", val_buf, tol_strings[tol]);
    lv_label_set_text(cc_result_lbl, buf);
}



// ============================================================
// RESISTOR GRAPHIC
// ============================================================


// Creates a resistor graphic and returns band objects in the provided array.
// band_count = 4 or 5
void draw_resistor(lv_obj_t *scr, int band_count, lv_obj_t *bands_out[]) {

  // --- Wires ---
  lv_obj_t *wire_l = lv_obj_create(scr);
  lv_obj_set_size(wire_l, 18, 4);
  lv_obj_align(wire_l, LV_ALIGN_TOP_MID, -116, 82);
  lv_obj_set_style_bg_color(wire_l, lv_color_make(200, 200, 200), 0);
  lv_obj_set_style_border_width(wire_l, 0, 0);
  lv_obj_set_style_radius(wire_l, 2, 0);

  lv_obj_t *wire_r = lv_obj_create(scr);
  lv_obj_set_size(wire_r, 18, 4);
  lv_obj_align(wire_r, LV_ALIGN_TOP_MID, 116, 82);
  lv_obj_set_style_bg_color(wire_r, lv_color_make(200, 200, 200), 0);
  lv_obj_set_style_border_width(wire_r, 0, 0);
  lv_obj_set_style_radius(wire_r, 2, 0);

  // --- Body ---
  lv_obj_t *body = lv_obj_create(scr);
  lv_obj_set_size(body, 196, 28);
  lv_obj_align(body, LV_ALIGN_TOP_MID, 0, 75);
  lv_obj_set_style_bg_color(body, lv_color_make(210, 185, 130), 0);
  lv_obj_set_style_border_width(body, 0, 0);
  lv_obj_set_style_radius(body, 8, 0);

  // --- Band positions ---
  // These are tuned to look like a real resistor
  static const int offsets_4[] = { -68, -28, 12, 52 };
  static const int offsets_5[] = { -78, -38, 2, 42, 82 };

  const int *offsets = (band_count == 4 ? offsets_4 : offsets_5);

  for (int i = 0; i < band_count; i++) {
    bands_out[i] = lv_obj_create(scr);
    lv_obj_set_size(bands_out[i], 16, 28);
    lv_obj_align(bands_out[i], LV_ALIGN_TOP_MID, offsets[i], 75);
    lv_obj_set_style_bg_color(bands_out[i], lv_color_make(20, 20, 20), 0);
    lv_obj_set_style_border_width(bands_out[i], 0, 0);
    lv_obj_set_style_radius(bands_out[i], 3, 0);
  }
}


// ============================================================
// ADDING OTHER RESISTOR CALCULATORS
// ============================================================

void show_calc_colour_5band() {
  
  lv_vendor_disp_lock();
  lv_obj_clean(lv_screen_active());
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, COL_BG, 0);
  create_header(scr, "5-Band Code", SCREEN_CALC_MENU);

  current_band_count = 5;

  build_colour_calc_ui(scr, 5);

  // Result box
  lv_obj_t *res_box = lv_obj_create(scr);
  lv_obj_set_size(res_box, 290, 50);
  lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 322);
  lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
  lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
  lv_obj_set_style_border_width(res_box, 2, 0);
  lv_obj_set_style_radius(res_box, 10, 0);

  cc_result_lbl = lv_label_create(res_box);
  lv_obj_set_style_text_color(cc_result_lbl, COL_ACCENT, 0);
  lv_obj_set_style_text_font(cc_result_lbl, &lv_font_montserrat_24, 0);
  lv_obj_align(cc_result_lbl, LV_ALIGN_CENTER, 0, 0);

  update_colour_result();
  lv_vendor_disp_unlock();
}





// ============================================================
// SHOW CALC COLOUR
// ============================================================


void show_calc_colour() {
  
  lv_vendor_disp_lock();
  lv_obj_clean(lv_screen_active());
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, COL_BG, 0);
  create_header(scr, "Colour Code", SCREEN_CALC_MENU);

  current_band_count = 4;

  build_colour_calc_ui(scr, 4);

  // Result box
  lv_obj_t *res_box = lv_obj_create(scr);
  lv_obj_set_size(res_box, 290, 50);
  lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 322);
  lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
  lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
  lv_obj_set_style_border_width(res_box, 2, 0);
  lv_obj_set_style_radius(res_box, 10, 0);

  cc_result_lbl = lv_label_create(res_box);
  lv_obj_set_style_text_color(cc_result_lbl, COL_ACCENT, 0);
  lv_obj_set_style_text_font(cc_result_lbl, &lv_font_montserrat_24, 0);
  lv_obj_align(cc_result_lbl, LV_ALIGN_CENTER, 0, 0);

  update_colour_result();
  lv_vendor_disp_unlock();
}






// ============================================================
// SHARED CALC LAYOUT
// ============================================================

// Shared UI builder for 4-band and 5-band calculators
void build_colour_calc_ui(lv_obj_t *scr, int band_count) {

  // Draw resistor + band visuals
  draw_resistor(scr, band_count, cc_visual);

  // Band labels
  static const char *labels_4[] = { "Band 1", "Band 2", "Mult", "Tol" };
  static const char *labels_5[] = { "Band 1", "Band 2", "Band 3", "Mult", "Tol" };

  const char **labels = (band_count == 4 ? labels_4 : labels_5);

  // Roller positions
  int roller_w = (band_count == 4 ? 66 : 56);
  int roller_cx_4[] = { 40, 112, 184, 256 };
  int roller_cx_5[] = { 32, 96, 160, 224, 288 };

  int *roller_cx = (band_count == 4 ? roller_cx_4 : roller_cx_5);

  for (int i = 0; i < band_count; i++) {

    // Label
    lv_obj_t *lbl = lv_label_create(scr);
    lv_label_set_text(lbl, labels[i]);
    lv_obj_set_style_text_color(lbl, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl, roller_cx[i] - roller_w / 2, 116);

    // Swatch
    cc_swatch[i] = lv_obj_create(scr);
    lv_obj_set_size(cc_swatch[i], roller_w, 18);
    lv_obj_set_pos(cc_swatch[i], roller_cx[i] - roller_w / 2, 135);
    lv_obj_set_style_bg_color(cc_swatch[i], lv_color_make(20, 20, 20), 0);
    lv_obj_set_style_border_color(cc_swatch[i], COL_ACCENT, 0);
    lv_obj_set_style_border_width(cc_swatch[i], 1, 0);
    lv_obj_set_style_radius(cc_swatch[i], 4, 0);

    // Roller
    cc_roller[i] = lv_roller_create(scr);
    lv_obj_set_size(cc_roller[i], roller_w, 156);
    lv_obj_set_pos(cc_roller[i], roller_cx[i] - roller_w / 2, 157);
    lv_obj_set_style_text_font(cc_roller[i], &lv_font_montserrat_14, 0);
    lv_obj_set_style_bg_color(cc_roller[i], COL_CARD, 0);
    lv_obj_set_style_text_color(cc_roller[i], COL_TEXT, 0);
    lv_obj_set_style_border_color(cc_roller[i], COL_ACCENT, 0);
    lv_obj_set_style_border_width(cc_roller[i], 1, 0);

    // Highlight selected item
    lv_obj_set_style_bg_color(cc_roller[i], COL_ACCENT, LV_PART_SELECTED);
    lv_obj_set_style_text_color(cc_roller[i], lv_color_make(0, 0, 0), LV_PART_SELECTED);

    // Options
    if (band_count == 4) {
      if (i < 2) lv_roller_set_options(cc_roller[i], DIGIT_OPTIONS, LV_ROLLER_MODE_NORMAL);
      else if (i == 2) lv_roller_set_options(cc_roller[i], MULT_OPTIONS, LV_ROLLER_MODE_NORMAL);
      else lv_roller_set_options(cc_roller[i], TOL_OPTIONS, LV_ROLLER_MODE_NORMAL);
    } else {  // 5-band
      if (i < 3) lv_roller_set_options(cc_roller[i], DIGIT_OPTIONS, LV_ROLLER_MODE_NORMAL);
      else if (i == 3) lv_roller_set_options(cc_roller[i], MULT_OPTIONS, LV_ROLLER_MODE_NORMAL);
      else lv_roller_set_options(cc_roller[i], TOL_OPTIONS, LV_ROLLER_MODE_NORMAL);
    }

    lv_obj_add_event_cb(cc_roller[i], cc_roller_cb, LV_EVENT_VALUE_CHANGED, NULL);
  }
}
