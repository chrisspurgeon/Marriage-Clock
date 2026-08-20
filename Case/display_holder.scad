$fn=20;



color("yellow")
translate([25,0,0])
cube([170,28,4]);



difference() {
    color("pink")
    translate([0,6,0])
    cube([220,16,4]);
    // Mounting holes
    translate([10,14,-1])
    cylinder(h=10,r=2.5);
    translate([210,14,-1])
    cylinder(h=10,r=2.5);
}

// 1
translate([30,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([10,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([30,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([10,25.4,5]);
    cylinder(h=19,r=1.45);
}

translate([65.56,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([45.56,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([65.56,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([45.56,25.4,5]);
    cylinder(h=19,r=1.45);
}


// 2
translate([71,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([51,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([71,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([51,25.4,5]);
    cylinder(h=19,r=1.45);
}

translate([106.56,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([86.56,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([106.56,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([86.56,25.4,5]);
    cylinder(h=19,r=1.45);
}

// 3
translate([112,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([92,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([112,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([92,25.4,5]);
    cylinder(h=19,r=1.45);
}

translate([147.56,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([127.56,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([147.56,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([127.56,25.4,5]);
    cylinder(h=19,r=1.45);
}

// 4
translate([153,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([133,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([153,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([133,25.4,5]);
    cylinder(h=19,r=1.45);
}

translate([188.56,2.5,0])
difference() {
    cylinder(h=16,r=3);
    translate([168.56,2.5,5]);
    cylinder(h=19,r=1.45);
}

translate([188.56,25.4,0])
difference() {
    cylinder(h=16,r=3);
    translate([168.56,25.4,5]);
    cylinder(h=19,r=1.45);
}


/*
$fn=200;
color("red")
translate([110,13,0])    // 126 & 94
scale([126, 26, 1])
cylinder(h = 2, r = 1);
*/
