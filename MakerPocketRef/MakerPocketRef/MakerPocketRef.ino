/**
 * @file MakerPocketRef.ino
 * @brief Maker's Pocket Reference App for Tuya T5AI Board
 * @author CMozMaker / Tinker Tailor
 *
 * A handheld reference tool for electronics makers.
 * Displays colour-coded pinout diagrams, component wiring info,
 * and quick calculators for common maker tasks.
 *
 * Hardware: Tuya T5AI Board V1.0.2 + 3.5" ILI9488 touchscreen (320x480)
 * Framework: Arduino + LVGL v9.1.0
 *
 * @note Add appConfig.json to sketch folder before compiling!
 * @note Select board: T5 (not TUYA_T5AI_BOARD) for uploading on Windows
 * @note There will be 2 ports displayed, if it doesn't upload, try the other.
 */

#include "Log.h"
#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "board_com_api.h"
#include "lvgl.h"
#include "lv_vendor.h"

// ============================================================
// COLOUR PALETTE - Pin category colours (RGB565-friendly)
// ============================================================
#define COL_POWER       lv_color_make(220, 50,  50)   // Red    - VCC/3V3/5V
#define COL_GND         lv_color_make(60,  60,  60)   // Dark   - GND
#define COL_GPIO        lv_color_make(50,  180, 80)   // Green  - Digital GPIO
#define COL_ADC         lv_color_make(230, 140, 0)    // Orange - Analog/ADC
#define COL_SPI         lv_color_make(50,  120, 220)  // Blue   - SPI
#define COL_I2C         lv_color_make(180, 160, 0)    // Yellow - I2C
#define COL_UART        lv_color_make(160, 50,  200)  // Purple - UART
#define COL_PWM         lv_color_make(0,   190, 200)  // Cyan   - PWM
#define COL_TOUCH       lv_color_make(200, 100, 180)  // Pink   - Touch
#define COL_SPECIAL     lv_color_make(140, 140, 140)  // Grey   - EN/BOOT/NC

#define COL_BG          lv_color_make(18,  18,  18)   // Dark background
#define COL_CARD        lv_color_make(30,  30,  30)   // Card background
#define COL_HEADER      lv_color_make(25,  25,  25)   // Header bar
#define COL_TEXT        lv_color_make(240, 240, 240)  // Main text
#define COL_TEXT_DIM    lv_color_make(140, 140, 140)  // Dim text
#define COL_ACCENT      lv_color_make(0,   200, 180)  // Teal accent

// ============================================================
// DATA STRUCTURES
// ============================================================
typedef struct {
    const char *name;
    const char *label;   // short label for pin box e.g. "3V3"
    lv_color_t  color;
} PinInfo;

