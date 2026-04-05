# Changelog

---

## [V1.0] — 2025 — Initial Public Release

First public release of Guava Home. Based on private development build V10.4.
All personal/location identifiers removed. Project renamed to Guava Home.

### Added
- **Telegram auto-status** — configurable interval (minutes). Set 0 to disable. Stored in flash.
- **Configurable Telegram poll interval** — 2–60 seconds, adjustable from web Config tab
- **Configurable pump ON/OFF cycle** — watering duration and rest period in seconds, web UI editable
- **Configurable soil dry threshold** — ADC value 1000–4095. Lower for plants needing wetter soil. Web UI editable with live current-value display
- **Clap counter** — total clap events tracked since boot, shown in `/status`
- **Garden & Pump section** in web Config tab covering all 4 tuning parameters
- **Telegram Automation section** in Config tab covering poll interval and auto-status
- **Dynamic soil threshold in JavaScript** — soil card colour updates in real-time when threshold changes
- Web dashboard: `curSDT` indicator shows current active threshold

### Fixed (from reference code, BUG audit)
- **BUG1 — ISR edge direction** — `RISING` changed to `FALLING`. KY-037 DO goes LOW on sound, not HIGH. Without this fix the clap sensor never triggered.
- **BUG2 — Auto-status immediate fire** — `tTGStatus` now initialised to `millis()` after WiFi connects. Without this, auto-status fired within milliseconds of boot.
- **BUG3/8 — Motion not logged** — `logEvent("Motion detected")` restored. Motion events now appear in web event log.
- **BUG6 — OTA safety** — `otaActive` flag and `if (otaActive) return;` in loop. Relay automation suspended during firmware flash.
- **BUG7 — Door lock not logged** — `logEvent("Door locked")` added to `lockDoor()`.
- **BUG9 — Pump fires immediately on boot** — `pump.phaseStart = millis()` set in setup. Pump now waits the full OFF duration before first ON.

### Changed
- Project name: Guava Home (was private Shivansh Home)
- AP network name: `Guava_Setup`
- OTA hostname: `GuavaHome`
- OTA default password: `guavahome`
- Flash namespace: `gh` (was `sh`)
- All personal identifiers removed from firmware, web UI, and boot messages
- `/status` Telegram command now includes pump cycle config, auto-status interval, and OTA status

### Inherited from V10.4 (development build)
- OTA firmware updates (ArduinoOTA + browser /update page)
- KY-037 hardware interrupt clap sensor (IRAM_ATTR, acoustic debounce)
- MQ-2 gas sensor replacing rain sensor (GPIO 36, 1kΩ/2kΩ divider)
- HC-SR04 pin swap (TRIG=12, ECHO=13) fixing boot crash on GPIO12 strapping pin
- Dynamic RFID access control (3 user slots, web UI editable, flash-backed)
- Live last-scanned UID on Config tab
- PIR daylight-aware lighting (motion + dark room required)
- Configurable PIR light-off delay
- 4-page LCD navigation with physical button
- LCD text scrolling (>16 chars)
- Ultrasonic LCD wake (presence < 80cm)
- Soil ADC inverted logic (dry=HIGH, wet=LOW)
- AP rescue mode with 10-minute self-destruct
- IST time sync (UTC+5:30)
- Telegram setInsecure() (no certificate expiry)
- All settings in flash (Preferences.h)
