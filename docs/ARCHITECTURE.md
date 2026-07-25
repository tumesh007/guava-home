# Guava Home Architecture

## System Overview

Guava Home is an ESP32-based smart home controller featuring:
- Web UI for local control
- Telegram bot integration for remote access
- RFID access control
- Multiple relay outputs for device control
- Real-time sensor monitoring

## Components

### Hardware
- **Controller**: ESP32 DevKit V1
- **Communication**: WiFi, UART (Telegram/RFID)
- **Outputs**: 4x Relay modules
- **Sensors**: Temperature, Humidity, RFID

### Software Architecture
- **Firmware**: Arduino C++ based
- **Web UI**: HTML/CSS/JavaScript (hosted on ESP32)
- **Integration**: Telegram Bot API

## Power Architecture

See power_architecture.md for detailed power distribution design.

## Wiring & Connections

See wiring_diagram.svg for complete connection diagram.
