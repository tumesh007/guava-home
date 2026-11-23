# Guava Home — Wall-Mount 3D Printed Enclosure

Parametric OpenSCAD enclosure designed for the **Bambu Lab A1 Mini (180 × 180 mm bed)**.
Outer dimensions: **168 × 136 × 60 mm**. Support-free printing.

## Files Included
- `guava_home_enclosure.scad` — Master parametric CAD source (edit dimensions/tolerances here).
- `export_base.scad` — Direct Base export (back-down).
- `export_lid.scad` — Direct Lid export (flipped face-down).
- `export_test_slice.scad` — 7 mm bottom fit-check print (verify your Purple ESP32 & 4-Ch Relay fit before committing to the full print).
- `build_stls.sh` — One-command STL batch generator for Linux (`chmod +x build_stls.sh && ./build_stls.sh`).

## Generating STLs
### Option A: OpenSCAD GUI
1. Open `export_base.scad` → Press **F6** (Render) → **File > Export > Export as STL**.
2. Open `export_lid.scad` → Press **F6** (Render) → **File > Export > Export as STL**.

### Option B: CLI (Linux / Zorin OS)
```bash
chmod +x build_stls.sh
./build_stls.sh
```

## Recommended Bambu Studio Settings (A1 Mini)
- **Filament:** PETG (recommended due to continuous MQ-2 heater warmth; avoid standard PLA).
- **Nozzle / Layer Height:** 0.4 mm nozzle / 0.20 mm Standard.
- **Walls:** 3 wall loops (ensures solid divider walls and strong M3 screw bosses).
- **Infill:** 20% Gyroid.
- **Supports:** Disabled (None required).
- **Brim:** Outer brim (5 mm) recommended for PETG corners on the 168 mm footprint.

## Hardware Needed
- 4× M3 × 12 mm countersunk screws (self-taps into 2.7 mm bosses; change to 4.2 mm in `.scad` for heat-set inserts).
- 2× M2 × 6 mm screws (or hot glue) for the HC-SR501 PIR module.
- 3 mm nylon zip-ties to lock down the Purple ESP32 expansion board and 4-channel relay module.
- 2× wall screws spaced **100 mm** apart center-to-center (max head diameter 8.5 mm, shaft ≤ 4 mm).
- Small foam ring around the clap mic capsule to decouple mechanical relay clicks.
