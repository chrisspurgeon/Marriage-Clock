// Hollow elliptical container
// Overall dimensions: 260mm × 60mm × 50mm
// Wall thickness: 3mm
// Open at the top

$fn = 300;

difference() {
    color("#555555")
    // Outer elliptical container
    scale([130, 30, 1])
        cylinder(h = 90, r = 1);

    // Inner cavity
    // 3mm walls: inner dimensions = 194mm × 54mm
    // Bottom thickness = 3mm
    translate([0, 0, 3])
        scale([127, 27, 1])
            cylinder(h = 88, r = 1);
    
    translate([-165/2,-20/2,-1])
    cube([165,20,5]);

// tuner holes
translate([107,0,-1])
cylinder(h=5,d=10);

translate([-107,0,-1])
cylinder(h=5,d=10);


}
    
// Mounting brackets
difference() {
    color("black")
    translate([118,-5,60])
    cube([10,10,20]);
    translate([122.5,0,70])
    cylinder(h=11,r=1.9);
}
difference() {
    color("green")
    translate([-128,-5,60])
    cube([10,10,20]);
    translate([-122.5,0,70])
    cylinder(h=11,r=1.9);
}

// LEGS

color("black")
translate([60,-40,0])
cube([6,15,90]);

color("black")
translate([-66,-40,0])
cube([6,15,90]);




    // 165 x 20 (30 total height)



