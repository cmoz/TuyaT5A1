#include "Display.h"
#include "Log.h"

Display display;

void setup() {
    Serial.begin(115200);
    Log.begin();

    if (OPRT_OK != board_register_hardware()) {
        PR_ERR("board_register_hardware error\n");
        while(1);
    }

    if (OPRT_OK != display.begin()) {
        PR_ERR("Failed to initialize display");
        return;
    }

    PR_NOTICE("Display resolution: %dx%d", display.getWidth(), display.getHeight());
    display.setBrightness(100);
    display.clear(0x000000);
}

void loop() {
    uint32_t color = random(0xFFFFFF);
    PR_DEBUG("Filling screen with color: 0x%06X", color);
    display.fillScreen(color);
    delay(2000);
}
