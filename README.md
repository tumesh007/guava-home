# 🍈 Guava Home — ESP32 Smart Home Controller

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino IDE](https://img.shields.io/badge/IDE-Arduino-teal.svg)](https://www.arduino.cc/)
[![Version](https://img.shields.io/badge/Version-V1.0-green.svg)](firmware/GuavaHome_V1_0/)

A fully-featured WiFi-connected smart home system built on the ESP32 DevKit V1. Controls fans, lights, a door lock and water pump. Monitors temperature, humidity, motion, soil moisture, gas/smoke and distance. Every setting is configurable from a web browser — no re-flashing needed after initial setup.

---

## ✨ Features

| Feature | Details |
|---------|---------|
| **Sensors** | DHT11, PIR, HC-SR04, LDR, Soil, MQ-2 Gas, KY-037 Sound |
| **Outputs** | 4 relays (fan, light, door, pump), buzzer, LCD 16×2 |
| **RFID Access** | RC522 — tap card to unlock door, 3 user slots |
| **Clap Control** | Hardware interrupt — clap to toggle lights instantly |
| **Telegram Bot** | Remote control + auto-status alerts from anywhere |
| **Web Dashboard** | PWA — live sensors, relay toggles, full config |
| **OTA Updates** | Flash new firmware over WiFi after first USB flash |
| **AP Rescue Mode** | Creates its own WiFi if home network unreachable |
| **Pump Auto-Cycle** | Configurable ON/OFF timer for garden watering |
| **All configurable** | Timing, thresholds, RFID users — all via web UI |

---

## 📁 Repository Structure

```
guava-home/
├── firmware/
│   └── GuavaHome_V1_0/
│       └── GuavaHome_V1_0.ino
├── schematics/
│   ├── wiring_diagram.svg
│   ├── logic_converter_map.md
│   └── power_architecture.md
├── docs/
│   ├── FIRST_BOOT.md
│   ├── TELEGRAM_COMMANDS.md
│   ├── WIRING.md
│   ├── USER_GUIDE.md
│   └── TROUBLESHOOTING.md
├── assets/
│   └── pinout_esp32_devkit_v1.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

---

## 🔧 Hardware

### Required
- **ESP32 DevKit V1** (38-pin)
- 4-channel relay module (active LOW)
- DHT11 temperature & humidity sensor
- PIR HC-SR501 motion sensor
- HC-SR04 ultrasonic distance sensor
- MQ-2 gas/smoke sensor
- KY-037 sound/clap sensor
- RFID RC522 + cards
- LCD 16×2 with I2C backpack (PCF8574)
- Active buzzer
- LDR + 10kΩ resistor
- Soil moisture sensor
- 8-channel BSS138 logic level converter (or 1kΩ/2kΩ resistors for ECHO/MQ-2)
- 2× tactile buttons

### Pin Map

| GPIO | Component | Notes |
|------|-----------|-------|
| 2 | Inbuilt LED | Mirrors PIR state |
| 4 | DHT11 DATA | 4.7kΩ pull-up to 3.3V |
| 5 | RFID SS | SPI |
| 12 | HC-SR04 TRIG | Output, idles LOW |
| 13 | HC-SR04 ECHO | Via 1kΩ/2kΩ divider |
| 14 | Buzzer | Active buzzer |
| 15 | Relay 1 — Fan | Active LOW |
| 16 | Relay 2 — Light | Active LOW |
| 17 | RFID RST | SPI |
| 18 | RFID SCK | SPI |
| 19 | RFID MISO | SPI |
| 21 | LCD SDA | I2C |
| 22 | LCD SCL | I2C |
| 23 | RFID MOSI | SPI |
| 25 | Relay 3 — Door | Active LOW |
| 26 | Exit Button | INPUT_PULLUP |
| 27 | PIR | 3.3V output |
| 32 | Soil moisture | ADC1 |
| 33 | Relay 4 — Pump | Active LOW |
| 34 | LDR | ADC1, input-only |
| 35 | LCD Page Button | Pull-DOWN, HIGH=pressed |
| 36 | MQ-2 AOUT | ADC1, via 1kΩ/2kΩ divider |
| 39 | KY-037 DO | ISR, FALLING edge |

---

## 🚀 Quick Start

### 1. Install Arduino IDE & ESP32 Support

Add to **File → Preferences → Additional Boards Manager URLs**:
```
https://dl.espressif.com/dl/package_esp32_index.json
```
Then install **esp32 by Espressif Systems** via Boards Manager.

### 2. Install Libraries

| Library | Author | Note |
|---------|--------|------|
| DHT sensor library | Adafruit | |
| Adafruit Unified Sensor | Adafruit | dependency |
| LiquidCrystal_I2C | Frank de Brabander | |
| UniversalTelegramBot | Brian Lough | |
| ArduinoJson | Benoit Blanchon | **v6.x only** |
| MFRC522 | GithubCommunity | |

`ArduinoOTA` is built into the ESP32 Arduino core — no install needed.

### 3. Upload Settings

| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Partition | **Default 4MB with spiffs** |
| CPU Frequency | 240MHz |
| Upload Speed | 115200 |

### 4. First Boot

See **[First Boot Guide →](docs/FIRST_BOOT.md)**

1. Upload via USB
2. ESP32 creates WiFi AP: `Guava_Setup`
3. Connect phone → open `http://192.168.4.1`
4. Config tab → enter WiFi, Telegram token, Chat ID → Save & Reboot
5. IP shown on LCD and sent via Telegram

### 5. Subsequent Updates (OTA)

After first USB flash, use WiFi:
- Arduino IDE → Tools → Port → Network → `GuavaHome`
- Enter OTA password (default: `guavahome`, change in Config tab)
- Upload normally

---

## ⚙️ Configurable Settings (Web UI)

All of these are saved to flash and survive reboots:

| Setting | Default | Description |
|---------|---------|-------------|
| WiFi SSID / Password | — | Home network credentials |
| Telegram Bot Token | — | From @BotFather |
| Telegram Chat ID | — | Your personal ID |
| OTA Password | guavahome | For wireless firmware updates |
| Telegram Auto-Status | 0 (off) | Send status every N minutes |
| Telegram Poll Interval | 8s | How often to check for commands |
| Light Auto-off Delay | 30s | Delay after motion stops |
| Pump ON Duration | 10s | Watering on-time per cycle |
| Pump OFF Duration | 50s | Rest period between cycles |
| Soil Dry Threshold | 3000 | ADC above = dry alert (lower for wetter plants) |
| RFID Users 1-3 | — | UID + name for door access |

---

## 📱 Telegram Commands

| Command | Action |
|---------|--------|
| `status` | Full sensor snapshot |
| `fan_on` / `fan_off` | Fan relay |
| `light_on` / `light_off` | Light relay |
| `unlock` | Door unlock (3s) |
| `pump_on` / `pump_off` | Pump manual |
| `pump_auto` / `pump_stop` | Pump auto-cycle |
| `mute_on` / `mute_off` | Buzzer |
| `next_page` | Cycle LCD page |
| `help` | Command list |
| `reboot` | Restart ESP32 |

---

## 🔔 Automatic Alerts

| Trigger | Alert |
|---------|-------|
| PIR motion detected | Telegram message |
| Temperature > 32°C | Telegram + fan auto-ON |
| Soil too dry | Telegram + 2 beeps |
| Gas/smoke (MQ-2 > 2000) | Telegram + 4 beeps |
| Intrusion (< 30cm) | Telegram + 3 beeps |
| RFID — known card | Telegram + door unlock |
| RFID — unknown card | Telegram + 3 beeps |
| Auto-status | Configurable interval (0 = off) |

---

## 🛡️ Safety Design

- **Boot glitch prevention** — `digitalWrite()` before `pinMode()` in setup
- **PIR warm-up** — 60-second ignore period (HC-SR501 stabilisation)
- **OTA suspension** — all relay automation halts during firmware flash
- **AP mode timeout** — rescue network self-destructs after 10 minutes
- **Acoustic debounce** — 1500ms lockout prevents relay-snap echo on clap
- **Clap override** — clap disables PIR auto-off timer
- **Pump rest period** — starts in OFF phase, preventing immediate activation on boot

---

## 📋 Changelog

See [CHANGELOG.md](CHANGELOG.md)

---

## 📄 License

MIT — see [LICENSE](LICENSE)
