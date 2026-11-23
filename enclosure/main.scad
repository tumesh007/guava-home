// ============================================================================
// PROJECT  : Guava Home — ESP32 Smart Home Controller Enclosure
// FILE     : main.scad
// PURPOSE  : Top-level entrypoint for assembly visualization and STL export.
// PRINTER  : Bambu Lab A1 Mini (180 × 180 × 180 mm)
// ============================================================================

include <config.scad>
use <base_housing.scad>
use <front_plate.scad>
use <buttons.scad>

// ----------------------------------------------------------------------------
// EXPORT MODE SELECTION
// Change 'part' to export individual STL files:
//   "assembly"    -> Full 3D exploded / mated assembly visualization
//   "base"        -> Base enclosure only (flat for bed printing)
//   "lid"         -> Front plate only (oriented for printing)
//   "buttons"     -> Button pair only
// ----------------------------------------------------------------------------
part = "assembly"; // ["assembly", "base", "lid", "buttons"]

// Optional explosion offset in millimeters for assembly view
explode_z = 25.0;

module hardware_mockups() {
    // 1. ESP32 38-Pin Purple Breakout Board Mockup
    color([0.5, 0.1, 0.7, 0.8]) // Purple PCB
        translate([ESP_POS_X, ESP_POS_Y, FLOOR_T + ESP_STANDOFF_H]) {
            cube([ESP_PCB_W, ESP_PCB_L, 1.6]);
            
            // ESP32 DevKit Module
            color([0.2, 0.2, 0.2, 0.9])
                translate([(ESP_PCB_W - 28.0) / 2, (ESP_PCB_L - 52.0) / 2, 1.6]) {
                    cube([28.0, 52.0, 4.5]);
                    // Metal RF Shield
                    color([0.8, 0.8, 0.8])
                        translate([2.0, 15.0, 4.5])
                            cube([18.0, 20.0, 2.5]);
                }
        }

    // 2. 10,000 mAh Powerbank Mockup
    color([0.2, 0.5, 0.8, 0.6])
        translate([WALL_T + 2.0, WALL_T + 4.0, FLOOR_T + 1.0])
            cube([68.0, 142.0, 16.0]);

    // 3. 16x2 LCD Display Mockup
    color([0.1, 0.4, 0.1, 0.8]) // Green PCB
        translate([LCD_POS_X, LCD_POS_Y, BASE_H + explode_z - LCD_BOSS_H - 1.6]) {
            cube([LCD_PCB_W, LCD_PCB_H, 1.6]);
            // LCD Glass bezel
            color([0.2, 0.6, 0.9, 0.9])
                translate([(LCD_PCB_W - LCD_BEZEL_W) / 2, (LCD_PCB_H - LCD_BEZEL_H) / 2, 1.6])
                    cube([LCD_BEZEL_W, LCD_BEZEL_H, 6.0]);
        }

    // 4. RC522 RFID Reader Mockup
    color([0.1, 0.3, 0.7, 0.8]) // Blue PCB
        translate([RFID_POS_X, RFID_POS_Y, BASE_H + explode_z - RFID_POCKET_D - 1.6]) {
            cube([RFID_PCB_W, RFID_PCB_L, 1.6]);
            // IC & antenna
            color([0.1, 0.1, 0.1])
                translate([12.0, 20.0, 1.6])
                    cube([15.0, 15.0, 1.5]);
        }
}

// ----------------------------------------------------------------------------
// RENDER LOGIC
// ----------------------------------------------------------------------------
if (part == "assembly") {
    // Base Housing (Charcoal / Dark Grey)
    color([0.22, 0.24, 0.28])
        base_housing();

    // Hardware components
    hardware_mockups();

    // Front Plate (Silver / Off-White)
    color([0.88, 0.89, 0.92, 0.95])
        translate([0, 0, BASE_H + explode_z])
            front_plate();

    // Tactile Buttons (Signal Orange)
    color([1.0, 0.45, 0.0]) {
        translate([BTN1_X, BTN_Y, BASE_H + explode_z + 1.5])
            single_button("E");
        translate([BTN2_X, BTN_Y, BASE_H + explode_z + 1.5])
            single_button("P");
    }
}
else if (part == "base") {
    // Slicing orientation: Floor flat on bed (open top facing +Z)
    base_housing();
}
else if (part == "lid") {
    // Slicing orientation: Face-down on bed (Z=0 at cosmetic front) or Face-up
    front_plate();
}
else if (part == "buttons") {
    // Slicing orientation: Flange flat on bed
    button_pair();
}
