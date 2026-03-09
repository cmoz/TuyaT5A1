// ui_calcs.cpp
#include "ui_calcs.h"
#include "ui_common.h"
#include "ui_colourcode.h"
#include "colours.h"
#include "sound.h"
#include "lvgl.h"
#include "lv_vendor.h"
#include <cstdio>    // snprintf, etc.

// ============================================================
// LED RESISTOR STATE + HELPERS
// (must be defined BEFORE show_calc_resistor uses them)
// ============================================================
static float calc_vcc   = 5.0f;
static float calc_vf    = 2.0f;
static float calc_if_ma = 20.0f;
static lv_obj_t *res_result_lbl = NULL;
static lv_obj_t *vcc_val_lbl    = NULL;
static lv_obj_t *vf_val_lbl     = NULL;
static lv_obj_t *if_val_lbl     = NULL;

static void update_resistor_result() {
    if (!res_result_lbl) return;
    float r   = (calc_vcc - calc_vf) / (calc_if_ma / 1000.0f);
    float std = r < 100  ? 100  : r < 220  ? 220  : r < 330  ? 330
              : r < 470  ? 470  : r < 560  ? 560  : r < 680  ? 680
              : r < 820  ? 820  : r < 1000 ? 1000 : r;
    char buf[60];
    snprintf(buf, sizeof(buf), "R = %.0f ohm  (use %.0f ohm)", r, std);
    lv_label_set_text(res_result_lbl, buf);

    // Also refresh value labels if they exist
    char vbuf[16];
    if (vcc_val_lbl) { snprintf(vbuf, sizeof(vbuf), "%.1fV",  calc_vcc);   lv_label_set_text(vcc_val_lbl, vbuf); }
    if (vf_val_lbl)  { snprintf(vbuf, sizeof(vbuf), "%.1fV",  calc_vf);    lv_label_set_text(vf_val_lbl,  vbuf); }
    if (if_val_lbl)  { snprintf(vbuf, sizeof(vbuf), "%.0fmA", calc_if_ma); lv_label_set_text(if_val_lbl,  vbuf); }
}

static void vcc_inc_cb(lv_event_t*) { calc_vcc   += 0.5f; update_resistor_result(); }
static void vcc_dec_cb(lv_event_t*) { if (calc_vcc   > 1.0f) { calc_vcc   -= 0.5f; update_resistor_result(); } }
static void vf_inc_cb(lv_event_t*)  { calc_vf    += 0.1f; update_resistor_result(); }
static void vf_dec_cb(lv_event_t*)  { if (calc_vf    > 0.1f) { calc_vf    -= 0.1f; update_resistor_result(); } }
static void if_inc_cb(lv_event_t*)  { calc_if_ma += 1.0f; update_resistor_result(); }
static void if_dec_cb(lv_event_t*)  { if (calc_if_ma > 1.0f) { calc_if_ma -= 1.0f; update_resistor_result(); } }

