// total height is 84




$fn = 300;

// BACK PLATE
difference() {
    scale([126, 26, 1])
    cylinder(h = 3, r = 1);

    // Mounting holes
    translate([122.5,0,-1])
    cylinder(h=11,r=2.5);
    translate([-122.5,0,-1])
    cylinder(h=11,r=2.5);
    
    // POWER OUTLET
    translate([90,0,-1])
    cylinder(h=11,d=11);

}
//END OF BACK PLATE

/*

// DISPLAY OPENING PLUG
color("red")
translate([-165/2,-17/2,84-25])
cube([165,17,25]);

color("pink")
difference() {
    translate([-110,-8,59])
    cube([220,16,4]);
    
    // Mounting holes
    translate([-100,0,58])
    cylinder(h=10,r=2.5);
    translate([100,0,58])
    cylinder(h=10,r=2.5);
}
// END OF DISPLAY OPENING PLUG

*/



//BOTTOM PLATE
color("yellow")
translate([-100,11,0])
cube([200,4,59]);

//DISPLAY MOUNTING PLATE
difference() {
    color("gray")
    translate([-110,-10,58])
    cube([220,25,3]);

    // Mounting holes
    translate([-100,0,48])
    cylinder(h=20,r=2.5);
    translate([100,0,48])
    cylinder(h=20,r=2.5);


}

// case depth = 90mm