typedef struct {
    const char  *name;
    const char  *subtitle;
    int          pin_count;
    PinInfo      left_pins[20];   // left column
    PinInfo      right_pins[20];  // right column
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

// ============================================================
// ESP32 DEVKIT V1 PIN DATA (38-pin, dual column)
// ============================================================
static BoardInfo board_esp32_devkit = {
    "ESP32 DevKit V1",
    "38-pin | 3.3V logic | Dual-core 240MHz",
    19,
    // Left column (top to bottom)
    {
        {"3V3",       "3V3",    COL_POWER},
        {"GND",       "GND",    COL_GND},
        {"D15/ADC2_3","IO15",   COL_ADC},
        {"D2/ADC2_2", "IO2",    COL_ADC},
        {"D4/ADC2_0", "IO4",    COL_ADC},
        {"RX2",       "RX2",    COL_UART},
        {"TX2",       "TX2",    COL_UART},
        {"D5/SS",     "IO5",    COL_SPI},
        {"D18/SCK",   "SCK",    COL_SPI},
        {"D19/MISO",  "MISO",   COL_SPI},
        {"D21/SDA",   "SDA",    COL_I2C},
        {"RX0",       "RX0",    COL_UART},
        {"TX0",       "TX0",    COL_UART},
        {"D22/SCL",   "SCL",    COL_I2C},
        {"D23/MOSI",  "MOSI",   COL_SPI},
        {"GND",       "GND",    COL_GND},
        {"D1/TX",     "TX",     COL_UART},
        {"D3/RX",     "RX",     COL_UART},
        {"D36/A0",    "VP",     COL_ADC},
    },
    // Right column (top to bottom)
    {
        {"EN",        "EN",     COL_SPECIAL},
        {"D36/VP",    "VP",     COL_ADC},
        {"D39/VN",    "VN",     COL_ADC},
        {"D34",       "IO34",   COL_GPIO},
        {"D35",       "IO35",   COL_GPIO},
        {"D32",       "IO32",   COL_ADC},
        {"D33",       "IO33",   COL_ADC},
        {"D25/DAC1",  "DAC1",   COL_ADC},
        {"D26/DAC2",  "DAC2",   COL_ADC},
        {"D27",       "IO27",   COL_GPIO},
        {"D14",       "IO14",   COL_GPIO},
        {"D12",       "IO12",   COL_GPIO},
        {"GND",       "GND",    COL_GND},
        {"D13",       "IO13",   COL_GPIO},
        {"SD2",       "SD2",    COL_SPECIAL},
        {"SD3",       "SD3",    COL_SPECIAL},
        {"CMD",       "CMD",    COL_SPECIAL},
        {"5V",        "5V",     COL_POWER},
        {"GND",       "GND",    COL_GND},
    }
};

// ============================================================
// ESP32-S3 DEVKIT-C PIN DATA (simplified, 20 per side)
// ============================================================
static BoardInfo board_esp32s3 = {
    "ESP32-S3 DevKit-C",
    "44-pin | 3.3V | 240MHz | USB-OTG",
    20,
    {
        {"3V3",       "3V3",    COL_POWER},
        {"3V3",       "3V3",    COL_POWER},
        {"RST",       "RST",    COL_SPECIAL},
        {"IO4/ADC",   "IO4",    COL_ADC},
        {"IO5/ADC",   "IO5",    COL_ADC},
        {"IO6/ADC",   "IO6",    COL_ADC},
        {"IO7/ADC",   "IO7",    COL_ADC},
        {"IO15",      "IO15",   COL_GPIO},
        {"IO16",      "IO16",   COL_GPIO},
        {"IO17",      "IO17",   COL_GPIO},
        {"IO18",      "IO18",   COL_GPIO},
        {"IO8/SDA",   "SDA",    COL_I2C},
        {"IO3",       "IO3",    COL_GPIO},
        {"IO46",      "IO46",   COL_GPIO},
        {"IO9",       "IO9",    COL_GPIO},
        {"IO10/MOSI", "MOSI",   COL_SPI},
        {"IO11/MISO", "MISO",   COL_SPI},
        {"IO12/SCK",  "SCK",    COL_SPI},
        {"IO13/SS",   "SS",     COL_SPI},
        {"GND",       "GND",    COL_GND},
    },
    {
        {"GND",       "GND",    COL_GND},
        {"5V",        "5V",     COL_POWER},
        {"IO43/TX",   "TX",     COL_UART},
        {"IO44/RX",   "RX",     COL_UART},
        {"IO1/ADC",   "IO1",    COL_ADC},
        {"IO2/ADC",   "IO2",    COL_ADC},
        {"IO42/SCL",  "SCL",    COL_I2C},
        {"IO41",      "IO41",   COL_GPIO},
        {"IO40",      "IO40",   COL_GPIO},
        {"IO39",      "IO39",   COL_GPIO},
        {"IO38",      "IO38",   COL_GPIO},
        {"IO37",      "IO37",   COL_GPIO},
        {"IO36",      "IO36",   COL_GPIO},
        {"IO35",      "IO35",   COL_GPIO},
        {"IO0/BOOT",  "BOOT",   COL_SPECIAL},
        {"IO45",      "IO45",   COL_GPIO},
        {"IO48",      "IO48",   COL_GPIO},
        {"IO47",      "IO47",   COL_GPIO},
        {"IO21",      "IO21",   COL_GPIO},
        {"GND",       "GND",    COL_GND},
    }
};

// ============================================================
// WS2812B COMPONENT DATA
// ============================================================
static ComponentInfo comp_ws2812b = {
    "WS2812B LED",
    "Addressable RGB LED with integrated driver",
    "5V (3.5V-5.3V)",
    "Single-wire (800kHz)",
    "60mA max per LED at full white. Budget 20mA avg.",
    {
        "VDD  → 5V (use capacitor 100uF across power)",
        "GND  → Ground",
        "DIN  → Data pin (via 300-500Ω resistor)",
        "DOUT → DIN of next LED in chain",
        "Logic level: 3.3V MCUs need level shifter",
        "Data line: add 10K pull-down to prevent glitches",
    },
    {
        "Add 0.1uF ceramic cap per LED near VDD/GND",
        "First pixel resistor prevents ringing on data line",
        "Keep data wire short, twisted pair if possible",
        "FastLED: addLeds<WS2812B, PIN, GRB>(leds, NUM)",
    }
};

// ============================================================
// SCREEN MANAGEMENT
// ============================================================
typedef enum {
    SCREEN_HOME = 0,
    SCREEN_BOARD_LIST,
    SCREEN_BOARD_DETAIL,
    SCREEN_COMPONENT_LIST,
    SCREEN_COMPONENT_DETAIL,
    SCREEN_CALC_MENU,
    SCREEN_CALC_RESISTOR,
    SCREEN_CALC_POWER,
} AppScreen;

static AppScreen current_screen = SCREEN_HOME;
static int selected_board = 0;
static int selected_component = 0;

static BoardInfo *boards[] = { &board_esp32_devkit, &board_esp32s3 };
static ComponentInfo *components[] = { &comp_ws2812b };
static int board_count = 2;
static int component_count = 1;

// ============================================================
// LEGEND DATA
// ============================================================
static const struct { const char *label; lv_color_t color; } legend[] = {
    {"PWR",  COL_POWER},
    {"GND",  COL_GND},
    {"GPIO", COL_GPIO},
    {"ADC",  COL_ADC},
    {"SPI",  COL_SPI},
    {"I2C",  COL_I2C},
    {"UART", COL_UART},
    {"PWM",  COL_PWM},
};

// ============================================================
// FORWARD DECLARATIONS
// ============================================================
void show_home_screen();
void show_board_list();
void show_board_detail(int index);
void show_component_list();
void show_component_detail(int index);
void show_calc_menu();
void show_calc_resistor();
void show_calc_power();

// ============================================================
// HELPER: Create header bar with back button
// ============================================================
static void back_btn_cb(lv_event_t *e) {
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_HOME:           show_home_screen(); break;
        case SCREEN_BOARD_LIST:     show_board_list(); break;
        case SCREEN_COMPONENT_LIST: show_component_list(); break;
        case SCREEN_CALC_MENU:      show_calc_menu(); break;
        default:                    show_home_screen(); break;
    }
}

