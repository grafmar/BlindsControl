pcbW=90;
pcbH=95;
pcbT=1.2;
thick=2.5;
$fs=0.5;

pcb();
bottom();
top();

module pcb() {
    color("green")
    translate([0,15,5])difference(){
        translate([-pcbW/2,0,0])cube([pcbW,pcbH,pcbT]);
        for(i=[-1,1])translate([i*(pcbW/2-3.5),5.4,0])cylinder(d=3,h=pcbT*3,center=true);
    }
}

module bottom() {
    HW=pcbW+20;
    HH=pcbH+25;
    difference(){
        union(){
            translate([0,HH/2,0])roundbox(HW,HH,,thick,5);
            translate([-pcbW/2,15,thick-0.1])cube([pcbW,12,5.1-thick]);
            // pcbHook
            translate([-pcbW/2,15+pcbH-10/2,thick-0.1])cube([pcbW,10,5.1-thick]);
            translate([-pcbW/2,15+pcbH+0.5,5])cube([pcbW,10/2-0.5,2]);
            hull(){
                translate([-pcbW/2,15+pcbH+0.5,5+1.4])cube([pcbW,10/2-0.5,0.1]);
                translate([-pcbW/2,15+pcbH-10/2,5+2])cube([pcbW,10,2]);
            }
            for(ix=[-1,1])for(iy=[-1,1])translate([ix*(HW/2-5),HH/2+iy*(HH/2-5),thick]){
                cylinder(d1=10,d2=6,h=3);
                cylinder(d=6,h=25-thick);
            }
        }
        for(i=[-1,1])translate([i*(pcbW/2-3.5),5.4+15,0])cylinder(d=2.5,h=5*3,center=true);
        for(yy=[5,HH-5])translate([0,yy,0])cylinder(d=4,h=10*3,center=true);
        for(ix=[-1,1])for(iy=[-1,1])translate([ix*(HW/2-5),HH/2+iy*(HH/2-5),thick+.1])cylinder(d=3,h=25-thick);
    }
}

module top() {
    HWb=pcbW+20; // bottom
    HHb=pcbH+25; // bottom
    HW=HWb+1;
    HH=HHb+1;
    difference(){
        translate([0,HH/2-0.5,0])roundbox(HW+2*thick,HH+2*thick,25+thick,5+thick);
        translate([0,HH/2-0.5,-0.1])roundbox(HW,HH,25.1,5+0.5);

        for(ix=[-1,1])for(iy=[-1,1])translate([ix*(HWb/2-5),HHb/2+iy*(HHb/2-5),25-.01]){
            translate([0,0,0])cylinder(d=3,h=2*thick);
            translate([0,0,0.1])cylinder(d1=3,d2=3+2*2,h=thick+.1);
        }
        
        hull()for(ix=[-1,1])for(iz=[-1,1])translate([ix*45/2,0,iz*thick+0.5])rotate([-90,0,0])cylinder(r=3,h=3*thick,center=true);
        // USB
        hull()for(iy=[-1,1])translate([HW/2,15+68+iy*11/2,5+14.5])rotate([0,90,0])cylinder(r=3,h=3*thick,center=true);
    }
}

module roundbox(x,y,z,r){
    hull() {
        for(xx=[-1,1])for(yy=[-1,1])translate([xx*(x/2-r),yy*(y/2-r),0])cylinder(r=r,h=z);
    }
}