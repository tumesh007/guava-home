# Guava Home - ESP32 Smart Home Controller

![License](https://img.shields.io/badge/license-MIT-blue)
![Status](https://img.shields.io/badge/status-Active-green)
![Platform](https://img.shields.io/badge/platform-ESP32-orange)

## Overview

Guava Home is a feature-rich, ESP32-based smart home controller with:
- 🌐 Web UI for local control
- 📱 Telegram bot integration for remote access
- 🔐 RFID access control system
- 🔌 4-relay output control
- 📊 Real-time sensor monitoring (temperature, humidity)
- ⚡ Low-power, reliable operation

## Quick Start

### Hardware Requirements
- ESP32 DevKit V1
- 4x Relay modules
- RFID reader module (RC522)
- Temperature/humidity sensor (DHT22)
- USB cable and 5V power supply

### Software Setup

1. **Install Arduino IDE** from https://www.arduino.cc/en/software

2. **Add ESP32 Board Support**
   - Preferences → Additional Boards Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board Manager → Install "esp32"

3. **Configure & Upload**
   - Open `src/firmware/GuavaHome_V1_5_1.ino`
   - Edit WiFi/Telegram credentials in the config section
   - Select Board: ESP32 Dev Module
   - Upload firmware

4. **Access Controls**
   - Web UI: `http://guava-home.local` or device IP
   - Telegram: Send commands to your bot

## Firmware Versions

| Version | File | Notes |
|---------|------|-------|
| V1.0 | `src/firmware/GuavaHome_V1_0.ino` | Initial release |
| V1.2 | `src/firmware/GuavaHome_V1_2.ino` | Added Telegram support |
| V1.4 | `src/firmware/GuavaHome_V1_4.ino` | RFID + web UI |
| V1.4.1 | `src/firmware/GuavaHome_V1_4_1_notes.txt` | Patch notes |
| **V1.5.1 (Latest)** | `src/firmware/GuavaHome_V1_5_1.ino` | Stable release |

## Documentation

- [Setup Guide](docs/SETUP_GUIDE.md) - Detailed installation instructions
- [First Boot](docs/FIRST_BOOT.md) - Initial configuration
- [Architecture](docs/ARCHITECTURE.md) - System design overview
- [Telegram Commands](docs/TELEGRAM_COMMANDS.md) - Bot command reference
- [Pinout Reference](docs/PINOUT.md) - GPIO assignments
- [Changelog](docs/CHANGELOG.md) - Version history

## Directory Structure

```
guava-home/
├── src/firmware/              # Arduino firmware code
│   ├── GuavaHome_V1_0.ino
│   ├── GuavaHome_V1_2.ino
│   ├── GuavaHome_V1_4.ino
│   ├── GuavaHome_V1_4_1_notes.txt
│   └── GuavaHome_V1_5_1.ino   # Latest stable
├── docs/                      # Documentation & assets
│   ├── assets/
│   │   └── screenshot.png
│   ├── releases/
│   └── *.md                   # Markdown docs
├── README.md                  # This file
├── LICENSE                    # MIT License
├── .gitignore                 # Git ignore rules
└── wiring_diagram.svg         # Circuit diagram
```

## Features

### Local Control
- Clean, responsive web interface
- Real-time status display
- One-click device control
- Configuration panel

### Telegram Integration
- Remote device control
- Status notifications
- Command-based operation
- Alert system

### Security
- RFID access control (RC522)
- WiFi encryption (WPA2/WPA3)

## License

MIT License - see [LICENSE](LICENSE) for details.
