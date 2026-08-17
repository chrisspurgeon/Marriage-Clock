// Hollow elliptical container
// Overall dimensions: 200mm × 60mm × 50mm
// Wall thickness: 3mm
// Open at the top

$fn = 300;

difference() {
    // Outer elliptical container
    scale([110, 30, 1])
        cylinder(h = 90, r = 1);

    // Inner cavity
    // 3mm walls: inner dimensions = 194mm × 54mm
    // Bottom thickness = 3mm
    translate([0, 0, 3])
        scale([107, 27, 1])
            cylinder(h = 88, r = 1);
    
    translate([-165/2,-20/2,-1])
    cube([165,20,5]);
}
//    translate([-165/2,-30/2,0])
//    color("red")
//    cube([165,30,5]);
    
// Mounting brackets
difference() {
    color("red")
    translate([98,-5,60])
    cube([10,10,20]);
    translate([102.5,0,70])
    cylinder(h=11,r=1.9);
}
difference() {
    color("green")
    translate([-108,-5,60])
    cube([10,10,20]);
    translate([-102.5,0,70])
    cylinder(h=11,r=1.9);
}

// LEGS

color("yellow")
translate([60,-38,0])
cube([6,15,90]);

color("yellow")
translate([-66,-38,0])
cube([6,15,90]);


// tuner holes
//color("white")
//translate([95,0,-1])
//cylinder(h=5,d=20);



    // 165 x 20 (30 total height)



