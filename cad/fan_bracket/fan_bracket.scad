module schraubBlock() {
	difference() {
		translate([0,0,8])
		cube(center=true,[7,7,16]);
		translate([0,0,-1])
		cylinder(h=18,r=2.2,$fn=20);
	}
}

module strebe() {
	translate([-3.5,3.4,0])
	cube([78,5,5]);
}

module Stabilisierung() {
	translate([0,5.5,7])
	cube(center=true,[7,6,6.5]);
}

module LuefterHalter() {
	union() {
	union() {
		Stabilisierung();
		schraubBlock();
	}	
	translate([71,0,0])
	union() {
		Stabilisierung();
		schraubBlock();
	}
	strebe();
	}
}

module KlemmeFeder() {
	translate([10,-5,0])
	cube([10,30,3]);
}

module Haelfte() {
		union() {
			translate([0,-8.5,0])
			LuefterHalter();
			KlemmeFeder();
	}
}

module Komplett() {
	union() {
		Haelfte();
		rotate([0,0,180])
		translate([-71,-20,0])
		Haelfte();
	}
}

Komplett();