# Guava Home - ESP32 Smart Home Controller

![License](https://img.shields.io/badge/license-MIT-blue)
![Status](https://img.shields.io/badge/status-Active-green)

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
- RFID reader module
- Temperature/humidity sensor
- USB cable and 5V power supply

### Software Setup

1. **Install Arduino IDE** from https://www.arduino.cc/en/software

2. **Add ESP32 Board Support**
   - Preferences → Additional Boards Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board Manager → Install "esp32"

3. **Configure & Upload**
   - Edit WiFi/Telegram credentials in `src/firmware/GuavaHome_Latest.ino`
   - Select Board: ESP32 Dev Module
   - Upload firmware

4. **Access Controls**
   - Web UI: `http://guava-home.local` or device IP
   - Telegram: Send commands to your bot

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
├── docs/                      # Documentation
├── scripts/                   # Build/deployment scripts
├── README.md                  # This file
├── LICENSE                    # MIT License
└── .gitignore                 # Git ignore rules
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
- RFID access control
- WiFi encryption (WPA2/WPA3)
- Secure boot configuration
- Pin-protected settings

### Reliability
- Automatic reconnection
- Watchdog timer
- Error logging
- Graceful degradation

## Firmware Versions

| Version | Status | Features |
|---------|--------|----------|
| v1.5.1  | Latest | Full features, recommended |
| v1.4    | Stable | Core features |
| v1.2    | Legacy | Basic WiFi + sensors |
| v1.0    | Legacy | GPIO control only |

## Troubleshooting

### Device won't connect to WiFi
- Verify SSID and password are correct
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Check if router is in range
- Try factory reset

### Telegram bot not responding
- Verify bot token is correct
- Confirm chat ID matches your Telegram ID
- Check device has internet access

### Web UI not loading
- Ensure device is powered and connected to WiFi
- Try accessing via IP address instead of mDNS
- Check firewall settings

For more help, see [Setup Guide](docs/SETUP_GUIDE.md)

## License

MIT License - See LICENSE file for details

## Author

**Tumesh007** - [GitHub Profile](https://github.com/tumesh007)

## Support

- 📖 [Documentation](docs/)
- 💬 [GitHub Issues](https://github.com/tumesh007/guava-home/issues)
- 📧 Contact via GitHub

---

**Last Updated**: July 2026
**Status**: Active Development
