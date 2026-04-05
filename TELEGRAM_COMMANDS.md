# Telegram Bot Commands

## Manual Commands

| Command | Action |
|---------|--------|
| `status` | Full sensor and device snapshot |
| `fan_on` / `fan_off` | Fan relay ON/OFF |
| `light_on` / `light_off` | Light relay ON/OFF |
| `unlock` | Unlock door for 3 seconds |
| `pump_on` / `pump_off` | Pump manual control |
| `pump_auto` | Enable automatic pump cycle |
| `pump_stop` | Stop pump and disable auto-cycle |
| `mute_on` / `mute_off` | Silence / re-enable buzzer |
| `next_page` | Cycle LCD display page |
| `help` | List all commands |
| `reboot` | Restart the ESP32 |

Commands work with or without the `/` prefix.

## Automatic Alerts

| Trigger | Message |
|---------|---------|
| PIR motion | `Motion detected` |
| Temp > 32°C | `Temp high: XX.XC — Fan ON` |
| Soil dry | `Plant needs water` + ADC value |
| Gas/smoke | `GAS/SMOKE ALERT` + ADC value |
| Intrusion | `INTRUSION: XXcm` |
| RFID access granted | `Access: [Name]` + UID |
| RFID access denied | `DENIED` + UID |
| Pump auto ON | `Pump ON — cycle N` |
| Auto-status | Full /status output at configured interval |

## Auto-Status

Configure the interval in web Config → Telegram Automation:
- Set **0** to disable
- Set **30** for status every 30 minutes
- Set **60** for hourly status
- Maximum: 1440 (once per day)

## Example Status Response

```
Guava Home V1.0
Temp/Hum: 28.4C / 61%
LDR:   820 DARK
Soil:  2450 OK (thresh:3000)
Gas:   340 OK
PIR:   clear
Dist:  142cm
Claps: 3
Fan:   OFF
Light: ON
Pump:  OFF (auto)
Pump cycle: 10s ON / 50s OFF
Auto-status: 30 min
OTA: ready (GuavaHome)
```

## Troubleshooting

**Bot not responding:**
- Check WiFi (LCD Page 4 shows status)
- Verify Bot Token in Config tab
- Verify Chat ID — message @userinfobot to get yours

**"Unauthorised" on every message:**
- Chat ID is wrong — get correct ID from @userinfobot

**Long delay before response:**
- Check Telegram Poll Interval in Config → currently set to N seconds
- Reduce if faster response needed (higher CPU load)
