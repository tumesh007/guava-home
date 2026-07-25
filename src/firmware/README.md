# Guava Home Firmware

Arduino C++ firmware for ESP32-based smart home controller.

## Files

- `GuavaHome_Latest.ino` - Current production firmware (v1.5.1)
- `GuavaHome_v1_4.ino` - Previous stable version (v1.4)
- `GuavaHome_v1_2.ino` - Earlier version (v1.2)
- `GuavaHome_v1_0.ino` - Original version (v1.0)

## Compilation

Use Arduino IDE with ESP32 board package installed.

See docs/SETUP_GUIDE.md for detailed instructions.

## Structure

Each .ino file is self-contained and includes:
- WiFi management
- Web server
- Telegram bot integration
- Relay control
- Sensor reading
- RFID interface

## Configuration

Edit these values in the firmware file:
- WiFi SSID and password
- Telegram bot token
- Device name
- Relay pin assignments
- Sensor types and pins
