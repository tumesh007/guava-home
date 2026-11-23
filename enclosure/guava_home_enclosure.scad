// ============================================================================
// Guava Home Wall-Mount Enclosure (Bambu Lab A1 Mini Friendly: 168 x 136 mm)
// Prints support-free: Base back-down, Lid face-down
// ============================================================================

PART = "assembly"; // Options: "base", "lid", "test_slice", "assembly"

// --- Outer Box Dimensions (mm) ---
box_w    = 168;  // X (Fits 180mm A1 Mini bed with margin)
box_h    = 136;  // Y
box_d    = 60;   // Z (Clearance for Purple board + ESP32 + DuPont jumpers)
wall     = 2.8;  // Perimeter & divider wall thickness
lid_t    = 2.8;  // Front lid thickness
corner_r = 4;    // Outer corner radius
tol      = 0.4;  // Fit tolerance for lid lip

// --- Internal Zoning ---
divider_x  = 88; // Solid barrier between Logic (left) and Relay/MQ-2 (right)
dht_wall_y = 34; // Thermal wall isolating DHT11 in bottom-left
mq2_wall_y = 94; // Thermal wall isolating MQ-2 in top-right

// --- Sensor Cutout Dimensions ---
pir_dome_d = 23.6; // HC-SR501 Fresnel lens
pir_hole_d = 2.2;  // M2 self-tap
pir_pitch  = 28.0;
mic_d      = 10.6; // Electret mic capsule (add thin foam ring around it)
ldr_d      = 6.0;  // LDR light tunnel
eps        = 0.1;

$fn = 48;

// --- Modules ---
module rounded_box(w, h, d, r) {
    hull() {
        for (x = [r, w - r], y = [r, h - r])
            translate([x, y, 0]) cylinder(r = r, h = d);
    }
}

module keyhole_slot(x, y) {
    translate([x, y, -eps]) {
        // Large head entry (9mm)
        cylinder(d = 9.0, h = wall + 2*eps);
        // Upper shaft slot (4.5mm)
        hull() {
            cylinder(d = 4.5, h = wall + 2*eps);
            translate([0, 9, 0]) cylinder(d = 4.5, h = wall + 2*eps);
        }
    }
}

module zip_tie_standoff(x, y, h = 6) {
    // Raised pad with slot underneath for 3mm zip-tie (fits any clone hole pitch)
    translate([x, y, wall]) {
        difference() {
            cube([12, 8, h], center = false);
            translate([3, -eps, 1.5]) cube([6, 8 + 2*eps, 2.5]);
        }
    }
}

module vent_slats(x0, y0, count, len, spacing, vertical = false, z_h = wall + 2*eps) {
    for (i = [0 : count - 1]) {
        if (vertical)
            translate([x0 + i*spacing, y0, -eps]) cube([2.2, len, z_h]);
        else
            translate([x0, y0 + i*spacing, -eps]) cube([len, 2.2, z_h]);
    }
}

module base() {
    boss_h = box_d - lid_t - 0.3;
    difference() {
        union() {
            // Outer Shell
            difference() {
                rounded_box(box_w, box_h, box_d - lid_t, corner_r);
                translate([wall, wall, wall])
                    rounded_box(box_w - 2*wall, box_h - 2*wall, box_d, max(1, corner_r - wall));
            }

            // Corner M3 Screw Bosses
            for (bx = [wall + 4.5, box_w - wall - 4.5], by = [wall + 4.5, box_h - wall - 4.5])
                translate([bx, by, wall]) cylinder(d = 9, h = boss_h - wall);

            // Solid Vertical Barrier (Logic vs Relays)
            translate([divider_x, wall, wall])
                cube([3.0, box_h - 2*wall, boss_h - wall - 2]);

            // DHT11 Thermal Isolation Wall (Bottom-Left)
            translate([wall, dht_wall_y, wall])
                cube([36, 2.4, boss_h - wall - 2]);
            translate([wall + 36, wall, wall])
                cube([2.4, dht_wall_y - wall + 2.4, boss_h - wall - 2]);

            // MQ-2 Thermal Isolation Wall (Top-Right)
            translate([divider_x + 3.0, mq2_wall_y, wall])
                cube([box_w - divider_x - 3.0 - wall, 2.4, boss_h - wall - 2]);

            // Universal Zip-Tie Standoffs: Purple ESP32 Board (Upper-Left)
            zip_tie_standoff(26, 48);
            zip_tie_standoff(66, 48);
            zip_tie_standoff(26, 112);
            zip_tie_standoff(66, 112);

            // Universal Zip-Tie Standoffs: 4-Channel Relay (Bottom-Right)
            zip_tie_standoff(102, 16);
            zip_tie_standoff(144, 16);
            zip_tie_standoff(102, 76);
            zip_tie_standoff(144, 76);
        }

        // M3 Self-Tap Screw Holes in Bosses (2.7mm for M3, change to 4.2mm for brass inserts)
        for (bx = [wall + 4.5, box_w - wall - 4.5], by = [wall + 4.5, box_h - wall - 4.5])
            translate([bx, by, boss_h - 14]) cylinder(d = 2.7, h = 15);

        // Wall-Mount Keyholes (100mm center-to-center)
        keyhole_slot(34, box_h - 24);
        keyhole_slot(134, box_h - 24);

        // Wire pass-through notch in main divider (for 5V + 4x IN jumper wires)
        translate([divider_x - eps, 56, wall + 8]) cube([3.0 + 2*eps, 22, 14]);

        // Left-Side USB / External Powerbank Cable Slot
        translate([-eps, 72, wall + 6]) cube([wall + 2*eps, 18, 14]);

        // Bottom-Center External Sensor Probe Exit (Soil Moisture / IR)
        translate([52, -eps, wall + 4]) cube([18, wall + 2*eps, 10]);

        // Bottom-Right Relay Load Wire Exit Slot
        translate([112, -eps, wall + 4]) cube([34, wall + 2*eps, 14]);

        // Bottom-Left DHT11 Intake Vents
        translate([10, -eps, wall + 6]) cube([20, wall + 2*eps, 12]);

        // Top-Right MQ-2 Heat Exhaust Chimney Slots
        for (x = [102 : 7 : 150])
            translate([x, box_h - wall - eps, wall + 8]) cube([3.2, wall + 2*eps, 26]);
    }
}

