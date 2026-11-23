// ============================================================================
// PROJECT  : Guava Home — ESP32 Smart Home Controller Enclosure
// FILE     : config.scad
// PRINTER  : Bambu Lab A1 Mini (180 × 180 × 180 mm build envelope)
// TARGET   : ESP32 38-Pin DevKit + Purple Breakout + 16x2 I2C LCD + RC522 RFID
//            + 10,000 mAh Powerbank + Dual Buttons + External Relay/Sensor I/O
// ============================================================================

$fn = 48; // Smooth curves for 3D printing

// ----------------------------------------------------------------------------
// 1. 3D PRINTER BED ENVELOPE (BAMBU LAB A1 MINI)
// ----------------------------------------------------------------------------
BED_X = 180.0;
BED_Y = 180.0;
BED_Z = 180.0;

// ----------------------------------------------------------------------------
// 2. ENCLOSURE OUTER DIMENSIONS & WALL THICKNESSES
// ----------------------------------------------------------------------------
BOX_W   = 172.0; // Outer Width (X)  - leaves 4.0 mm margin on each side for brim
BOX_D   = 156.0; // Outer Depth (Y)  - leaves 12.0 mm margin on each side
BASE_H  = 38.0;  // Base Housing Height (Z)
LID_H   = 5.0;   // Lid Plate Thickness (Z)
TOTAL_H = BASE_H + LID_H; // 43.0 mm total height (generous DuPont wire arch headroom)

WALL_T  = 2.4;   // 6 perimeters @ 0.4mm nozzle (ultra rigid shell)
FLOOR_T = 2.4;   // 12 solid bottom layers @ 0.2mm layer height
CORNER_R = 6.0;  // Outer corner radius for aesthetics & strength

LIP_OFFSET = 1.2; // Step width for interlocking tongue-and-groove lip
LIP_HEIGHT = 2.0; // Step height of mating perimeter rim
LIP_TOL    = 0.25;// Slicing fit clearance

// Bed fit assertions
assert(BOX_W <= BED_X, "ERROR: BOX_W exceeds Bambu Lab A1 Mini build plate (180mm)!");
assert(BOX_D <= BED_Y, "ERROR: BOX_D exceeds Bambu Lab A1 Mini build plate (180mm)!");
assert(TOTAL_H <= BED_Z, "ERROR: TOTAL_H exceeds Bambu Lab A1 Mini height!");

// Interior Dimensions
INT_W = BOX_W - (2 * WALL_T); // 167.2 mm
INT_D = BOX_D - (2 * WALL_T); // 151.2 mm
INT_H = BASE_H - FLOOR_T;    // 35.6 mm interior clearance

// ----------------------------------------------------------------------------
// 3. INTERNAL PARTITION: POWERBANK & CONTROLLER BAYS
// ----------------------------------------------------------------------------
// Left Bay: 10,000 mAh Powerbank (Standard slim envelope: ~146 x 71 x 16.5 mm)
PB_BAY_W     = 72.0;  // Clear width for 10,000 mAh power bank
DIVIDER_W    = 2.0;   // Thickness of internal partition wall
DIVIDER_H    = 22.0;  // Height of partition (leaves 13.6 mm top gap for cable routing)
DIVIDER_X    = WALL_T + PB_BAY_W; // X position of divider

// Right Bay: ESP32 Purple Breakout Board + Sensor Cable Routing
CONTROLLER_BAY_W = INT_W - PB_BAY_W - DIVIDER_W; // 93.2 mm

// ----------------------------------------------------------------------------
// 4. ESP32 38-PIN PURPLE EXPANSION BOARD STANDOFFS
// ----------------------------------------------------------------------------
ESP_PCB_W         = 63.0; // Expansion board width (X)
ESP_PCB_L         = 78.0; // Expansion board length (Y)
ESP_HOLE_DX       = 57.0; // Center-to-center hole pitch in X
ESP_HOLE_DY       = 72.0; // Center-to-center hole pitch in Y
ESP_STANDOFF_H    = 5.0;  // Standoff height above floor (prevents pin shorts, air gap)
ESP_STANDOFF_OD   = 7.0;  // Outer standoff boss diameter
ESP_SCREW_PILOT_D = 2.8;  // M3 self-tapping screw pilot hole (or M3 heat-set insert)

// Position of ESP32 board in Right Bay (centered horizontally & positioned lower)
ESP_POS_X = DIVIDER_X + DIVIDER_W + ((CONTROLLER_BAY_W - ESP_PCB_W) / 2);
ESP_POS_Y = WALL_T + 12.0;

