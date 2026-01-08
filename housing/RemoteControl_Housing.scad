NumPadWidth=46;
NumPadHeight=57;
NumPadRadius=7/2;
NumPadDispalyDistance=5.5;
DisplayHeight=7;
DisplayWidth=24;
DisplayOuterHeight=13;
DisplayOuterWidth=40;
Thickness=2;

NumPadFrontThickness=3;
DisplayFrontThickness=1;

PcbWidth=56;
PcbHeight=82.5;

HousingInnerHeight=18-3;

UsbWidth=12;
UsbHeight=10;
UsbRadius=1.5;
UsbTopOffset=4+3;

ScrewDia=2.5;
ScrewHeadDia=4.5;
ScrewHeadDepth=1.5;

ScrewOffsetX=46;
ScrewOffsetY=59;


//lid();
lid2();
//housing();

//translate([0,2.5,10])cube([NumPadWidth,NumPadHeight,2*HousingInnerHeight],center=true);

module housing(){
    difference(){
        hull()for(ix=[-1,1])for(iy=[-1,1])translate([ix*(PcbWidth+1)/2,iy*(PcbHeight+1)/2,0])cylinder(r=Thickness,h=HousingInnerHeight+NumPadFrontThickness+Thickness,$fn=30);
            
        // Inner
        cube([PcbWidth+1,PcbHeight+1,2*(HousingInnerHeight+Thickness)],center=true);
        
        // NumPad
        hull()for(ix=[-1,1])for(iy=[-1,1])translate([ix*(NumPadWidth-2*NumPadRadius+1)/2,iy*(NumPadHeight-2*NumPadRadius+1)/2+2.5,0])cylinder(r=NumPadRadius,h=2*HousingInnerHeight,$fn=30);
            
        // ScrewHoles
        for(ix=[-1,1])for(iy=[-1,1])translate([ix*(ScrewOffsetX)/2,iy*(ScrewOffsetY)/2+2.5,0]){
            cylinder(r=ScrewDia/2,h=2*HousingInnerHeight,$fn=30);
            translate([0,0,HousingInnerHeight+Thickness+NumPadFrontThickness-ScrewHeadDepth])cylinder(r=ScrewHeadDia/2,h=2*HousingInnerHeight,$fn=30);
        }
        
        // Display
        translate([0,-NumPadHeight/2-DisplayHeight/2+2.5-NumPadDispalyDistance,0])cube([DisplayWidth,DisplayHeight,2*(HousingInnerHeight+Thickness+NumPadFrontThickness+1)],center=true);
        translate([0,-NumPadHeight/2-DisplayOuterHeight/2+2.5-3,0])cube([DisplayOuterWidth,DisplayOuterHeight,2*(HousingInnerHeight+Thickness+NumPadFrontThickness-DisplayFrontThickness)],center=true);
        
        // USB
        hull()for(ix=[-1,1])for(iz=[-1,1])translate([ix*(UsbWidth-2*UsbRadius)/2,PcbHeight/2-2*Thickness,Thickness+HousingInnerHeight+3-UsbTopOffset+iz*(UsbHeight-2*UsbRadius)/2])%rotate([-90,0,0])cylinder(d=UsbRadius,h=5*Thickness,$fn=15);
        
        // LidSnap
        snap();
    }
}

module snap() {
    for(iy=[-1,1]) translate([0,iy*PcbHeight/2,Thickness/2]) hull()for(ix=[-1,1])translate([ix*5/2,0,0])sphere(d=Thickness,$fn=20);

    for(ix=[-1,1])for(iy=[-1,1]) translate([ix*PcbWidth/2,iy*PcbHeight/4,Thickness/2]) hull()for(iyy=[-1,1])translate([0,iyy*5/2,0])sphere(d=Thickness,$fn=20);
}

module lid() {
    translate([0,0,Thickness/2])cube([PcbWidth,PcbHeight,Thickness],center=true);
    snap();
}

module lid2() {
    lid();
    translate([0,0,-Thickness])hull()for(ix=[-1,1])for(iy=[-1,1])translate([ix*(PcbWidth+1)/2,iy*(PcbHeight+1)/2,0])cylinder(r=Thickness,h=Thickness,$fn=30);

}
