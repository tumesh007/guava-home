# First Boot Setup

## Step 1 — Arduino IDE Settings

| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Partition | **Default 4MB with spiffs** |
| CPU Frequency | 240MHz |
| Upload Speed | 115200 |

> If upload fails: hold `BOOT` button while clicking Upload, release when "Connecting..." appears.

## Step 2 — Upload & Connect

1. Upload `GuavaHome_V1_0.ino` via USB
2. Open Serial Monitor at 115200 baud (optional — shows boot log)
3. ESP32 cannot find saved WiFi → creates AP: **`Guava_Setup`** (no password)
4. On phone/laptop: connect to `Guava_Setup`
5. Open browser → `http://192.168.4.1`

## Step 3 — Configure

In the **Config** tab:

| Field | What to enter |
|-------|--------------|
| WiFi SSID | Your home WiFi name (case-sensitive) |
| WiFi Password | Your WiFi password |
| Telegram Bot Token | From @BotFather (format: `1234567890:AAF...`) |
| Telegram Chat ID | Your personal ID — send a message to @userinfobot |
| OTA Password | Leave as `guavahome` or change it |

Click **Save Settings & Reboot**.

## Step 4 — Verify

After reboot, LCD shows IP address. Telegram receives:
```
Guava Home V1.0 Online
Dashboard: http://192.168.x.x
```

Open that URL on any phone/laptop on the same WiFi — dashboard loads.

## Step 5 — Add RFID Cards

1. Hold RFID card near scanner
2. Card UID appears in **Config tab → Last Scanned UID box**
3. Tap the box to copy the UID
4. Paste into User 1 UID field, enter a name
5. Click Save Settings & Reboot

## Step 6 — Set Garden Parameters

In Config → Garden & Pump:
- **Pump ON Duration** — how long pump runs per cycle (seconds)
- **Pump OFF Duration** — rest time between cycles (seconds)
- **Soil Dry Threshold** — ADC value above which plant needs water
  - Default 3000 (dry soil)
  - Lower for plants needing wetter soil (e.g. 2000)

## Subsequent Firmware Updates (OTA)

No USB cable needed after first flash:
1. Arduino IDE → Tools → Port → Network → `GuavaHome at 192.168.x.x`
2. Enter OTA password when prompted
3. Click Upload — firmware transfers over WiFi
4. Alternative: open `http://<IP>/update` → select `.bin` → Upload

## AP Rescue Mode

If home WiFi router is changed or password updated:
1. ESP32 creates `Guava_Setup` after 30 failed connect attempts
2. Connect to `Guava_Setup` → `http://192.168.4.1` → Config tab
3. Update WiFi credentials → Save & Reboot
4. AP mode self-destructs after 10 minutes and reboots automatically