static lv_obj_t* make_spin_row(lv_obj_t *parent, const char *label,
                                lv_event_cb_t dec_cb, lv_event_cb_t inc_cb, int y) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl, 10, y + 10);

    lv_obj_t *dec = lv_button_create(parent);
    lv_obj_set_size(dec, 40, 36);
    lv_obj_set_pos(dec, 170, y);
    lv_obj_set_style_bg_color(dec, COL_CARD, 0);
    lv_obj_set_style_border_color(dec, COL_ACCENT, 0);
    lv_obj_set_style_border_width(dec, 1, 0);
    lv_obj_add_event_cb(dec, dec_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *dl = lv_label_create(dec);
    lv_label_set_text(dl, "-");
    lv_obj_set_style_text_color(dl, COL_TEXT, 0);
    lv_obj_align(dl, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *val = lv_label_create(parent);
    lv_obj_set_size(val, 60, 36);
    lv_obj_set_pos(val, 214, y + 10);
    lv_obj_set_style_text_color(val, COL_ACCENT, 0);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(val, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *inc = lv_button_create(parent);
    lv_obj_set_size(inc, 40, 36);
    lv_obj_set_pos(inc, 278, y);
    lv_obj_set_style_bg_color(inc, COL_CARD, 0);
    lv_obj_set_style_border_color(inc, COL_ACCENT, 0);
    lv_obj_set_style_border_width(inc, 1, 0);
    lv_obj_add_event_cb(inc, inc_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *il = lv_label_create(inc);
    lv_label_set_text(il, "+");
    lv_obj_set_style_text_color(il, COL_TEXT, 0);
    lv_obj_align(il, LV_ALIGN_CENTER, 0, 0);

    return val;
}

// ============================================================
// POWER CALCULATOR STATE
// (must also be defined before show_calc_power)
// ============================================================
static float pw_volts = 5.0f;
static float pw_amps  = 0.5f;
static lv_obj_t *pw_result_lbl  = NULL;
static lv_obj_t *pw_v_val_lbl   = NULL;
static lv_obj_t *pw_a_val_lbl   = NULL;

static void update_power_result() {
    if (!pw_result_lbl) return;
    char buf[80];
    snprintf(buf, sizeof(buf), "P=%.2fW   R=%.1f ohm",
             pw_volts * pw_amps, pw_volts / pw_amps);
    lv_label_set_text(pw_result_lbl, buf);

    char vbuf[16];
    if (pw_v_val_lbl) { snprintf(vbuf, sizeof(vbuf), "%.1fV", pw_volts); lv_label_set_text(pw_v_val_lbl, vbuf); }
    if (pw_a_val_lbl) { snprintf(vbuf, sizeof(vbuf), "%.1fA", pw_amps);  lv_label_set_text(pw_a_val_lbl, vbuf); }
}

static void pv_inc_cb(lv_event_t*) { pw_volts += 0.5f; update_power_result(); }
static void pv_dec_cb(lv_event_t*) { if (pw_volts > 0.5f) { pw_volts -= 0.5f; update_power_result(); } }
static void pa_inc_cb(lv_event_t*) { pw_amps  += 0.1f; update_power_result(); }
static void pa_dec_cb(lv_event_t*) { if (pw_amps  > 0.1f) { pw_amps  -= 0.1f; update_power_result(); } }

// ============================================================
// CALCULATOR MENU SCREEN
// ============================================================
static void calc_cb(lv_event_t *e) {
    sound_play_click();
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_CALC_RESISTOR:     show_calc_resistor();    break;
        case SCREEN_CALC_POWER:        show_calc_power();       break;
        case SCREEN_CALC_COLOUR:
        case SCREEN_CALC_COLOUR_4BAND: show_calc_colour();      break;
        case SCREEN_CALC_COLOUR_5BAND: show_calc_colour_5band(); break;
        default: break;
    }
}

void show_calc_menu() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "Calculators", SCREEN_HOME);

    struct { const char *name; const char *desc; AppScreen t; } calcs[] = {
        {"LED Resistor",   "Find resistor value for LED",    SCREEN_CALC_RESISTOR},
        {"Power / Current","P=VI  V=IR  I=P/V calculator",   SCREEN_CALC_POWER},
        {"4-Band Code",    "Calculate your 4-band resistor", SCREEN_CALC_COLOUR_4BAND},
        {"5-Band Code",    "Calculate your 5-band resistor", SCREEN_CALC_COLOUR_5BAND},
    };

    for (size_t i = 0; i < sizeof(calcs)/sizeof(calcs[0]); i++) {
        lv_obj_t *btn = lv_button_create(scr);
        lv_obj_set_size(btn, 290, 64);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 60 + i * 76);
        lv_obj_set_style_bg_color(btn, COL_CARD, 0);
        lv_obj_set_style_border_color(btn, lv_color_make(200, 120, 0), 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, calc_cb, LV_EVENT_CLICKED,
                            (void*)(intptr_t)calcs[i].t);

        lv_obj_t *name = lv_label_create(btn);
        lv_label_set_text(name, calcs[i].name);
        lv_obj_set_style_text_color(name, COL_TEXT, 0);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_14, 0);
        lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, -8);

        lv_obj_t *sub = lv_label_create(btn);
        lv_label_set_text(sub, calcs[i].desc);
        lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
        lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
        lv_obj_align(sub, LV_ALIGN_LEFT_MID, 10, 12);
    }
    lv_vendor_disp_unlock();
}

