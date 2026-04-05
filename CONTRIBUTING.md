# Contributing

Contributions welcome. This firmware runs on real hardware — please test on physical ESP32 before submitting.

## How to Contribute

1. Fork the repository
2. Create a branch: `git checkout -b feature/your-feature`
3. Make and test changes on hardware
4. Commit: `git commit -m "Add: BME280 support"`
5. Open a Pull Request

## Commit Prefixes

| Prefix | Use |
|--------|-----|
| `Add:` | New feature |
| `Fix:` | Bug fix |
| `Docs:` | Documentation only |
| `Refactor:` | Code cleanup, no behaviour change |

## Code Guidelines

- All timing uses `millis()` — never `delay()` in `loop()`
- New sensors: add to `SensorState` struct, read in fast sensor block, add to `buildJSON()`, add to `/status`
- Flash writes: only on Save — never in `loop()`
- ISR functions must be `IRAM_ATTR` and use only `volatile` variables
- Safety: relay pins must have `digitalWrite()` before `pinMode()` in `setup()`

## Opening Issues

Please include:
- Firmware version
- Arduino IDE + ESP32 core version
- Serial Monitor boot output
- Steps to reproduce