static lv_obj_t* create_header(lv_obj_t *parent, const char *title, AppScreen back_target) {
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, 320, 48);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, COL_HEADER, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);

    // Back button
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

    // Title
    lv_obj_t *title_lbl = lv_label_create(header);
    lv_label_set_text(title_lbl, title);
    lv_obj_set_style_text_color(title_lbl, COL_TEXT, 0);
    lv_obj_set_style_text_font(title_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(title_lbl, LV_ALIGN_CENTER, 24, 0);

    // Bottom border line
    lv_obj_t *line = lv_obj_create(parent);
    lv_obj_set_size(line, 320, 2);
    lv_obj_set_pos(line, 0, 48);
    lv_obj_set_style_bg_color(line, COL_ACCENT, 0);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 0, 0);

    return header;
}

// ============================================================
// HELPER: Create a colour-coded pin box
// ============================================================
static void create_pin_box(lv_obj_t *parent, int x, int y,
                            int w, const char *label, lv_color_t color) {
    lv_obj_t *box = lv_obj_create(parent);
    lv_obj_set_size(box, w, 22);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_style_bg_color(box, color, 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(box, 0, 0);
    lv_obj_set_style_radius(box, 3, 0);
    lv_obj_set_style_pad_all(box, 0, 0);

    lv_obj_t *lbl = lv_label_create(box);
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);
}

// ============================================================
// HOME SCREEN
// ============================================================
static void home_btn_cb(lv_event_t *e) {
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_BOARD_LIST:     show_board_list(); break;
        case SCREEN_COMPONENT_LIST: show_component_list(); break;
        case SCREEN_CALC_MENU:      show_calc_menu(); break;
        default: break;
    }
}