// ----------------------------------------------------------------------------
// 5. 16x2 I2C LCD DISPLAY SPECIFICATIONS (FRONT LID MOUNT)
// ----------------------------------------------------------------------------
LCD_PCB_W     = 80.0;
LCD_PCB_H     = 36.0;
LCD_BEZEL_W   = 72.0; // Metal bezel cutout width
LCD_BEZEL_H   = 25.0; // Metal bezel cutout height
LCD_HOLE_DX   = 75.0; // 4x mounting holes pitch X
LCD_HOLE_DY   = 31.0; // 4x mounting holes pitch Y
LCD_BOSS_H    = 5.0;  // Standoff height on inside of lid
LCD_BOSS_OD   = 6.5;
LCD_PILOT_D   = 2.6;  // M2.5 / M3 screw pilot hole

// LCD location on Front Plate (Upper-Left portion)
LCD_POS_X = WALL_T + 10.0;
LCD_POS_Y = BOX_D - WALL_T - LCD_PCB_H - 10.0; // Y = 105.6 mm

// ----------------------------------------------------------------------------
// 6. RC522 RFID READER SPECIFICATIONS (FRONT LID MOUNT)
// ----------------------------------------------------------------------------
RFID_PCB_W       = 40.5;
RFID_PCB_L       = 60.5;
RFID_HOLE_DX     = 34.0;
RFID_HOLE_DY     = 54.0;
RFID_POCKET_D    = 3.2;  // Depth of pocket recessed from inside of lid
RFID_FACE_T      = 1.2;  // Plastic thickness remaining on front face (high RF coupling)
RFID_BOSS_H      = 4.5;
RFID_BOSS_OD     = 6.0;
RFID_PILOT_D     = 2.6;

// RFID reader location on Front Plate (Upper-Right portion)
RFID_POS_X = BOX_W - WALL_T - RFID_PCB_W - 14.0;
RFID_POS_Y = BOX_D - WALL_T - RFID_PCB_L - 8.0;

// ----------------------------------------------------------------------------
// 7. USER CONTROLS & ACOUSTIC PORTS (FRONT LID)
// ----------------------------------------------------------------------------
// 2x Tactile Pushbuttons: BTN1 (Exit/Unlock), BTN2 (Page Switch)
BTN_HOLE_D     = 8.2;  // Hole for button plunger
BTN_SPACING_X  = 26.0; // Distance between buttons
BTN1_X         = LCD_POS_X + 18.0;
BTN2_X         = BTN1_X + BTN_SPACING_X;
BTN_Y          = LCD_POS_Y - 26.0;

// Active Buzzer Acoustic Vent Grid
BUZZER_VENT_D  = 1.8;  // Individual acoustic hole diameter
BUZZER_VENT_P  = 3.6;  // Pitch between holes (3x3 grid)
BUZZER_POS_X   = RFID_POS_X + (RFID_PCB_W / 2);
BUZZER_POS_Y   = BTN_Y;

// ----------------------------------------------------------------------------
// 8. CABLE EXIT PORTS & PASS-THROUGHS (EXTERNAL SENSORS & RELAYS)
// ----------------------------------------------------------------------------
// Port 1: Bottom wall right side (for external relays, soil, rain, DHT sensors)
CABLE_PORT_W   = 28.0;
CABLE_PORT_H   = 10.0;
CABLE_PORT_R   = 3.0;  // Radiused corner to prevent wire chafing

// Port 2: Right side wall (for ultrasonic / door IR wires)
SIDE_PORT_W    = 22.0;
SIDE_PORT_H    = 10.0;

// Internal Cable Tie Strain Relief Slots (for 2.5mm zip-ties)
ZIP_SLOT_W     = 4.0;
ZIP_SLOT_H     = 1.8;

// ----------------------------------------------------------------------------
// 9. CORNER SCREW BOSSES (FASTEN LID TO BASE)
// ----------------------------------------------------------------------------
CORNER_BOSS_OD      = 9.0;
CORNER_PILOT_D      = 2.8; // M3 self-tap or heat-set insert pilot (15mm deep)
CORNER_HOLE_MARGIN  = 7.0; // Distance from outer shell edge
CORNER_SCREW_CLR_D  = 3.4; // Clearance hole in lid for M3 screw
CORNER_COUNTERSINK_D= 6.5; // 90° countersunk screw head diameter

// ----------------------------------------------------------------------------
// 10. WALL MOUNTING KEYHOLES (REAR FLOOR)
// ----------------------------------------------------------------------------
KEYHOLE_SPACING_X = 100.0; // Standard 100mm horizontal hole pitch
KEYHOLE_ENTRY_D   = 8.5;   // Large hole diameter for screw head
KEYHOLE_SLOT_W    = 4.4;   // Narrow slot width for screw shaft (M3.5 / M4)
KEYHOLE_SLOT_L    = 8.0;   // Upward slide length
KEYHOLE_POS_Y     = BOX_D - 35.0; // Positioned high on back plate for stable hang

// ============================================================================
// HELPER MODULES
// ============================================================================
module rounded_rect(w, d, h, r) {
    linear_extrude(height = h) {
        offset(r = r) {
            square([w - (2 * r), d - (2 * r)], center = true);
        }
    }
}
