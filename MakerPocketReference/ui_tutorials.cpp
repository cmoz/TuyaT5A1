// ui_tutorials.cpp
// QR codes drawn using the ricmoo/QRCode library (qrcode.h + qrcode.c in sketch folder)
// No lv_qrcode needed — modules are rendered as small lv_obj rectangles on a white card.
#include "ui_tutorials.h"
#include "ui_common.h"
#include "colours.h"
#include "sound.h"
#include "lvgl.h"
#include "lv_vendor.h"
#include <cstdio>
#include <string.h>

// ricmoo/QRCode library — qrcode.h and qrcode.c must be in the sketch folder
extern "C" {
#include "qrcode.h"
}

// ============================================================
// TUTORIALS DATA
// ============================================================
typedef struct {
    const char *title;
    const char *url;
} TutorialEntry;

static const TutorialEntry tutorials[] = {
    {"Tired of blinking LED?\nTry this instead",  "https://youtu.be/byPnBiiqz9o"},
    {"Tap to trigger:\nRFID + ESP32 = Awesome",   "https://youtu.be/7sqpxrMZuvo"},
    {"Easy OLED project you\ncan finish today",   "https://youtu.be/wiu2lC0JfTA"},
};
static const int tutorial_count = 3;
static int selected_tutorial = 0;

static void show_tutorial_qr_fwd(int index);

static void tutorial_item_cb(lv_event_t *e) {
    sound_play_click();
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    show_tutorial_qr_fwd(idx);
}

// ============================================================
// TUTORIALS LIST SCREEN
// ============================================================
void show_tutorials() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "CMozMaker Tutorials", SCREEN_HOME);

    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "Tap a video to get the QR code");
    lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 56);

    for (int i = 0; i < tutorial_count; i++) {
        lv_obj_t *btn = lv_button_create(scr);
        lv_obj_set_size(btn, 290, 72);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 78 + i * 84);
        lv_obj_set_style_bg_color(btn, COL_CARD, 0);
        lv_obj_set_style_border_color(btn, lv_color_make(220, 50, 80), 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, tutorial_item_cb, LV_EVENT_CLICKED,
                            (void*)(intptr_t)i);

        lv_obj_t *icon = lv_label_create(btn);
        lv_label_set_text(icon, LV_SYMBOL_PLAY);
        lv_obj_set_style_text_color(icon, lv_color_make(220, 50, 80), 0);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 8, 0);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, tutorials[i].title);
        lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_width(lbl, 220);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 44, 0);

        lv_obj_t *arrow = lv_label_create(btn);
        lv_label_set_text(arrow, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_color(arrow, COL_TEXT_DIM, 0);
        lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -8, 0);
    }
    lv_vendor_disp_unlock();
}

// ============================================================
// QR CODE DRAWING
// Uses ricmoo/QRCode to generate the bitmap, then draws each
// dark module as a small lv_obj child on a white card.
//
// QR version 4 = 33x33 modules, handles youtu.be URLs (~28 chars)
// at ECC_LOW. Scale = 5px/module + 8px quiet zone each side.
// White card = 33*5 + 16 = 181px square.
// ============================================================
#define QR_VERSION  4
#define QR_SCALE    5
#define QR_QUIET    8   // pixels (not modules) of white border

static void draw_qr_on_card(lv_obj_t *card, const char *url) {
    QRCode qrcode;
    // Stack-allocate the data buffer — getBufferSize(4) = 76 bytes, fine
    uint8_t qrdata[qrcode_getBufferSize(QR_VERSION)];

    int err = qrcode_initText(&qrcode, qrdata, QR_VERSION, ECC_LOW, url);
    if (err < 0) {
        // Fallback: show URL text if QR generation fails
        lv_obj_t *lbl = lv_label_create(card);
        lv_label_set_text(lbl, url);
        lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
        lv_obj_set_width(lbl, 180);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
        lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);
        return;
    }

    // Size the card to fit the QR
    int card_px = qrcode.size * QR_SCALE + QR_QUIET * 2;
    lv_obj_set_size(card, card_px, card_px);

    // Draw each dark module as a tiny black box child
    for (int y = 0; y < qrcode.size; y++) {
        for (int x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                lv_obj_t *dot = lv_obj_create(card);
                lv_obj_set_size(dot, QR_SCALE, QR_SCALE);
                lv_obj_set_pos(dot, QR_QUIET + x * QR_SCALE,
                                    QR_QUIET + y * QR_SCALE);
                lv_obj_set_style_bg_color(dot, lv_color_black(), 0);
                lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
                lv_obj_set_style_border_width(dot, 0, 0);
                lv_obj_set_style_radius(dot, 0, 0);
                lv_obj_set_style_pad_all(dot, 0, 0);
                // Disable scrolling/clicking on dots
                lv_obj_remove_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
            }
        }
    }
}

// ============================================================
// TUTORIAL QR SCREEN
// ============================================================
void show_tutorial_qr(int index) {
    selected_tutorial = index;
    const TutorialEntry *t = &tutorials[index];

    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    create_header(scr, "Scan to Watch", SCREEN_TUTORIALS);

    // Video title
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, t->title);
    lv_obj_set_style_text_color(title, COL_TEXT, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_width(title, 290);
    lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 56);

    // White card — size will be set by draw_qr_on_card()
    lv_obj_t *card = lv_obj_create(scr);
    lv_obj_set_style_bg_color(card, lv_color_white(), 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 6, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 108);

    draw_qr_on_card(card, t->url);

    // Hint below QR
    lv_obj_t *hint = lv_label_create(scr);
    lv_label_set_text(hint, "Point your phone camera at the QR code");
    lv_obj_set_style_text_color(hint, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(hint, 290);
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 0, 350);

    lv_vendor_disp_unlock();
}

static void show_tutorial_qr_fwd(int index) {
    show_tutorial_qr(index);
}