void show_home_screen() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    // App title
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Maker's\nPocket Reference");
    lv_obj_set_style_text_color(title, COL_ACCENT, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 24);

    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "tinkertailor.ca");
    lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 88);

    // Category buttons
    struct { const char *icon; const char *label; AppScreen target; lv_color_t col; } btns[] = {
        {LV_SYMBOL_LIST,   "Boards",      SCREEN_BOARD_LIST,     lv_color_make(50, 120, 220)},
        {LV_SYMBOL_CHARGE, "Components",  SCREEN_COMPONENT_LIST, lv_color_make(50, 180, 80)},
        {LV_SYMBOL_SHUFFLE,"Calculators", SCREEN_CALC_MENU,      lv_color_make(200, 120, 0)},
    };

    int btn_y = 130;
    for (int i = 0; i < 3; i++) {
        lv_obj_t *btn = lv_button_create(scr);
        lv_obj_set_size(btn, 280, 80);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, btn_y + i * 100);
        lv_obj_set_style_bg_color(btn, COL_CARD, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(btn, btns[i].col, 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_radius(btn, 14, 0);
        lv_obj_add_event_cb(btn, home_btn_cb, LV_EVENT_CLICKED,
                            (void*)(intptr_t)btns[i].target);

        lv_obj_t *icon = lv_label_create(btn);
        lv_label_set_text(icon, btns[i].icon);
        lv_obj_set_style_text_color(icon, btns[i].col, 0);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 20, 0);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, btns[i].label);
        lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_24, 0);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 72, 0);

        lv_obj_t *arrow = lv_label_create(btn);
        lv_label_set_text(arrow, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_color(arrow, COL_TEXT_DIM, 0);
        lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -16, 0);
    }

    lv_vendor_disp_unlock();
}

