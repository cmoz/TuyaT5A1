// sound.cpp
// Uses Arduino tone() with a passive piezo buzzer.
// If tone() is not available in the Tuya framework, replace the
// tone() calls with Tuya's tal_pwm API for the same GPIO pin.

#include "sound.h"
#include "Arduino.h"

static bool g_muted = false;

void sound_init() {
#if BUZZER_PIN >= 0
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
#endif
}

// Short, crisp tap sound (1.2kHz, 25ms)
void sound_play_click() {
#if BUZZER_PIN >= 0
    if (g_muted) return;
    tone(BUZZER_PIN, 1200, 25);
#endif
}

// Slightly lower navigation sound (900Hz, 40ms)
void sound_play_navigate() {
#if BUZZER_PIN >= 0
    if (g_muted) return;
    tone(BUZZER_PIN, 900, 40);
#endif
}

void sound_toggle_mute() {
    g_muted = !g_muted;
}

void sound_set_muted(bool muted) {
    g_muted = muted;
}

bool sound_is_muted() {
    return g_muted;
}
