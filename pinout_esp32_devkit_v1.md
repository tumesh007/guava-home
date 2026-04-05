# ESP32 DevKit V1 — Pin Reference

## Pin Usage in This Project

```
                    ┌──────────────┐
              EN ───┤              ├─── IO23  MOSI → RFID
           IO36 ─── Rain          ├─── IO22  SCL  → LCD
           IO39 ─── FSR           ├─── IO21  SDA  → LCD
           IO34 ─── LDR           ├─── IO19  MISO ← RFID
           IO35 ─── BTN2          ├─── IO18  SCK  → RFID
           IO32 ─── SOIL          ├─── IO5   SS   → RFID
           IO33 ─── RELAY4/Pump   ├─── IO17  RST  → RFID
           IO25 ─── RELAY3/Door   ├─── IO16  RELAY2/Light
           IO26 ─── BTN1/Exit     ├─── IO4   DHT11
           IO27 ─── PIR           ├─── IO2   Inbuilt LED
           IO14 ─── BUZZER        ├─── IO15  RELAY1/Fan
           IO12 ─── ECHO          ├─── IO13  TRIG
             GND ───              ├─── GND
             VIN ───┤              ├─── 3V3
                    └──────────────┘
```

## Pin Properties

| GPIO | ADC | Touch | PWM | Notes |
|------|-----|-------|-----|-------|
| 0 | ADC2 | T1 | Yes | Boot pin — avoid for output |
| 2 | ADC2 | T2 | Yes | Inbuilt LED |
| 4 | ADC2 | T0 | Yes | DHT11 |
| 5 | — | — | Yes | RFID SS / SPI CS |
| 12 | ADC2 | T5 | Yes | RFID boot pin — use carefully |
| 13 | ADC2 | T4 | Yes | HC-SR04 TRIG |
| 14 | ADC2 | T6 | Yes | Buzzer |
| 15 | ADC2 | T3 | Yes | RELAY1 Fan |
| 16 | — | — | Yes | RELAY2 Light |
| 17 | — | — | Yes | RFID RST |
| 18 | — | — | Yes | SPI SCK |
| 19 | — | — | Yes | SPI MISO |
| 21 | — | — | Yes | I2C SDA |
| 22 | — | — | Yes | I2C SCL |
| 23 | — | — | Yes | SPI MOSI |
| 25 | DAC1 | — | Yes | RELAY3 Door |
| 26 | DAC2 | — | Yes | BTN1 Exit |
| 27 | ADC2 | T7 | Yes | PIR |
| 32 | ADC1 | T9 | Yes | Soil Moisture |
| 33 | ADC1 | T8 | Yes | RELAY4 Pump |
| 34 | ADC1 | — | — | LDR — INPUT ONLY |
| 35 | ADC1 | — | — | BTN2 — INPUT ONLY, no pull-up |
| 36 | ADC1 | — | — | Rain — INPUT ONLY, no pull-up |
| 39 | ADC1 | — | — | FSR — INPUT ONLY, no pull-up |

## Reserved Pins (NEVER USE)

| GPIO | Reserved For |
|------|-------------|
| 1 | UART0 TX (Serial) |
| 3 | UART0 RX (Serial) |
| 6–11 | Internal flash memory |

## ADC Notes

- **ADC2** is disabled when WiFi is active. All analog sensors use **ADC1** (GPIOs 32–39).
- ADC resolution: 12-bit (0–4095)
- ADC reference: 3.3V (so 4095 = 3.3V)
- ADC is non-linear near 0V and 3.3V — calibrate for accurate readings

## Available Pins (unused in this project)

GPIOs not used: 0, 3 (avoid), 10, 11 (avoid — flash), others via I2C expansion