// ============================================================
// BOARD LIST SCREEN
// ============================================================
static void board_list_cb(lv_event_t *e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    show_board_detail(idx);
}

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
                            (void*)(intptr_t)i);

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
// BOARD DETAIL SCREEN - Colour Coded Pinout Diagram
// ============================================================
void show_board_detail(int index) {
    selected_board = index;
    BoardInfo *board = boards[index];

    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    create_header(scr, board->name, SCREEN_BOARD_LIST);

    // Scrollable content area
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 320, 428);
    lv_obj_set_pos(cont, 0, 52);
    lv_obj_set_style_bg_color(cont, COL_BG, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);

    // Subtitle
    lv_obj_t *sub = lv_label_create(cont);
    lv_label_set_text(sub, board->subtitle);
    lv_obj_set_style_text_color(sub, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(sub, 10, 8);

    // Pin diagram title
    lv_obj_t *ptitle = lv_label_create(cont);
    lv_label_set_text(ptitle, "PINOUT");
    lv_obj_set_style_text_color(ptitle, COL_ACCENT, 0);
    lv_obj_set_style_text_font(ptitle, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(ptitle, 10, 28);

    // Draw pins - two column layout
    // Left column: x=8, Right column: x=180
    // Each pin box: 128px wide, 22px tall, 2px gap
    int pin_start_y = 50;
    int row_h = 24;
    int left_x = 8;
    int right_x = 180;
    int pin_w = 130;

    for (int i = 0; i < board->pin_count; i++) {
        // Left pin
        char left_buf[16];
        snprintf(left_buf, sizeof(left_buf), "%s", board->left_pins[i].label);
        create_pin_box(cont, left_x, pin_start_y + i * row_h,
                       pin_w, left_buf, board->left_pins[i].color);

        // Right pin
        char right_buf[16];
        snprintf(right_buf, sizeof(right_buf), "%s", board->right_pins[i].label);
        create_pin_box(cont, right_x, pin_start_y + i * row_h,
                       pin_w, right_buf, board->right_pins[i].color);
    }

    // Legend below pins
    int legend_y = pin_start_y + board->pin_count * row_h + 10;
    lv_obj_t *leg_title = lv_label_create(cont);
    lv_label_set_text(leg_title, "LEGEND");
    lv_obj_set_style_text_color(leg_title, COL_ACCENT, 0);
    lv_obj_set_style_text_font(leg_title, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(leg_title, 10, legend_y);

    int lx = 8, ly = legend_y + 20;
    for (int i = 0; i < 8; i++) {
        create_pin_box(cont, lx + (i % 4) * 76, ly + (i / 4) * 26,
                       70, legend[i].label, legend[i].color);
    }

    lv_vendor_disp_unlock();
}

// ============================================================
// COMPONENT LIST SCREEN
// ============================================================
static void comp_list_cb(lv_event_t *e) {
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
    }
    lv_vendor_disp_unlock();
}

// ============================================================
// COMPONENT DETAIL SCREEN
// ============================================================
void show_component_detail(int index) {
    selected_component = index;
    ComponentInfo *comp = components[index];

    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    create_header(scr, comp->name, SCREEN_COMPONENT_LIST);

    // Scrollable content
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 320, 428);
    lv_obj_set_pos(cont, 0, 52);
    lv_obj_set_style_bg_color(cont, COL_BG, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_hor(cont, 12, 0);
    lv_obj_set_style_pad_top(cont, 8, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);

    int y = 0;
    auto add_section = [&](const char *heading, const char *value) {
        lv_obj_t *h = lv_label_create(cont);
        lv_label_set_text(h, heading);
        lv_obj_set_style_text_color(h, COL_ACCENT, 0);
        lv_obj_set_style_text_font(h, &lv_font_montserrat_14, 0);
        lv_obj_set_pos(h, 0, y);
        y += 18;

        lv_obj_t *v = lv_label_create(cont);
        lv_label_set_text(v, value);
        lv_obj_set_style_text_color(v, COL_TEXT, 0);
        lv_obj_set_style_text_font(v, &lv_font_montserrat_14, 0);
        lv_obj_set_width(v, 290);
        lv_label_set_long_mode(v, LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(v, 4, y);
        y += lv_obj_get_height(v) + 12;
    };

    add_section("VOLTAGE", comp->voltage);
    add_section("DATA", comp->dataline);
    add_section("POWER NOTE", comp->power_note);

    // Wiring section
    lv_obj_t *wh = lv_label_create(cont);
    lv_label_set_text(wh, "WIRING");
    lv_obj_set_style_text_color(wh, COL_ACCENT, 0);
    lv_obj_set_style_text_font(wh, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(wh, 0, y);
    y += 18;

    for (int i = 0; i < 6 && comp->wiring[i]; i++) {
        lv_obj_t *w = lv_label_create(cont);
        char buf[80];
        snprintf(buf, sizeof(buf), "• %s", comp->wiring[i]);
        lv_label_set_text(w, buf);
        lv_obj_set_style_text_color(w, COL_TEXT, 0);
        lv_obj_set_style_text_font(w, &lv_font_montserrat_14, 0);
        lv_obj_set_width(w, 290);
        lv_label_set_long_mode(w, LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(w, 4, y);
        y += lv_obj_get_height(w) + 4;
    }

    y += 8;

    // Tips section
    lv_obj_t *th = lv_label_create(cont);
    lv_label_set_text(th, "TIPS");
    lv_obj_set_style_text_color(th, COL_ACCENT, 0);
    lv_obj_set_style_text_font(th, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(th, 0, y);
    y += 18;

    for (int i = 0; i < 4 && comp->tips[i]; i++) {
        lv_obj_t *t = lv_label_create(cont);
        char buf[100];
        snprintf(buf, sizeof(buf), "• %s", comp->tips[i]);
        lv_label_set_text(t, buf);
        lv_obj_set_style_text_color(t, COL_TEXT_DIM, 0);
        lv_obj_set_style_text_font(t, &lv_font_montserrat_14, 0);
        lv_obj_set_width(t, 290);
        lv_label_set_long_mode(t, LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(t, 4, y);
        y += lv_obj_get_height(t) + 4;
    }

    lv_vendor_disp_unlock();
}

// ============================================================
// CALCULATOR MENU SCREEN
// ============================================================
static void calc_cb(lv_event_t *e) {
    AppScreen target = (AppScreen)(intptr_t)lv_event_get_user_data(e);
    switch (target) {
        case SCREEN_CALC_RESISTOR: show_calc_resistor(); break;
        case SCREEN_CALC_POWER:    show_calc_power(); break;
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
        {"LED Resistor",   "Find resistor value for LED",   SCREEN_CALC_RESISTOR},
        {"Power / Current","P=VI  V=IR  I=P/V calculator",  SCREEN_CALC_POWER},
    };

    for (int i = 0; i < 2; i++) {
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
static float calc_vcc     = 5.0f;
static float calc_vf      = 2.0f;   // Forward voltage
static float calc_if_ma   = 20.0f;  // Forward current mA
static lv_obj_t *res_result_lbl = NULL;

static void update_resistor_result() {
    if (!res_result_lbl) return;
    float r = (calc_vcc - calc_vf) / (calc_if_ma / 1000.0f);
    char buf[60];
    // Round up to nearest standard value suggestion
    snprintf(buf, sizeof(buf), "R = %.0f Ω  (use %.0f Ω)", r,
             r < 100 ? 100.0f : r < 220 ? 220.0f : r < 330 ? 330.0f :
             r < 470 ? 470.0f : r < 560 ? 560.0f : r < 680 ? 680.0f :
             r < 820 ? 820.0f : r < 1000 ? 1000.0f : r);
    lv_label_set_text(res_result_lbl, buf);
}

static void vcc_inc_cb(lv_event_t *e)  { calc_vcc  += 0.5f; update_resistor_result(); }
static void vcc_dec_cb(lv_event_t *e)  { if (calc_vcc > 1.0f) { calc_vcc -= 0.5f; update_resistor_result(); } }
static void vf_inc_cb(lv_event_t *e)   { calc_vf   += 0.1f; update_resistor_result(); }
static void vf_dec_cb(lv_event_t *e)   { if (calc_vf > 0.1f) { calc_vf -= 0.1f; update_resistor_result(); } }
static void if_inc_cb(lv_event_t *e)   { calc_if_ma += 1.0f; update_resistor_result(); }
static void if_dec_cb(lv_event_t *e)   { if (calc_if_ma > 1.0f) { calc_if_ma -= 1.0f; update_resistor_result(); } }

static lv_obj_t* make_spin_row(lv_obj_t *parent, const char *label,
                                 lv_event_cb_t dec_cb, lv_event_cb_t inc_cb, int y) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, label);
    lv_obj_set_style_text_color(lbl, COL_TEXT, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(lbl, 10, y + 8);

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
    lv_obj_set_pos(val, 214, y + 8);
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

    return val; // return the value label so caller can update it
}

void show_calc_resistor() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    create_header(scr, "LED Resistor Calc", SCREEN_CALC_MENU);

    // Info
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "R = (Vcc - Vf) / If");
    lv_obj_set_style_text_color(info, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 58);

    lv_obj_t *vcc_lbl = make_spin_row(scr, "Vcc (Supply V)", vcc_dec_cb, vcc_inc_cb, 90);
    lv_obj_t *vf_lbl  = make_spin_row(scr, "Vf  (LED Fwd V)", vf_dec_cb,  vf_inc_cb,  144);
    lv_obj_t *if_lbl  = make_spin_row(scr, "If  (LED mA)",    if_dec_cb,  if_inc_cb,  198);

    // Update value labels
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1fV", calc_vcc);
    lv_label_set_text(vcc_lbl, buf);
    snprintf(buf, sizeof(buf), "%.1fV", calc_vf);
    lv_label_set_text(vf_lbl, buf);
    snprintf(buf, sizeof(buf), "%.0fmA", calc_if_ma);
    lv_label_set_text(if_lbl, buf);

    // Result box
    lv_obj_t *res_box = lv_obj_create(scr);
    lv_obj_set_size(res_box, 290, 60);
    lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 264);
    lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
    lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
    lv_obj_set_style_border_width(res_box, 2, 0);
    lv_obj_set_style_radius(res_box, 10, 0);

    res_result_lbl = lv_label_create(res_box);
    lv_obj_set_style_text_color(res_result_lbl, COL_ACCENT, 0);
    lv_obj_set_style_text_font(res_result_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(res_result_lbl, LV_ALIGN_CENTER, 0, 0);
    update_resistor_result();

    // Common Vf reference
    lv_obj_t *ref = lv_label_create(scr);
    lv_label_set_text(ref,
        "Common Vf values:\n"
        "Red 1.8V  Green 2.1V  Blue 3.3V\n"
        "White 3.3V  Yellow 2.1V  IR 1.2V");
    lv_obj_set_style_text_color(ref, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(ref, &lv_font_montserrat_14, 0);
    lv_obj_align(ref, LV_ALIGN_TOP_MID, 0, 340);

    lv_vendor_disp_unlock();
}

// ============================================================
// POWER CALCULATOR (P=VI)
// ============================================================
static float pw_volts = 5.0f;
static float pw_amps  = 0.5f;
static lv_obj_t *pw_result_lbl = NULL;

static void update_power_result() {
    if (!pw_result_lbl) return;
    float p = pw_volts * pw_amps;
    float r = pw_volts / pw_amps;
    char buf[80];
    snprintf(buf, sizeof(buf), "P = %.2fW    R = %.1f Ω", p, r);
    lv_label_set_text(pw_result_lbl, buf);
}

static void pv_inc_cb(lv_event_t *e) { pw_volts += 0.5f; update_power_result(); }
static void pv_dec_cb(lv_event_t *e) { if (pw_volts > 0.5f) { pw_volts -= 0.5f; update_power_result(); } }
static void pa_inc_cb(lv_event_t *e) { pw_amps  += 0.1f; update_power_result(); }
static void pa_dec_cb(lv_event_t *e) { if (pw_amps > 0.1f) { pw_amps -= 0.1f; update_power_result(); } }

void show_calc_power() {
    lv_vendor_disp_lock();
    lv_obj_clean(lv_screen_active());
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);

    create_header(scr, "Power Calculator", SCREEN_CALC_MENU);

    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "P = V × I        V = I × R");
    lv_obj_set_style_text_color(info, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 58);

    lv_obj_t *v_lbl = make_spin_row(scr, "Voltage (V)", pv_dec_cb, pv_inc_cb, 100);
    lv_obj_t *a_lbl = make_spin_row(scr, "Current (A)", pa_dec_cb, pa_inc_cb, 154);

    char buf[16];
    snprintf(buf, sizeof(buf), "%.1fV", pw_volts);
    lv_label_set_text(v_lbl, buf);
    snprintf(buf, sizeof(buf), "%.1fA", pw_amps);
    lv_label_set_text(a_lbl, buf);

    // Result box
    lv_obj_t *res_box = lv_obj_create(scr);
    lv_obj_set_size(res_box, 290, 60);
    lv_obj_align(res_box, LV_ALIGN_TOP_MID, 0, 220);
    lv_obj_set_style_bg_color(res_box, COL_CARD, 0);
    lv_obj_set_style_border_color(res_box, COL_ACCENT, 0);
    lv_obj_set_style_border_width(res_box, 2, 0);
    lv_obj_set_style_radius(res_box, 10, 0);

    pw_result_lbl = lv_label_create(res_box);
    lv_obj_set_style_text_color(pw_result_lbl, COL_ACCENT, 0);
    lv_obj_set_style_text_font(pw_result_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(pw_result_lbl, LV_ALIGN_CENTER, 0, 0);
    update_power_result();

    // WS2812B quick ref
    lv_obj_t *ws = lv_label_create(scr);
    lv_label_set_text(ws,
        "WS2812B quick ref:\n"
        "1 LED full white = 60mA @ 5V = 0.3W\n"
        "30 LEDs = 1.8A = 9W minimum PSU");
    lv_obj_set_style_text_color(ws, COL_TEXT_DIM, 0);
    lv_obj_set_style_text_font(ws, &lv_font_montserrat_14, 0);
    lv_obj_align(ws, LV_ALIGN_TOP_MID, 0, 300);

    lv_vendor_disp_unlock();
}

// ============================================================
// SETUP & LOOP
// ============================================================
void setup(void) {
    Serial.begin(115200);
    Log.begin(1024);

    PR_NOTICE("========================================");
    PR_NOTICE("Maker's Pocket Reference v1.0");
    PR_NOTICE("CMozMaker / Tinker Tailor");
    PR_NOTICE("========================================");

    if (OPRT_OK != board_register_hardware()) {
        PR_ERR("board_register_hardware error!");
        while(1);
    }

    static char display_name[] = "display";
    lv_vendor_init(display_name);
    lv_vendor_disp_lock();
    show_home_screen();
    lv_vendor_disp_unlock();
    lv_vendor_start(5, 1024 * 8);

    PR_NOTICE("App started!");
}

void loop() {
    delay(10);
}