module lid() {
    lip_h = 3.0;
    difference() {
        union() {
            // Flat Faceplate
            rounded_box(box_w, box_h, lid_t, corner_r);

            // Inner Alignment Lip (keeps lid from sliding)
            difference() {
                translate([wall + tol, wall + tol, lid_t])
                    rounded_box(box_w - 2*(wall + tol), box_h - 2*(wall + tol), lip_h, max(1, corner_r - wall));
                translate([wall + tol + 2, wall + tol + 2, lid_t - eps])
                    rounded_box(box_w - 2*(wall + tol + 2), box_h - 2*(wall + tol + 2), lip_h + 2*eps, 1);
            }

            // Opaque LDR Light Shroud (blocks internal ESP32 LEDs)
            translate([16, 76, lid_t])
                difference() {
                    cylinder(d = ldr_d + 4, h = 14);
                    translate([0, 0, -eps]) cylinder(d = ldr_d, h = 14 + 2*eps);
                }
        }

        // Corner M3 Countersunk Clearance Holes
        for (bx = [wall + 4.5, box_w - wall - 4.5], by = [wall + 4.5, box_h - wall - 4.5]) {
            translate([bx, by, -eps]) cylinder(d = 3.4, h = lid_t + lip_h + 2*eps);
            translate([bx, by, -eps]) cylinder(d1 = 6.4, d2 = 3.4, h = 1.8);
        }

        // PIR Sensor Cutout (Over Relay Bay -> zero jumper collision)
        pir_x = 128; pir_y = 54;
        translate([pir_x, pir_y, -eps]) cylinder(d = pir_dome_d, h = lid_t + 2*eps);
        translate([pir_x - pir_pitch/2, pir_y, -eps]) cylinder(d = pir_hole_d, h = lid_t + 2*eps);
        translate([pir_x + pir_pitch/2, pir_y, -eps]) cylinder(d = pir_hole_d, h = lid_t + 2*eps);

        // LDR Cutout
        translate([16, 76, -eps]) cylinder(d = ldr_d, h = lid_t + 2*eps);

        // Sound/Clap Mic Port (Far-Left, away from relays)
        translate([16, 106, -eps]) cylinder(d = mic_d, h = lid_t + 2*eps);

        // DHT11 Front Convection Louvers (Bottom-Left)
        vent_slats(10, 10, 4, 22, 5.5, false);

        // MQ-2 Front Gas Intake Louvers (Top-Right)
        vent_slats(102, 104, 5, 48, 5.0, false);
    }
}

module lid_printable() {
    // Flipped face-down on Z=0 for support-free printing
    translate([0, box_h, 0]) rotate([180, 0, 0]) translate([0, 0, -lid_t]) lid();
}

module test_slice() {
    // 15-minute footprint & zip-tie standoff fit test (bottom 7mm of base)
    intersection() {
        base();
        cube([box_w, box_h, 7.0]);
    }
}

// --- Render Selector ---
if (PART == "base") {
    base();
} else if (PART == "lid") {
    lid_printable();
} else if (PART == "test_slice") {
    test_slice();
} else {
    base();
    color("SteelBlue", 0.65)
        translate([0, 0, box_d - lid_t])
            translate([0, box_h, lid_t]) rotate([180, 0, 0]) lid();
}
