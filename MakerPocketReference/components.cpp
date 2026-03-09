#include "types.h"
#include "colours.h"

int selected_component = 0;

static ComponentInfo comp_ws2812b = {
    "WS2812B LED",
    "Individually addressable RGB LED",
    "5V",
    "DIN",
    "Use a large capacitor on the supply",
    { "5V → 5V", "GND → GND", "DIN → GPIO18", "", "", "" },
    { "Use level shifting", "Keep wires short", "", "" }
};

static ComponentInfo comp_dht22 = {
    "DHT22 Sensor",
    "Temperature + humidity sensor",
    "3.3V / 5V",
    "DATA",
    "Use a 4.7k pull-up resistor",
    { "VCC → 3.3V", "GND → GND", "DATA → GPIO4", "", "", "" },
    { "Keep wires short", "Avoid long polling intervals", "", "" }
};

static ComponentInfo comp_hcsr04 = {
    "HC-SR04 Ultrasonic",
    "Distance measurement sensor",
    "5V",
    "TRIG / ECHO",
    "Use a voltage divider on ECHO",
    { "VCC → 5V", "GND → GND", "TRIG → GPIO5", "ECHO → GPIO18", "", "" },
    { "Avoid noisy power", "Mount securely", "", "" }
};

/* ---------------- NEW COMPONENTS BELOW ---------------- */

static ComponentInfo comp_mfrc522 = {
    "MFRC522 RFID",
    "13.56MHz RFID reader",
    "3.3V",
    "SPI",
    "Do NOT use 5V on VCC",
    { "VCC → 3.3V", "GND → GND", "SDA → GPIO21", "SCK → GPIO18", "MOSI → GPIO23", "MISO → GPIO19" },
    { "Keep tags close", "Avoid metal surfaces", "", "" }
};

static ComponentInfo comp_bme280 = {
    "BME280 Sensor",
    "Temp + humidity + pressure",
    "3.3V",
    "I2C",
    "Check I2C address (0x76/0x77)",
    { "VCC → 3.3V", "GND → GND", "SCL → GPIO22", "SDA → GPIO21", "", "" },
    { "Avoid condensation", "Use short wires", "", "" }
};

static ComponentInfo comp_ds18b20 = {
    "DS18B20 Probe",
    "Waterproof temperature sensor",
    "3.3V / 5V",
    "1-Wire",
    "Use 4.7k pull-up on DATA",
    { "VCC → 3.3V", "GND → GND", "DATA → GPIO4", "", "", "" },
    { "Great for liquids", "Cable can be extended", "", "" }
};

static ComponentInfo comp_mpu6050 = {
    "MPU6050 IMU",
    "Gyro + accelerometer",
    "3.3V / 5V",
    "I2C",
    "Check orientation when mounting",
    { "VCC → 3.3V", "GND → GND", "SCL → GPIO22", "SDA → GPIO21", "", "" },
    { "Avoid vibration", "Calibrate offsets", "", "" }
};

static ComponentInfo comp_rc522keyfob = {
    "RFID Key Fob",
    "13.56MHz RFID tag",
    "Passive",
    "N/A",
    "Works with MFRC522",
    { "", "", "", "", "", "" },
    { "Hold flat to reader", "", "", "" }
};

static ComponentInfo comp_oled1306 = {
    "SSD1306 OLED 128x64",
    "Monochrome I2C display",
    "3.3V / 5V",
    "I2C",
    "Check I2C address (0x3C/0x3D)",
    { "VCC → 3.3V", "GND → GND", "SCL → GPIO22", "SDA → GPIO21", "", "" },
    { "Use short wires", "Avoid bending flex cable", "", "" }
};

static ComponentInfo comp_rotary = {
    "Rotary Encoder",
    "Incremental encoder with push button",
    "3.3V / 5V",
    "CLK / DT / SW",
    "Use pull-ups if needed",
    { "VCC → 3.3V", "GND → GND", "CLK → GPIO32", "DT → GPIO33", "SW → GPIO25", "" },
    { "Debounce in software", "Avoid long wires", "", "" }
};

static ComponentInfo comp_relay = {
    "5V Relay Module",
    "Isolated switching module",
    "5V",
    "IN",
    "Do NOT drive directly from GPIO",
    { "VCC → 5V", "GND → GND", "IN → GPIO26", "", "", "" },
    { "Use external power if needed", "Beware of inductive loads", "", "" }
};

/* ------------------------------------------------------ */

ComponentInfo* components[] = {
    &comp_ws2812b,
    &comp_dht22,
    &comp_hcsr04,
    &comp_mfrc522,
    &comp_bme280,
    &comp_ds18b20,
    &comp_mpu6050,
    &comp_rc522keyfob,
    &comp_oled1306,
    &comp_rotary,
    &comp_relay,
    // etc...
};

int component_count = sizeof(components) / sizeof(components[0]);
