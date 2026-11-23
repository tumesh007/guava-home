# Guava Home — 3D-Printable Wall Enclosure (Bambu Lab A1 Mini)

[![Bambu Lab A1 Mini](https://img.shields.io/badge/3D_Printer-Bambu_Lab_A1_Mini-orange?logo=bambulab)](https://bambulab.com)
[![OpenSCAD](https://img.shields.io/badge/CAD-OpenSCAD-blue.svg)](https://openscad.org)
[![Build Volume](https://img.shields.io/badge/Footprint-172_×_156_×_43_mm-brightgreen.svg)]()
[![Mounting](https://img.shields.io/badge/Mount-Wall_Keyholes_100mm-blueviolet.svg)]()

A compact, rugged, two-piece wall-mounted console engineered specifically to replace breadboard/cardboard prototypes for the **Guava Home ESP32 Smart Home Controller**. Optimized for immediate drop-and-print on the **180 × 180 × 180 mm build plate of the Bambu Lab A1 Mini**.

---

## 1. Design Overview & Physical Specifications

| Specification | Dimension / Value | Engineering Feature & Rationale |
|---|---|---|
| **Outer Dimensions (W × D × H)** | **172.0 × 156.0 × 43.0 mm** | Fits inside the 180 × 180 mm A1 Mini bed with 4.0 mm brim clearance |
| **Internal Clear Height** | **35.6 mm** | Ample vertical headroom so DuPont jumper wires arch freely without pinching |
| **Wall & Floor Thickness** | **2.40 mm** | 6 solid perimeter loops @ 0.4 mm nozzle for industrial-grade stiffness |
| **Powerbank Bay** | **72.0 × 151.2 × 22.0 mm** | Snug retention bay for slim 10,000 mAh power bank with bottom USB port access |
| **Controller Bay** | **93.2 × 151.2 mm** | Dedicated bay for the 38-Pin purple expansion board with 5 mm standoff air-gap |
| **Wall Mounting** | **Dual 100 mm Keyholes** | Rear teardrop slots (Ø 8.5 mm entry, 4.4 mm slot) for M3.5/M4 drywall screws |
| **Cable Management** | **Dual Ports + Zip Anchors** | 28 × 10 mm bottom port (relays) & 22 × 10 mm side port with zip-tie relief |
| **Corner Fasteners** | **4× M3 × 16–20 mm screws** | Deep corner screw bosses (Ø 2.8 mm pilot) for self-tapping M3 screws |

---

## 2. Front Plate UI & Component Layout

```
 ┌──────────────────────────────────────────────────────────────┐
 │  ┌───────────────────────┐         ┌──────────────────────┐  │
 │  │      16x2 I2C LCD     │         │   RC522 RFID READER  │  │
 │  │    (72 x 25mm window) │         │ (1.2mm RF faceplate) │  │
 │  └───────────────────────┘         │                      │  │
 │                                    │     [RFID TAP]       │  │
 │     GUAVA HOME CONTROLLER          └──────────────────────┘  │
 │                                                              │
 │      (E)          (P)                     ( ::: )            │
 │     EXIT         PAGE                     BUZZER             │
 └──────────────────────────────────────────────────────────────┘
```

1. **16x2 I2C LCD Display**:
   - Clean 72 × 25 mm bezel cutout with 45° chamfered frame.
   - 4× M2.5/M3 mounting bosses on underside (75 × 31 mm hole pitch).
2. **RC522 RFID Reader**:
   - Recessed interior pocket with faceplate thinned to **1.2 mm** for maximum 13.56 MHz RFID RF field coupling.
   - Distinct engraved card icon and radio wave target graphic on outer face.
3. **Tactile Pushbuttons (Exit & Page)**:
   - 2× Ø 8.2 mm apertures with engraved text labels (`EXIT` for door release, `PAGE` for LCD cycling).
   - Paired with captive 3D-printable plunger buttons (`buttons.scad`) that cannot fall out.
4. **Piezo Buzzer Acoustic Port**:
   - 3×3 matrix of Ø 1.8 mm acoustic vent holes aligned with the onboard active buzzer.
5. **Chimney Convective Cooling**:
   - Louvered slots along the top lid edge and lower side walls create natural convective airflow to keep the ESP32 and powerbank cool.

---

## 3. Bambu Lab A1 Mini Slicing & Print Profile

All parts in `enclosure/` are pre-oriented for direct printing on the Bambu Lab A1 Mini with **zero supports required**.

| Parameter | Setting | Rationale |
|---|---|---|
| **Printer** | Bambu Lab A1 Mini (0.4 mm Nozzle) | Footprint (172 × 156 mm) verified against 180 × 180 mm plate |
| **Build Plate** | WOL3D Gold Textured PEI / Textured PEI | Clean first-layer texture, superior bed adhesion |
| **Filament (Base)** | **PETG** or **PLA / PLA+** | High rigidity for screw bosses, keyholes, and wire retention |
| **Filament (Lid)** | **PETG** or **PLA+** | Toughness for living fits, countersinks, and display mounts |
| **Layer Height** | **0.20 mm Standard** | Clean layer multiples for step lip (2.0 mm) and RFID pocket (1.2 mm) |
| **Wall Loops** | **6** | 100% solid perimeter walls (2.4 mm) for durability and screw holding |
| **Top Shell Layers** | 5 (1.0 mm) | Prevents pillowing over sparse infill |
| **Bottom Shell Layers** | 5 (1.0 mm) | Solid rigid base floor |
| **Infill** | 20% Gyroid | Isotropic shear and compressive strength |
| **Supports** | **DISABLED (None)** | Chamfered bridges and teardrop geometries eliminate supports |
| **Brim (Base Box)** | **4–5 mm Outer Brim** | **MANDATORY.** Prevents sharp corner lifting of deep base box |
| **Brim (Lid)** | None | Stays flat on textured PEI |

---

## 4. Repository Structure

```
enclosure/
├── config.scad             # Master parametric configuration & Bambu A1 Mini assertions
├── base_housing.scad       # Base box, powerbank bay, ESP32 standoffs, keyholes, cable ports
├── front_plate.scad        # Front lid with LCD bezel, RFID pocket, button holes, buzzer grille
├── buttons.scad            # 3D printable captive tactile button plungers
├── main.scad               # Assembly preview & single-part STL export entrypoint
├── stl/                    # Exported STL model binaries
└── README.md               # Slicing, BOM & assembly instructions (this file)
```

---

## 5. Bill of Materials (BOM)

| Item | Component | Qty | Notes |
|---|---|:---:|---|
| **MCU** | ESP32 DevKit V1 (38-pin) | 1 | Plugged into purple expansion board |
| **Expansion Board** | Purple 38-Pin Terminal Breakout | 1 | 78 × 63 mm footprint with screw terminals |
| **Display** | 16x2 LCD with I2C Backpack | 1 | 80 × 36 mm PCB, 75 × 31 mm hole spacing |
| **RFID** | RC522 RFID Reader Module | 1 | 60 × 40 mm PCB, 3.3V powered |
| **Power** | 10,000 mAh Slim Powerbank | 1 | Standard ~146 × 71 × 16 mm envelope |
| **Buttons** | 6×6 mm or 12×12 mm Tactile Switches | 2 | Exit (GPIO 26), Page (GPIO 35) |
| **Buzzer** | 5V / 3.3V Active Buzzer | 1 | Ø 12 mm standard buzzer (GPIO 14) |
| **Fasteners (Case)** | M3 × 16–20 mm Countersunk Screws | 4 | Fastens front plate into base corner bosses |
| **Fasteners (PCBs)** | M3 or M2.5 × 6 mm Pan Head Screws | 12 | 4× ESP32, 4× LCD, 4× RFID |
| **Wall Screws** | M3.5 or M4 Pan Head Screws | 2 | Anchored into wall at 100 mm pitch |
| **Strain Relief** | 2.5 mm × 100 mm Nylon Cable Ties | 2 | Clamps wire bundles at exit ports |

---

## 6. Assembly & Wiring Steps

1. **Mount Base to Wall**:
   - Drive 2× M3.5 or M4 screws into the wall with exactly **100 mm horizontal spacing**, leaving ~3 mm of screw head exposed.
   - Align the base housing's rear keyholes and slide downward to lock in place.
2. **Install Electronics**:
   - Fasten the **Purple 38-pin ESP32 expansion board** onto the 4 lower standoffs with M3 × 6 mm screws.
   - Slide your **10,000 mAh power bank** into the left compartment. Plug a short right-angle or flexible USB cable into the power bank, routing it through the divider notch to the ESP32.
3. **Mount UI to Front Plate**:
   - Insert the two 3D-printed button plungers into the front plate holes from the inside (their wide retention flanges keep them captive).
   - Fasten the **16x2 LCD** to its 4 underside bosses using M2.5/M3 × 6 mm screws.
   - Press the **RC522 RFID reader** flat into its recessed 1.2 mm pocket and secure with 4× screws.
4. **Wire Jumper Cables**:
   - Connect I2C (SDA/SCL/VCC/GND), RFID SPI bus, and button switches using DuPont jumper wires. With 35.6 mm clear height, jumper wire loops fit comfortably with zero crushing.
   - Pass wires for external relays (4 channels) and sensors (PIR, ultrasonic, DHT, rain, soil) through the bottom and side exit ports. Secure each bundle with a zip-tie to the internal anchor bridges.
5. **Close Enclosure**:
   - Lower the front plate onto the base. The 2.0 mm tongue-and-groove stepped perimeter lip aligns the enclosure automatically.
   - Drive 4× M3 × 16–20 mm countersunk screws into the corners.
