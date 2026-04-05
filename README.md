# 🍋 Guava Home 

Guava Home is a fully featured, standalone Smart Home Controller built on the ESP32. It combines physical automation (relays, sensors, RFID) with remote control via a Telegram Bot and a local Web Dashboard (PWA).

All settings—including WiFi credentials, Telegram API keys, authorized RFID tags, and sensor thresholds—are saved dynamically to the ESP32's flash memory via the Web UI. No hardcoding or re-flashing is required after the initial setup.

## ✨ Features
* **Dual-Interface Control:** Operate devices locally via the Web Dashboard or remotely from anywhere via Telegram.
* **Smart Lighting Automation:** Daylight-aware PIR motion sensing, coupled with an acoustic hardware interrupt (clap sensor) for manual overrides. 
* **Dynamic Access Control:** RC522 RFID reader for door lock control. Scan unknown cards to the Web UI to easily add them to the 3 authorized user slots.
* **Garden Management:** Automated water pump cycling with tunable ON/OFF durations and soil moisture threshold alerts.
* **Safety Monitoring:** Real-time analog MQ-2 gas/smoke monitoring and intrusion detection via ultrasonic distance sensing.
* **Over-The-Air (OTA) Updates:** Flash new firmware wirelessly through the Web UI or Arduino IDE.
* **AP Rescue Mode:** Automatically broadcasts a fallback Setup Network (`192.168.4.1`) if the primary WiFi drops.

## 🛠 Hardware Required
* **Microcontroller:** ESP32 DevKit V1 (38-pin)
* **Sensors:** * DHT11 (Temperature & Humidity)
  * HC-SR04 (Ultrasonic Distance)
  * HC-SR501 (PIR Motion)
  * MQ-2 (Analog Gas/Smoke)
  * KY-037 (Sound/Clap Sensor)
  * Standard LDR & Analog Soil Moisture Probe
* **Modules:**
  * MFRC522 RFID Reader
  * 4-Channel 5V Relay Module (Active Low)
  * 16x2 I2C LCD Display
  * Active Buzzer

## ⚡ Installation & First Boot
1. Open `guava_home.ino` in the Arduino IDE.
2. Install the required libraries (listed in the code header).
3. Flash the code to your ESP32 via USB.
4. On your phone or PC, connect to the new WiFi network: **Guava_Setup**.
5. Open a browser and navigate to `http://192.168.4.1`.
6. Enter your home WiFi credentials and Telegram Bot details in the **Config** tab.
7. Click **Save Settings & Reboot**. The ESP32 will connect to your home network and send an initialization message to your Telegram app.

## ⚙️ Hardware Wiring Notes
* **Voltage Dividers:** Both the `HC-SR04 ECHO` and `MQ-2 AOUT` output 5V logic. You **must** use a 1kΩ / 2kΩ resistor voltage divider to step these signals down to 3.3V before connecting them to the ESP32 GPIO pins to prevent hardware damage. Do not use standard I2C Logic Level Converters for these specific sensors.
* **Clap Sensor:** Connect the Digital Out (D0) pin to GPIO 39. Tune the potentiometer on the module until the trigger LED only flashes on a loud, sharp noise.
