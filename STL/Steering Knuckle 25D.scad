$fa = 3;
$fs = .5;

module clamp() {
    difference() {
        translate([0, -20, -21]) {
            cube(size = [15, 19.75, 42]);
        }
        rotate([0, 90, 0]) {
            cylinder(h = 20, r = 12.5);
        }
    }
}

module k() {
    difference() {
        union() {
            import("Steering Knuckle.stl");
            translate([-80, 17.5, -16]) {
                cube(size = [50, 2.5, 55]);
            }

            translate([-80, 17.5, 39]) {
                rotate([0, 45, 0]) {
                    cube(size = [25, 2.5, 18]);
                }
            }


            translate([-80, -20, -16]) {
                cube(size = [15, 37.5, 31]);
            }

            translate([-32, 0, 0]) {
                rotate([180, 0, 0]) {
                    clamp();
                }
            }
        }
        translate([-80, 0, 0]) {
            rotate([0, 90, 0]) {
                cylinder(h = 20, r = 12.5);
            }
        }
        translate ([-80, -12.5, 0]) {
            cube(size = [20 , 25, 15]);
        }
        translate([-81, -20, -18]) {
            rotate([45, 0, 0]) {
                cube(size = [49, 2, 2]);
            }
        }
       translate([-85, -20.5, 14]) {
            rotate([45, 0, 0]) {
                cube(size = [20, 2, 2]);
            }
        }
    }
}
 

//        translate([-32, -13, -14]) {
//            difference() {
//                rotate([0, 90, 0]) {
//                    cylinder(h = 15, r = 7);
//                }
//                translate([-1, 0, -7]) {
//                    cube(size = [17, 14, 14]);
//                }
//                translate([-1, -7, 0]) {
//                    cube(size = [17, 14, 14]);
//                }
//            }
//        }
        

module h() {
    union() {
        rotate([90, 0, 0]) {
            cylinder(h = 30, r = 1.6);
        }
        translate([0, -2, 0]) {
            rotate([90, 0, 0]) {
                cylinder(h = 28, r = 2.5);
            }
        }
        translate([0, -9, 0]) {
            rotate([90, 0, 0]) {
                cylinder(h = 21, r = 4.5);
            }
        }
    }
}

module h1() {
    union() {
        rotate([90, 0, 0]) {
            cylinder(h = 30, r = 1.6);
        }
        translate([0, -7, 0]) {
            rotate([90, 0, 0]) {
                cylinder(h = 23, r = 4.5);
            }
        }
    }
}


difference() {
    k();
    rotate([180, 0, 0]) {
        translate([-24.5, 0, -15.5]) {
            h();
        }
    }
    rotate([180, 0, 0]) {
        translate([-24.5, 0, 15.5]) {
            h();
        }
    }
}

rotate([180, 0, 0]) {
    translate([40, 0, 0]) {
        difference() {
            clamp();
            translate([7.5, 0, -15.5]) {
                h1();
            }
            translate([7.5, 0, 15.5]) {
                h1();
            }
        }
    }
}
// color("red") {
//    translate([-76, 0, 0]) {
//        rotate([0, 90, 0]) {
//            cylinder(h = 75, r = 12.5);
//        }
//    }
//}