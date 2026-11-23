// ============================================================================
// PROJECT  : Guava Home — ESP32 Smart Home Controller Enclosure
// FILE     : base_housing.scad
// PURPOSE  : Bottom housing containing powerbank cradle, ESP32 standoffs,
//            wall-mount keyholes, cable exit ports, and strain-relief anchors.
// ============================================================================

include <config.scad>

module base_housing() {
    difference() {
        // --------------------------------------------------------------------
        // 1. MAIN SOLID BODY & INTERNAL BOSSES
        // --------------------------------------------------------------------
        union() {
            // Outer solid shell with rounded corners
            translate([BOX_W / 2, BOX_D / 2, 0])
                rounded_rect(BOX_W, BOX_D, BASE_H, CORNER_R);

            // 4x Corner Screw Bosses (solid columns for screw retention)
            for (cx = [CORNER_HOLE_MARGIN, BOX_W - CORNER_HOLE_MARGIN]) {
                for (cy = [CORNER_HOLE_MARGIN, BOX_D - CORNER_HOLE_MARGIN]) {
                    translate([cx, cy, 0])
                        cylinder(d = CORNER_BOSS_OD, h = BASE_H);
                }
            }

            // Divider Wall separating Powerbank and Controller Bay
            translate([DIVIDER_X, WALL_T, FLOOR_T])
                cube([DIVIDER_W, INT_D, DIVIDER_H]);

            // Standoff Bosses for ESP32 38-Pin Purple Breakout Board
            for (hx = [0, ESP_HOLE_DX]) {
                for (hy = [0, ESP_HOLE_DY]) {
                    translate([ESP_POS_X + ((ESP_PCB_W - ESP_HOLE_DX) / 2) + hx,
                               ESP_POS_Y + ((ESP_PCB_L - ESP_HOLE_DY) / 2) + hy,
                               FLOOR_T]) {
                        cylinder(d = ESP_STANDOFF_OD, h = ESP_STANDOFF_H);
                        // Stiffening fillet / gusset to base
                        cylinder(d1 = ESP_STANDOFF_OD + 3.0, d2 = ESP_STANDOFF_OD, h = 2.0);
                    }
                }
            }

            // Zip-tie strain-relief bridges for external sensor/relay cables
            translate([BOX_W - WALL_T - 16.0, WALL_T + 18.0, FLOOR_T])
                cube([10.0, 4.0, 3.5]);
            translate([BOX_W - WALL_T - 16.0, WALL_T + 26.0, FLOOR_T])
                cube([10.0, 4.0, 3.5]);
        }

        // --------------------------------------------------------------------
        // 2. MAIN INTERIOR CAVITY
        // --------------------------------------------------------------------
        translate([WALL_T, WALL_T, FLOOR_T])
            cube([INT_W, INT_D, BASE_H - FLOOR_T + 1.0]);

        // Stepped Mating Lip (Recessed rim for lid alignment)
        translate([WALL_T - LIP_OFFSET, WALL_T - LIP_OFFSET, BASE_H - LIP_HEIGHT])
            cube([INT_W + (2 * LIP_OFFSET), INT_D + (2 * LIP_OFFSET), LIP_HEIGHT + 1.0]);

        // --------------------------------------------------------------------
        // 3. CORNER SCREW PILOT HOLES (M3 SELF-TAPPING)
        // --------------------------------------------------------------------
        for (cx = [CORNER_HOLE_MARGIN, BOX_W - CORNER_HOLE_MARGIN]) {
            for (cy = [CORNER_HOLE_MARGIN, BOX_D - CORNER_HOLE_MARGIN]) {
                translate([cx, cy, BASE_H - 18.0])
                    cylinder(d = CORNER_PILOT_D, h = 19.0);
            }
        }

        // --------------------------------------------------------------------
        // 4. ESP32 STANDOFF PILOT HOLES (M3)
        // --------------------------------------------------------------------
        for (hx = [0, ESP_HOLE_DX]) {
            for (hy = [0, ESP_HOLE_DY]) {
                translate([ESP_POS_X + ((ESP_PCB_W - ESP_HOLE_DX) / 2) + hx,
                           ESP_POS_Y + ((ESP_PCB_L - ESP_HOLE_DY) / 2) + hy,
                           FLOOR_T])
                    cylinder(d = ESP_SCREW_PILOT_D, h = ESP_STANDOFF_H + 2.0);
            }
        }

        // --------------------------------------------------------------------
        // 5. WALL-MOUNT KEYHOLE SLOTS (DUAL 100mm SPACING)
        // --------------------------------------------------------------------
        for (kx = [(BOX_W / 2) - (KEYHOLE_SPACING_X / 2), (BOX_W / 2) + (KEYHOLE_SPACING_X / 2)]) {
            // Large entry hole (for screw head up to 8.5mm)
            translate([kx, KEYHOLE_POS_Y, -0.5])
                cylinder(d = KEYHOLE_ENTRY_D, h = FLOOR_T + 1.0);
            
            // Narrow slot (sliding upwards for M3.5 / M4 screw shank)
            translate([kx - (KEYHOLE_SLOT_W / 2), KEYHOLE_POS_Y, -0.5])
                cube([KEYHOLE_SLOT_W, KEYHOLE_SLOT_L, FLOOR_T + 1.0]);
            
            translate([kx, KEYHOLE_POS_Y + KEYHOLE_SLOT_L, -0.5])
                cylinder(d = KEYHOLE_SLOT_W, h = FLOOR_T + 1.0);
        }

        // --------------------------------------------------------------------
        // 6. CABLE PASS-THROUGHS & EXTERNAL SENSOR I/O PORTS
        // --------------------------------------------------------------------
        // Bottom Cable Exit Port (to external 4x Relays, Mains, Sensors)
        translate([BOX_W - WALL_T - 38.0, -1.0, FLOOR_T + 4.0])
            rounded_rect(CABLE_PORT_W, WALL_T + 2.0, CABLE_PORT_H, CABLE_PORT_R);

        // Right Wall Cable Exit Port (for PIR, HC-SR04, DHT)
        translate([BOX_W - WALL_T - 1.0, WALL_T + 30.0, FLOOR_T + 4.0])
            rotate([0, 0, 90])
                rounded_rect(SIDE_PORT_W, WALL_T + 2.0, SIDE_PORT_H, CABLE_PORT_R);

        // Powerbank to Controller Bay Wire Notch in Divider
        translate([DIVIDER_X - 1.0, WALL_T + 30.0, FLOOR_T + 6.0])
            cube([DIVIDER_W + 2.0, 20.0, DIVIDER_H]);

        // Powerbank USB Access Cutout (on bottom left wall for charging power bank)
        translate([WALL_T + 15.0, -1.0, FLOOR_T + 3.0])
            cube([20.0, WALL_T + 2.0, 10.0]);

        // Zip-tie slots through strain relief bridges
        translate([BOX_W - WALL_T - 13.0, WALL_T + 19.0, FLOOR_T + 1.0])
            cube([4.0, ZIP_SLOT_W, ZIP_SLOT_H]);
        translate([BOX_W - WALL_T - 13.0, WALL_T + 27.0, FLOOR_T + 1.0])
            cube([4.0, ZIP_SLOT_W, ZIP_SLOT_H]);

        // --------------------------------------------------------------------
        // 7. PASSIVE VENTILATION GRILLES (SIDE COOLING)
        // --------------------------------------------------------------------
        for (i = [0:6]) {
            // Left wall vents (near powerbank)
            translate([-1.0, WALL_T + 35.0 + (i * 12.0), FLOOR_T + 12.0])
                cube([WALL_T + 2.0, 6.0, 2.5]);
            
            // Right wall vents (near ESP32)
            translate([BOX_W - WALL_T - 1.0, WALL_T + 60.0 + (i * 12.0), FLOOR_T + 12.0])
                cube([WALL_T + 2.0, 6.0, 2.5]);
        }
    }
}

// Preview when rendered directly
base_housing();
