/************************************************************
 * Project: Tuya T5A1 – Display Colour Test
 * File: displayColourTest.ino
 * Author: CMoz (YouTube @cmozmaker)
 * Description:
 *    Simple diagnostic sketch that initializes the Tuya
 *    display hardware and fills the screen with a new
 *    random colour every 2 seconds. Useful for verifying
 *    display wiring, brightness, and rendering behaviour.
 *
 * Created by: CMoz
 * GitHub: https://github.com/cmoz
 ************************************************************/

#include "Display.h"   // Display driver abstraction
#include "Log.h"       // Logging utilities for debug output

// Create a global Display object
Display display;

void setup() {
    Serial.begin(115200);   // Start serial output for debugging
    Log.begin();            // Initialize Tuya logging system

    // Register board hardware (required for Tuya platform)
    if (OPRT_OK != board_register_hardware()) {
        PR_ERR("board_register_hardware error\n");
        while (1);          // Halt execution if hardware fails
    }

    // Initialize the display hardware
    if (OPRT_OK != display.begin()) {
        PR_ERR("Failed to initialize display");
        return;             // Stop setup if display fails
    }

    // Report detected display resolution
    PR_NOTICE("Display resolution: %dx%d",
              display.getWidth(),
              display.getHeight());

    display.setBrightness(100);   // Set display brightness to max
    display.clear(0x000000);      // Clear screen to black
}

void loop() {
    // Generate a random 24‑bit RGB colour
    uint32_t color = random(0xFFFFFF);

    // Log the colour being used
    PR_DEBUG("Filling screen with color: 0x%06X", color);

    // Fill the entire screen with the random colour
    display.fillScreen(color);

    delay(2000);   // Wait 2 seconds before choosing a new colour
}
