// ============================================================================
// PROJECT  : Guava Home — ESP32 Smart Home Controller Enclosure
// FILE     : buttons.scad
// PURPOSE  : 3D-printable tactile button caps (Exit and Page) with captive
//            retention flanges to prevent dropping out through front plate.
// ============================================================================

include <config.scad>

module single_button(label = "") {
    stem_d   = BTN_HOLE_D - 0.7; // 7.5 mm diameter (slides smoothly in 8.2mm hole)
    stem_h   = 7.0;             // Total plunger height
    flange_d = 11.2;            // Retention flange diameter
    flange_h = 1.4;             // Flange thickness
    
    difference() {
        union() {
            // Wide retention base flange (rests behind lid aperture)
            cylinder(d = flange_d, h = flange_h);
            
            // Plunger stem extending through front face
            cylinder(d = stem_d, h = stem_h);
            
            // Subtle top chamfer for tactile feel
            translate([0, 0, stem_h - 0.6])
                cylinder(d1 = stem_d, d2 = stem_d - 1.2, h = 0.6);
        }
        
        // Tactile concave thumb dish on top face
        translate([0, 0, stem_h + 8.0])
            sphere(r = 8.5);
            
        // Cavity in underside to engage switch actuator
        translate([0, 0, -0.1])
            cylinder(d = 4.0, h = 2.5);
            
        // Optional embossed letter on top (e.g., 'E' or 'P')
        if (len(label) > 0) {
            translate([0, 0, stem_h - 0.4])
                linear_extrude(height = 0.6)
                    text(label, size = 3.2, font = "Liberation Sans:style=Bold", halign = "center", valign = "center");
        }
    }
}

module button_pair() {
    translate([-8.0, 0, 0])
        single_button("E"); // Exit Button
        
    translate([8.0, 0, 0])
        single_button("P"); // Page Button
        
    // Sacrificial 0.4mm breakaway runner connecting pair on build plate
    translate([-8.0, -0.6, 0])
        cube([16.0, 1.2, 0.4]);
}

// Render pair by default
button_pair();
