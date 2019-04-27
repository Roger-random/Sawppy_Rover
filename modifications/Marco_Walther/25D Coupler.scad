$fa = 3;
$fs = .5;

union() {
    difference() {
        cylinder(h = 10, r = 12);
        cylinder(h = 10, r = 4.1);
        translate([0, 0, 5]) {
            rotate([90, 0, 0]) {
                cylinder(h = 15, r = 2.5);
            }
        }
    }

    translate([0, 0, 10]) {
        difference() {
            cylinder(h = 10, r = 10);
            cylinder(h = 10, r = 2.1);
            translate([0, 0, 5]) {
                rotate([90, 0, 0]) {
                    cylinder(h = 15, r = 2.5);
                }
            }
        }
    }
}