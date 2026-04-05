# Logic Level Converter — Channel Allocation

## Why needed
ESP32 GPIO pins operate at **3.3V**. Several sensors/modules use **5V logic**.
Connecting a 5V signal directly to an ESP32 GPIO will damage the pin over time.
The 8-channel BSS138 bidirectional converter handles the voltage translation.

## Converter Wiring
```
LV pin  → 3.3V from ESP32
HV pin  → 5V rail
GND     → Common GND (connect both GND pins on converter)
LV1–LV8 → ESP32 GPIO pins (3.3V side)
HV1–HV8 → Sensor signal pins (5V side)
```

## Channel Allocation

| CH | LV (3.3V) — ESP32 | HV (5V) — Sensor | Direction | Sensor |
|----|-------------------|------------------|-----------|--------|
| 1  | GPIO 12           | HC-SR04 ECHO     | 5V → 3.3V | Ultrasonic echo |
| 2  | GPIO 13           | HC-SR04 TRIG     | 3.3V → 5V | Ultrasonic trigger |
| 3  | Spare             | —                | —         | Available |
| 4  | Spare             | —                | —         | Available |
| 5  | Spare             | —                | —         | Available |
| 6  | Spare             | —                | —         | Available |
| 7  | Spare             | —                | —         | Available |
| 8  | Spare             | —                | —         | Available |

## Sensors NOT needing converter

| Sensor | Output Voltage | Reason |
|--------|---------------|--------|
| DHT11 | 3.3V | 3.3V or 5V supply, output is 3.3V compatible |
| PIR HC-SR501 | 3.3V | Output spec is 3.3V |
| RFID RC522 | 3.3V | Runs entirely on 3.3V natively |
| LCD I2C backpack | 3.3V | PCF8574 accepts 3.3V I2C |
| Relay module inputs | 3.3V compatible | Optocoupler inputs accept 3.3V |
| LDR | Analog divider | Divider naturally limits to 3.3V |
| Soil moisture AOUT | Analog divider | Same |
| FSR | Analog divider | Same |
| Rain DOUT | 5V module | GPIO 36 is input-only, module has its own comparator — no converter needed as long as module VCC is 5V and output is read on input-only pin |
| Active buzzer | Drive from 3.3V | 3.3V sufficient for most active buzzers |
