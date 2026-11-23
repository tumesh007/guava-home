// ============================================================================
// PROJECT  : Guava Home — ESP32 Smart Home Controller Enclosure
// FILE     : front_plate.scad
// PURPOSE  : Top faceplate / lid housing 16x2 LCD display, RC522 RFID reader,
//            tactile button ports, buzzer acoustic vents, and top ventilation.
// ============================================================================

include <config.scad>

module front_plate() {
    difference() {
        // --------------------------------------------------------------------
        // 1. MAIN SOLID LID & INTERNAL BOSSES
        // --------------------------------------------------------------------
        union() {
            // Main plate with rounded corners
            translate([BOX_W / 2, BOX_D / 2, 0])
                rounded_rect(BOX_W, BOX_D, LID_H, CORNER_R);

            // Stepped Tongue / Perimeter Flange for interlocking base fit
            translate([WALL_T + LIP_OFFSET + LIP_TOL,
                       WALL_T + LIP_OFFSET + LIP_TOL,
                       -LIP_HEIGHT])
                cube([INT_W - (2 * (LIP_OFFSET + LIP_TOL)),
                      INT_D - (2 * (LIP_OFFSET + LIP_TOL)),
                      LIP_HEIGHT + 0.1]);

            // 4x LCD Mounting Bosses on underside of lid
            for (lx = [0, LCD_HOLE_DX]) {
                for (ly = [0, LCD_HOLE_DY]) {
                    translate([LCD_POS_X + ((LCD_PCB_W - LCD_HOLE_DX) / 2) + lx,
                               LCD_POS_Y + ((LCD_PCB_H - LCD_HOLE_DY) / 2) + ly,
                               -LCD_BOSS_H])
                        cylinder(d = LCD_BOSS_OD, h = LCD_BOSS_H + 0.1);
                }
            }

            // 4x RFID Reader Mounting Bosses on underside of lid
            for (rx = [0, RFID_HOLE_DX]) {
                for (ry = [0, RFID_HOLE_DY]) {
                    translate([RFID_POS_X + ((RFID_PCB_W - RFID_HOLE_DX) / 2) + rx,
                               RFID_POS_Y + ((RFID_PCB_L - RFID_HOLE_DY) / 2) + ry,
                               -RFID_BOSS_H])
                        cylinder(d = RFID_BOSS_OD, h = RFID_BOSS_H + 0.1);
                }
            }
        }

        // --------------------------------------------------------------------
        // 2. 16x2 I2C LCD BEZEL CUTOUT & CHAMFER
        // --------------------------------------------------------------------
        translate([LCD_POS_X + ((LCD_PCB_W - LCD_BEZEL_W) / 2),
                   LCD_POS_Y + ((LCD_PCB_H - LCD_BEZEL_H) / 2),
                   -LIP_HEIGHT - 1.0])
            cube([LCD_BEZEL_W, LCD_BEZEL_H, LID_H + LIP_HEIGHT + 2.0]);

        // 4x LCD Screw Pilot Holes (M2.5 / M3)
        for (lx = [0, LCD_HOLE_DX]) {
            for (ly = [0, LCD_HOLE_DY]) {
                translate([LCD_POS_X + ((LCD_PCB_W - LCD_HOLE_DX) / 2) + lx,
                           LCD_POS_Y + ((LCD_PCB_H - LCD_HOLE_DY) / 2) + ly,
                           -LCD_BOSS_H - 1.0])
                    cylinder(d = LCD_PILOT_D, h = LCD_BOSS_H + 2.0);
            }
        }

        // --------------------------------------------------------------------
        // 3. RC522 RFID READER POCKET (1.2mm THIN FRONT WALL FOR MAX RF RANGE)
        // --------------------------------------------------------------------
        translate([RFID_POS_X, RFID_POS_Y, -RFID_POCKET_D])
            cube([RFID_PCB_W, RFID_PCB_L, RFID_POCKET_D + 0.1]);

        // 4x RFID Screw Pilot Holes (M2.5 / M3)
        for (rx = [0, RFID_HOLE_DX]) {
            for (ry = [0, RFID_HOLE_DY]) {
                translate([RFID_POS_X + ((RFID_PCB_W - RFID_HOLE_DX) / 2) + rx,
                           RFID_POS_Y + ((RFID_PCB_L - RFID_HOLE_DY) / 2) + ry,
                           -RFID_BOSS_H - 1.0])
                    cylinder(d = RFID_PILOT_D, h = RFID_BOSS_H + 2.0);
            }
        }

        // RFID Outer Face Target Engraving
        translate([RFID_POS_X + (RFID_PCB_W / 2), RFID_POS_Y + (RFID_PCB_L / 2), LID_H - 0.6]) {
            // Card icon outline
            difference() {
                cube([26.0, 36.0, 1.0], center = true);
                cube([23.6, 33.6, 1.2], center = true);
            }
            // Signal waves
            for (r = [4.0, 7.0, 10.0]) {
                difference() {
                    cylinder(r = r, h = 1.0, center = true);
                    cylinder(r = r - 1.0, h = 1.2, center = true);
                    translate([-r - 1, -r - 1, -1]) cube([(2 * r) + 2, r + 1, 2]);
                }
            }
        }

        // RFID Text Label
        translate([RFID_POS_X + (RFID_PCB_W / 2), RFID_POS_Y + 8.0, LID_H - 0.5])
            linear_extrude(height = 0.8)
                text("RFID TAP", size = 4.2, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");

        // --------------------------------------------------------------------
        // 4. USER BUTTON APERTURES & ENGRAVED LABELS
        // --------------------------------------------------------------------
        // BTN1 (Exit Button)
        translate([BTN1_X, BTN_Y, -LIP_HEIGHT - 1.0])
            cylinder(d = BTN_HOLE_D, h = LID_H + LIP_HEIGHT + 2.0);
        
        translate([BTN1_X, BTN_Y + 9.0, LID_H - 0.5])
            linear_extrude(height = 0.8)
                text("EXIT", size = 3.6, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");

        // BTN2 (Page Switch Button)
        translate([BTN2_X, BTN_Y, -LIP_HEIGHT - 1.0])
            cylinder(d = BTN_HOLE_D, h = LID_H + LIP_HEIGHT + 2.0);
        
        translate([BTN2_X, BTN_Y + 9.0, LID_H - 0.5])
            linear_extrude(height = 0.8)
                text("PAGE", size = 3.6, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");

        // --------------------------------------------------------------------
        // 5. ACTIVE BUZZER ACOUSTIC VENTILATION GRID (3x3 ARRAY)
        // --------------------------------------------------------------------
        for (bx = [-1, 0, 1]) {
            for (by = [-1, 0, 1]) {
                translate([BUZZER_POS_X + (bx * BUZZER_VENT_P),
                           BUZZER_POS_Y + (by * BUZZER_VENT_P),
                           -LIP_HEIGHT - 1.0])
                    cylinder(d = BUZZER_VENT_D, h = LID_H + LIP_HEIGHT + 2.0);
            }
        }

        // --------------------------------------------------------------------
        // 6. TOP CONVECTIVE VENTILATION SLOTS
        // --------------------------------------------------------------------
        for (i = [0:7]) {
            translate([LCD_POS_X + 6.0 + (i * 8.5), BOX_D - WALL_T - 5.0, -1.0])
                cube([4.5, 3.5, LID_H + 2.0]);
        }

        // --------------------------------------------------------------------
        // 7. CORNER COUNTERSUNK SCREW HOLES (4x M3)
        // --------------------------------------------------------------------
        for (cx = [CORNER_HOLE_MARGIN, BOX_W - CORNER_HOLE_MARGIN]) {
            for (cy = [CORNER_HOLE_MARGIN, BOX_D - CORNER_HOLE_MARGIN]) {
                // Shaft clearance hole
                translate([cx, cy, -LIP_HEIGHT - 1.0])
                    cylinder(d = CORNER_SCREW_CLR_D, h = LID_H + LIP_HEIGHT + 2.0);
                
                // 90-degree countersink cone
                translate([cx, cy, LID_H - 2.0])
                    cylinder(d1 = CORNER_SCREW_CLR_D, d2 = CORNER_COUNTERSINK_D, h = 2.1);
            }
        }

        // Title Branding Text
        translate([LCD_POS_X + (LCD_PCB_W / 2), LCD_POS_Y - 9.0, LID_H - 0.5])
            linear_extrude(height = 0.8)
                text("GUAVA HOME CONTROLLER", size = 3.8, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");
    }
}

// Preview when rendered directly
front_plate();
