# 🍋 Project Guava Home — Standalone ESP32 Smart Home Hub

## [V1.4.1 | Web-Configurable Standalone Automation]

Guava Home is an all-in-one smart home hub built on the ESP32 DevKit V1. It integrates dozens of components to provide security, climate, lighting, and garden automation, all configurable through a local Web UI and a remote Telegram Bot. No cloud dependency. No hardcoded variables.

Here is the complete Guava Home ecosystem at a glance:

![🍋 PROJECT GUAVA HOME Infographic](https://raw.githubusercontent.com/tumesh007/guava-home/refs/heads/main/1775408625584.png)

***

## 🛠️ Hardware & Sensors

Guava Home integrates the following physical hardware to sense and control its environment:

* **ESP32 DevKit V1 (38-pin):** The core microcontroller.
* **4-Channel Relay Module:** Controls AC power for the Fan, Light, Door Lock, and Water Pump. (Active LOW).
* **RC522 RFID Reader:** Controls authorized door access.
* **DHT11 Temp/Humidity Sensor:** Moniters climate.
* **MQ-2 Gas/Smoke Sensor:** Detects hazardous levels.
* **Ultrasonic Distance Sensor:** Triggers security and LCD wake alerts.
* **PIR Motion Sensor:** Automates lighting.
* **KY-037 Sound Sensor:** Provides acoustic light override (clap sensor).
* **LDR (Photoresistor):** Monitors daylight levels.
* **Soil Moisture Sensor:** Monitors plant water needs.
* **I2C 16x2 LCD Screen & Active Buzzer:** For local status and feedback.
* **Physical Buttons:** Exit Button and LCD Page Button.

***

## ⚙️ Core Configuration & Operation

By default, Guava Home is **100% web-configurable**. All operational parameters are stored in the ESP32's flash memory using the `Preferences` library and can be adjusted in real-time.

### 📶 Initial Setup & AP Mode

1.  **AP Safe Mode:** If no saved Wi-Fi SSID is found, Guava Home boots into **Failsafe AP Mode**.
2.  **Connect:** Look for the Wi-Fi network: **`Guava_Setup`**
3.  **Config Portal:** Open your browser and go to: **`http://192.168.4.1`**
4.  **Save Credentials:** Enter your home Wi-Fi SSID and Password. Enter your Telegram Bot Token, Chat ID, and desired OTA Password.
5.  **Reboot:** Click "Save Settings & Reboot". The system reboots and connects to your local network, displaying its new IP address on the LCD.

### 🎛️ Local Web Dashboard & Configuration

You have full control over all system behavior via the main configuration page:

| Configuration Pillar | Configurable Parameters (Web UI & Flash Memory) |
| :--- | :--- |
| **🔒 SECURITY** | **RFID slots (Users 1-3):** Save specific UIDs and associate names. <br> **Intrusion Alert Distance:** cm. <br> **Door Auto-Lock Delay (V1.4):** 1 to 60 seconds. <br> **Telegram Alerts checklist:** RFID, Intrusion, Gas. |
| **🌡️ CLIMATE** | **Dynamic Fan Auto-ON (V1.1):** Set °C threshold. <br> **Telegram Alerts checklist:** High Temperature. |
| **🌱 GARDEN** | **Pump cycles:** Set specific ON duration (sec) and OFF resting duration (sec). <br> **Soil Dry Alert
6. Enter your home WiFi credentials and Telegram Bot details in the **Config** tab.
7. Click **Save Settings & Reboot**. The ESP32 will connect to your home network and send an initialization message to your Telegram app.

## ⚙️ Hardware Wiring Notes
* **Voltage Dividers:** Both the `HC-SR04 ECHO` and `MQ-2 AOUT` output 5V logic. You **must** use a 1kΩ / 2kΩ resistor voltage divider to step these signals down to 3.3V before connecting them to the ESP32 GPIO pins to prevent hardware damage. Do not use standard I2C Logic Level Converters for these specific sensors.
* **Clap Sensor:** Connect the Digital Out (D0) pin to GPIO 39. Tune the potentiometer on the module until the trigger LED only flashes on a loud, sharp noise.
