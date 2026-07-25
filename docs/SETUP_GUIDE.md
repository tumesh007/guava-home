# Complete Setup Guide

## Prerequisites
- Arduino IDE 1.8.19 or later
- ESP32 Board Package (via Board Manager)
- USB to UART driver installed
- WiFi network access
- Telegram Bot token (from @BotFather)

## Installation Steps

### 1. Install Arduino IDE
Download from https://www.arduino.cc/en/software

### 2. Add ESP32 Board Support
1. Open Arduino IDE → Preferences
2. Add to "Additional Boards Manager URLs":
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. Tools → Board Manager → Search "ESP32" → Install

### 3. Configure WiFi & Telegram
Edit the firmware configuration in src/firmware/GuavaHome_Latest.ino:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* telegramBotToken = "YOUR_BOT_TOKEN";
const char* telegramChatId = "YOUR_CHAT_ID";
```

### 4. Upload Firmware
1. Select Board: Tools → Board → ESP32 Dev Module
2. Select Port: Tools → Port → COM# (your device)
3. Click Upload
4. Wait for completion

### 5. Access Web UI
- Find device IP from Serial Monitor (115200 baud)
- Navigate to `http://<device-ip>` in browser
- Or use mDNS: `http://guava-home.local`

## Troubleshooting

### Device not appearing
- Check USB cable (data cable, not power-only)
- Install CH340 USB driver if needed
- Try different USB port

### WiFi connection fails
- Verify SSID and password
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Check router is within range

### Telegram bot not responding
- Verify bot token is correct
- Check chat ID matches your Telegram user ID
- Ensure device has internet access

## Firmware Versions

- v1.5.1 (Latest): Full feature set, recommended
- v1.4: Stable, fewer features
- v1.2, v1.0: Legacy versions, not recommended

See CHANGELOG.md for version history.
