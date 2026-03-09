/**
 * @file MakerPocketReferenceV8.ino
 * @brief Maker's Pocket Reference App for Tuya T5AI Board
 * @author CMozMaker / Tinker Tailor
 *
 * Hardware: Tuya T5AI Board V1.0.2 + 3.5" ILI9488 touchscreen (320x480)
 * Framework: Arduino + LVGL v9.1.0
 *
 * @note Add appConfig.json to sketch folder before compiling!
 * @note Select board: TUYA_T5AI_BOARD for upload
 * @note QR codes: copy qrcode.h + qrcode.c from github.com/ricmoo/QRCode
 *       into this sketch folder (src/ subdirectory, use the files directly)
 * @note Sounds: set BUZZER_PIN in sound.h to your GPIO (or leave -1 to skip)
 */

#include "Log.h"
#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "board_com_api.h"
#include "lvgl.h"
#include "lv_vendor.h"

#include "types.h"
#include "boards.h"
#include "components.h"
#include "sound.h"

#include "ui_splash.h"
#include "ui_home.h"
#include "ui_boards.h"
#include "ui_components.h"
#include "ui_calcs.h"
#include "ui_colourcode.h"
#include "ui_common.h"
#include "ui_tutorials.h"

void setup(void) {
    Serial.begin(115200);
    Log.begin(1024);

    PR_NOTICE("========================================");
    PR_NOTICE("Maker's Pocket Reference v8.0");
    PR_NOTICE("CMozMaker / Tinker Tailor");
    PR_NOTICE("tinkertailor.ca");
    PR_NOTICE("========================================");

    if (OPRT_OK != board_register_hardware()) {
        PR_ERR("board_register_hardware error!");
        while (1);
    }

    sound_init();

    static char display_name[] = "display";
    lv_vendor_init(display_name);

    lv_vendor_disp_lock();
    show_splash_screen();   // boots into retro terminal, auto-transitions to home
    lv_vendor_disp_unlock();

    lv_vendor_start(5, 1024 * 8);

    PR_NOTICE("App started!");
}

void loop() {
    delay(10);
}
