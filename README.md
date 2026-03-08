# TuyaT5A1
Trying this board from Tuya. AI capable with T5 chip, screen, many features. It is very new at the time of writing and it has errors in the windows version of support, which I am trying to fix to be able to use the board.

/* ------- WORKFLOW I've had to do to get the code to run using Arduino IDE ------------------------------------------------ */

[to follow]



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

