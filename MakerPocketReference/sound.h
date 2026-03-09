#pragma once

// ============================================================
// SOUND / BUZZER
// Connect a passive piezo buzzer between BUZZER_PIN and GND.
// Set BUZZER_PIN to -1 if you don't have a buzzer — the mute
// button will still appear but sounds will simply be disabled.
// ============================================================

#define BUZZER_PIN  (-1)   // <-- change to your GPIO pin, e.g. 25

void sound_init();
void sound_play_click();       // short UI tap beep
void sound_play_navigate();    // slightly lower tone for screen changes
void sound_toggle_mute();
void sound_set_muted(bool muted);
bool sound_is_muted();
