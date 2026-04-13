# TuyaT5A1
Trying this board from Tuya. AI capable with T5 chip, screen, many features. It is very new at the time of writing and it has errors in the windows version of support, which I am trying to fix to be able to use the board.



/* ---------- NOTE ON AI USAGE --------------------------------------------------------------------------------------------- */

The T5AI board has real AI hardware capabilities:
- The board has 2 microphones + 1 speaker built in, and the T5 chip supports ASR (speech recognition), KWS (keyword wake-word spotting), and TTS (text-to-speech) at the hardware level. That's the "AI" in T5AI.
  
But there's a catch for Arduino IDE users:
The full AI voice agent capabilities — connecting to LLMs like ChatGPT, Gemini, Claude etc. — require a TuyaOpen license code (UUID + authkey) burned into or flashed onto the device. GitHub This is separate from normal Arduino development and uses the TuyaOpen C SDK build system (tos.py), not Arduino IDE.
The full AI features including ASR, KWS, TTS, and LLM integration are part of the TuyaOpen framework GitHub, which is a different development path from what I've been doing in Arduino. Different from the start of this project, which is an attempt to bring this board to Arduino makers.

Practically speaking, there are two realistic options:

1. Stay in Arduino — add a "Hey, ask me!" screen that takes a Wi-Fi connection and calls an LLM API (OpenAI/Claude/etc.) directly over HTTP. You control it completely, no Tuya account needed, and it fits the tutorial narrative perfectly.
2. Switch to TuyaOpen SDK — get the full voice wake-word + chatbot experience, but it requires a separate toolchain and a paid license code per device.

I'm going with option 1: it is simpler, no vendor lock-in, and I can demo calling any AI API.



/* ------- WORKFLOW I've had to do to get the code to run using Arduino IDE ------------------------------------------------ */

PART 1- What You Need

Tuya T5AI Board V1.0.2 with 3.5" ILI9488 LCD stack module
Windows 10/11 PC
Arduino IDE 2 (latest)
USB-C cable
CH343 USB-to-serial driver — this is NOT standard, you must install it manually

Download: https://www.wch-ic.com/downloads/CH343SER_ZIP.html
Without this, the board won't appear as a COM port at all


----   ----    ----    ----    ----    ----
PART 2 — Install the Board Package

Open Arduino IDE → File → Preferences
Paste this URL into "Additional boards manager URLs":

   https://github.com/tuya/arduino-tuyaopen/releases/download/global/package_tuya_open_index.json

Go to Boards Manager, search "Tuya Open", install latest (v1.2.2)
Select board: TUYA_T5AI_BOARD

⚠️ Bug #1 — Case sensitivity in boards.txt:
The installed boards.txt references vendor-T5 but the actual folder is vendor-t5 (lowercase). On Windows this doesn't matter, but the build system does care. If you get "vendor tools not found" errors, check:
C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\hardware\tuya_open\1.2.2\boards.txt

----   ----    ----    ----    ----    ----
PART 3 — The Windows Path Fixes (Critical)
This is where most people get stuck. The Tuya build system uses paths that break on Windows.
Bug #2 — The C:\flags\ problem:
The toolchain expects build flags at C:\flags\. You need to copy the flags folder:
Source: C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\tools\vendor-t5\0.0.8\flags\
Destination: C:\flags\
Bug #3 — Junction links needed:
The build system hardcodes C:\platform and C:\src. Create junction links (Windows symlinks):
cmdmklink /J C:\platform "C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\tools\vendor-t5\0.0.8\platform"
mklink /J C:\src "C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\tools\vendor-t5\0.0.8\src"
Run these as Administrator in Command Prompt.

----   ----    ----    ----    ----    ----
PART 4 — The Display Fix (Board Won't Show Anything)
Bug #4 — Wrong variant files:
The board package installs variant files to a TUYA_T5AI_BOARD folder, but the build system looks in a t5 folder. Fix:

Copy everything from:
C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\hardware\tuya_open\1.2.2\variants\TUYA_T5AI_BOARD\

Into:
C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\hardware\tuya_open\1.2.2\variants\t5\
Without this, board_register_hardware() silently fails and the screen stays blank.

----   ----    ----    ----    ----    ----
PART 5 — The appConfig.json Requirement
Bug #5 — Every sketch needs this file:
Every Arduino sketch folder MUST contain a file called exactly appConfig.json. Without it you get cryptic build errors. Minimum content:
json{
    "uuid": "your-uuid-here",
    "auth_key": "your-authkey-here"
}
Even for sketches that don't use WiFi, the file must exist (can have placeholder values).

----   ----    ----    ----    ----    ----
PART 6 — LVGL Limitations to Know
Only two fonts are available: lv_font_montserrat_14 and lv_font_montserrat_24. Using any other font will cause a linker error — you won't get a helpful message, it just won't compile.
lv_conf.h location (for enabling features like QR codes):
C:\Users\[you]\AppData\Local\Arduino15\packages\tuya_open\tools\vendor-t5\0.0.8\src\liblvgl\v9\conf\lv_conf.h

----   ----    ----    ----    ----    ----
PART 7 — The C++ Strictness Issues
The Tuya ARM toolchain is stricter than standard Arduino. Common issues:

snprintf needs #include <cstdio> — it's NOT auto-included
Forward declarations required — functions must be declared before use
No #pragma once in .cpp files — headers only
Static variables in .h files cause duplicate symbol errors — declarations go in .h, definitions go in .cpp

----   ----    ----    ----    ----    ----
PART 8 — Serial Port Setup
The T5AI board has two COM ports when connected:

Lower number = log output (115200 baud) — use this for Serial Monitor
Higher number = firmware flashing — Arduino IDE selects this automatically

If flashing fails, try the other port number.

----   ----    ----    ----    ----    ----
PART 9 — Adding AI (TuyaOS AI)
If you want to use the on-board AI / cloud LLM features:

Create a product at platform.tuya.com → AI Product → Cloud Access Mode: TuyaOS AI
Get 2 free licenses from the Hardware Development tab → "Get 2 Free Licenses" → License List
Download the credential file — contains your UUID and AuthKey
TuyaAI.Audio.begin() is mandatory even for text-only AI — without it the input pipeline never starts
Use AI_CHAT_MODE_FREE and save it with TuyaAI.saveModeConfig() — the board saves WAKEUP mode by default
Publish EVENT_MQTT_CONNECTED in your IoT event callback or the AI stack won't initialize

