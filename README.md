<div align="center">

# 🍈 Guava Home
**ESP32 Smart Home Controller & Automation Hub**

[![ESP32](https://img.shields.io/badge/Board-ESP32_DevKit_V1-blue.svg)](https://www.espressif.com/)
[![C++](https://img.shields.io/badge/Language-C++-00599C.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/Version-1.5.1-orange.svg)]()

Guava Home is a complete, standalone IoT smart home controller built for the ESP32. It features a Progressive Web App (PWA) dashboard, Telegram bot integration, hardware sensors, RFID door access, and Over-The-Air (OTA) firmware updates. No cloud subscription required.

</div>

---

## 🚀 What's New in V1.5.1

The latest update introduces **Hotel Door Logic** and a highly resilient **Acoustic Filtering System** to handle severe weather.

* **Double-Clap Storm Filter:** The KY-037 sound sensor now features a state machine requiring two sharp peaks (200–700ms apart) to toggle lights. The 150ms hardware debounce actively strips out thunder rumbles, and relay "snaps" are automatically muted to prevent acoustic feedback loops.
* **Hotel-Style Smart Lock:** Uses an IR obstacle sensor (GPIO 17) to detect the physical door frame. If the door is unlocked and opened, the 3-second auto-relock timer is physically *frozen* until the door swings shut, preventing the deadbolt from firing while the door is open. 
* **Dynamic Web Config:** You can now configure the Fan Auto-ON Temperature, Acoustic Modes (Single/Double/Muted), and IR Sensor polarity directly from the web dashboard. No reflashing required.

---

## ✨ Core Features

* 📱 **Web Dashboard:** A responsive, dark-mode PWA accessible via local IP.
* 💬 **Telegram Bot:** Control relays, get environmental statuses, and receive instant intrusion/gas alerts from anywhere in the world.
* 🚪 **RFID Access Control:** Add up to 3 authorized UID cards via the web interface to trigger the door lock relay.
* 💡 **Smart Lighting:** PIR motion sensing with daylight awareness (LDR) and configurable auto-off delays.
* 🌡️ **Climate Control:** DHT11 sensor automatically triggers the Fan relay if the room exceeds a configured threshold.
* 🌱 **Automated Garden Pump:** Configurable ON/OFF cycle timers and soil moisture alerts.
* 🚨 **Safety Alerts:** MQ-2 Gas/Smoke detection and HC-SR04 Ultrasonic intrusion detection.
* ⚡ **Fail-Safe AP Mode:** If WiFi is lost, the ESP32 broadcasts a "Guava_Setup" hotspot to allow reconfiguration.

---

## 🔌 Pin Mapping & Hardware Wiring

| Component | ESP32 Pin | Logic / Notes |
| :--- | :--- | :--- |
| **DHT11 (Temp/Hum)** | `GPIO 4` | Needs 4.7kΩ pull-up to 3.3V |
| **RFID (RC522) SPI** | `GPIO 5, 18, 19, 23` | RST pin is hardwired to 3.3V (frees GPIO 17) |
| **HC-SR04 (Ultrasonic)** | `Trig: 12, Echo: 13` | ECHO requires 5V to 3.3V voltage divider |
| **Active Buzzer** | `GPIO 14` | Active HIGH |
| **Relay 1 (Fan)** | `GPIO 15` | Active LOW |
| **Relay 2 (Light)** | `GPIO 16` | Active LOW |
| **Relay 3 (Door Lock)**| `GPIO 25` | Active LOW |
| **Relay 4 (Pump)** | `GPIO 33` | Active LOW |
| **IR Door Sensor** | `GPIO 17` | Input Pull-up. Reversible logic via Web UI. |
| **I2C LCD (16x2)** | `SDA: 21, SCL: 22` | Displays system status and pages |
| **Exit Button** | `GPIO 26` | Input Pull-up (LOW = pressed) |
| **PIR (HC-SR501)** | `GPIO 27` | Tx pot fully counter-clockwise |
| **Soil Moisture** | `GPIO 32` | ADC1 (Dry > 3000, Wet < 1000) |
| **LDR (Light)** | `GPIO 34` | ADC1 |
| **LCD Page Button** | `GPIO 35` | 10kΩ pull-DOWN (HIGH = pressed) |
| **MQ-2 Gas Sensor** | `GPIO 36` | ADC1 via voltage divider |
| **KY-037 Clap Sensor** | `GPIO 39` | Hardware Interrupt (FALLING edge) |
| **Inbuilt LED** | `GPIO 2` | Mirrors PIR motion state |

---

## 🛠️ Setup & Installation

### 1. Initial Flash
1. Open `GuavaHome_V1_5_1.ino` in the Arduino IDE.
2. Install all required libraries (Adafruit DHT, LiquidCrystal_I2C, MFRC522, UniversalTelegramBot, ArduinoJson v6.x).
3. Upload the code to your ESP32 via USB. 

### 2. First Boot & Network Config
1. Upon first boot, the LCD will say "Setup Mode". Connect your phone/PC to the WiFi network: **`Guava_Setup`** (No password).
2. Open a browser and navigate to `http://192.168.4.1`.
3. Go to the **Config** tab and enter your home WiFi SSID, WiFi Password, Telegram Bot Token, and Chat ID. 
4. Click **Save Settings & Reboot**. The ESP32 will connect to your home network and display its new local IP on the LCD.

---

## 💬 Telegram Commands

Interact with your Guava Home bot from anywhere. If polling is enabled, the ESP checks for commands every 8 seconds.

* `/status` - Returns a full readout of all sensors, relays, and modes.
* `/unlock` - Fires the deadbolt relay for 3 seconds.
* `/light_on` / `/light_off` - Manual override for room lighting.
* `/fan_on` / `/fan_off` - Manual override for fan.
* `/pump_auto` / `/pump_stop` - Controls the automated watering loop.
* `/clap_single` / `/clap_double` - Switch between legacy single-clap or storm-safe double-clap modes.
* `/clap_mute` / `/clap_unmute` - Completely disable the microphone during heavy thunderstorms.
* `/mute_on` / `/mute_off` - Silences the physical hardware buzzer.
* `/reboot` - Safely restarts the ESP32.

---

## 🔄 OTA (Over-The-Air) Updates

Guava Home supports wireless firmware flashing. Once the system is on your WiFi network, you never need to plug it into your computer via USB again.

**Method 1: Browser Upload**
1. In Arduino IDE, click `Sketch` > `Export Compiled Binary`.
2. Go to `http://<ESP_IP>/update` in your web browser.
3. Upload the `.bin` file.

**Method 2: Arduino IDE Network Port**
1. In Arduino IDE, go to `Tools` > `Port` > `Network Ports` and select `GuavaHome`.
2. Click Upload. When prompted, the default password is `guavahome` (can be changed in the Web UI).

---

## 🧩 Advanced Configuration Details

* **IR Sensor Logic Inversion:** Some IR obstacle modules output `HIGH` when an object is detected, and others output `LOW`. If your "Door Status" card says "OPEN" when the door is closed, simply check the "IR sensor inverted" box in the Web Config tab.
* **Adding RFID Cards:** Scan a new tag. The UID will appear in the Web Config tab under "Last Scanned UID". Copy that UID, paste it into User Slot 1, 2, or 3, give it a name, and hit Save.

---
*Developed for ESP32. Licensed under MIT.*
