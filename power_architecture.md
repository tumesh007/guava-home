# Power Architecture

## Power Rails

```
230V AC Mains
    │
    └─── Relay LOAD side only
         (NO, COM terminals per channel)
         NEVER connected to breadboard or ESP32 side
         ───────────────────────────────────────────

USB 5V charger / PSU set to 5V (minimum 2A)
    │
    ├─── ESP32 VIN pin (or USB micro connector)
    ├─── HC-SR04 VCC (5V required)
    ├─── PIR HC-SR501 VCC (5V required)
    ├─── Rain sensor module VCC
    ├─── Relay module JD-VCC (remove JD-VCC jumper, power separately)
    ├─── LCD 16x2 VCC
    └─── Logic converter HV side

ESP32 onboard 3.3V regulator (max 300mA — do not overload)
    │
    ├─── RFID RC522 VCC (MUST be 3.3V)
    ├─── DHT11 VCC
    ├─── Logic converter LV side
    └─── 4.7kΩ pull-up for DHT11 data line

Common GND
    └─── All components share one GND rail
         ESP32 GND = PSU GND = relay GND = sensor GND
```

## Current Budget (approximate)

| Component | Current at 5V |
|-----------|--------------|
| ESP32 (WiFi active) | 240mA peak, 80mA average |
| HC-SR04 | 15mA |
| PIR HC-SR501 | 65mA |
| Relay module (4 relays active) | 70mA per relay = 280mA max |
| Rain sensor | 20mA |
| LCD with backlight | 25mA |
| **Total worst case** | **~725mA** |

**Recommended PSU: 5V 2A minimum.** A standard phone charger (2A) is sufficient for the ESP32 and sensors. Use a separate 5V supply for the relay module if possible.

## Relay Isolation

Remove the JD-VCC jumper on the relay board. This isolates:
- ESP32 side: 5V from USB charger, optocoupler inputs
- Relay coil side: separate 5V supply, coil and COM/NO/NC terminals

This prevents relay switching spikes from resetting the ESP32 or corrupting sensor readings.

## Capacitor Recommendations

Adding decoupling capacitors improves stability:

| Cap | Value | Location | Purpose |
|-----|-------|----------|---------|
| C1 | 100µF electrolytic | Across 5V rail | Bulk decoupling, handles relay inrush |
| C2 | 10µF ceramic | Across ESP32 3.3V output | High-frequency decoupling |
| C3 | 100nF ceramic | Across each sensor VCC/GND | Local decoupling |

## Flyback Diode

Add a 1N4007 diode across each relay coil (cathode to VCC, anode to GND side of coil). This suppresses voltage spikes when relay turns off, protecting optocouplers and the ESP32.

```
5V ─────┬──── Relay coil + ────┐
        │                      │ coil
   1N4007 (reversed)           │
   cathode to VCC              │
   anode to coil−  ────────────┘
        │
       GND
```