// ============================================================
// LED RESISTOR CALCULATOR
// ============================================================
void show_calc_resistor() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "LED Resistor Calc", SCREEN_CALC_MENU);

    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "R = (Vcc - Vf) / If");
    lv_obj_set_style_text_color(info, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 58);

    vcc_val_lbl = make_spin_row(scr, "Vcc (Supply V)", vcc_dec_cb, vcc_inc_cb, 90);
    vf_val_lbl  = make_spin_row(scr, "Vf  (LED Fwd V)", vf_dec_cb,  vf_inc_cb,  136);
    if_val_lbl  = make_spin_row(scr, "If  (LED mA)",    if_dec_cb,  if_inc_cb,  182);

    lv_obj_t *res_box = lv_obj_create(scr);
    lv_obj_set_size(res_box, 290, 54);
    lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 240);
    lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
    lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
    lv_obj_set_style_border_width(res_box, 2, 0);
    lv_obj_set_style_radius(res_box, 10, 0);

    res_result_lbl = lv_label_create(res_box);
    lv_obj_set_style_text_color(res_result_lbl, COL_ACCENT, 0);
    lv_obj_set_style_text_font(res_result_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(res_result_lbl, LV_ALIGN_CENTER, 0, 0);
    update_resistor_result();

    lv_obj_t *ref = lv_label_create(scr);
    lv_label_set_text(ref,
        "Common Vf: Red 1.8V  Green 2.1V\n"
        "Blue 3.3V  White 3.3V  IR 1.2V");
    lv_obj_set_style_text_color(ref, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(ref, &lv_font_montserrat_14, 0);
    lv_obj_align(ref, LV_ALIGN_TOP_MID, 0, 308);
    lv_vendor_disp_unlock();
}

// ============================================================
// POWER CALCULATOR
// ============================================================
void show_calc_power() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "Power Calculator", SCREEN_CALC_MENU);

    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "P = V x I        V = I x R");
    lv_obj_set_style_text_color(info, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 58);

    pw_v_val_lbl = make_spin_row(scr, "Voltage (V)", pv_dec_cb, pv_inc_cb, 100);
    pw_a_val_lbl = make_spin_row(scr, "Current (A)", pa_dec_cb, pa_inc_cb, 148);

    lv_obj_t *res_box = lv_obj_create(scr);
    lv_obj_set_size(res_box, 290, 54);
    lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 206);
    lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
    lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
    lv_obj_set_style_border_width(res_box, 2, 0);
    lv_obj_set_style_radius(res_box, 10, 0);

    pw_result_lbl = lv_label_create(res_box);
    lv_obj_set_style_text_color(pw_result_lbl, COL_ACCENT, 0);
    lv_obj_set_style_text_font(pw_result_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(pw_result_lbl, LV_ALIGN_CENTER, 0, 0);
    update_power_result();

    lv_obj_t *ws = lv_label_create(scr);
    lv_label_set_text(ws,
        "WS2812B: 1 LED full = 60mA @ 5V = 0.3W\n"
        "30 LEDs = 1.8A minimum PSU");
    lv_obj_set_style_text_color(ws, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(ws, &lv_font_montserrat_14, 0);
    lv_obj_align(ws, LV_ALIGN_TOP_MID, 0, 276);
    lv_vendor_disp_unlock();
}
