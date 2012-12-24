codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_SkyViewTools.sci");

#=================================================================================
# SETUP
#=================================================================================

mydata=map;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
InitSkyView(ref(mydata));



#equinoxes & solstices frame
poledir=root.SC.Universe.SolarSystem.Earth.Inclin.transf.Zaxis;
dr1=vecnorm(vector(0,0,1)*poledir);
dr2=vecnorm(vector(0,0,1)*dr1);
dd0=0.8;dd=1.2;
txts=autokm(0.1);
ptsframe=root.SC.Universe.SolarSystem.addsubframe("PTS");
ptsframe.visible=false;

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr1),@point(autokm(dd)*dr1));
txt=ptsframe.add("Text3D","Content":'Lente equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr1);
txt.UnitY=-1*txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr1),@point(autokm(-1*dd)*dr1));
txt=ptsframe.add("Text3D","Content":'Herfst equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr1);
txt.UnitY=txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr2),@point(autokm(dd)*dr2));
txt=ptsframe.add("Text3D","Content":'Zomer zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr2);
txt.UnitY=-1*txts*dr2;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr2),@point(autokm(-1*dd)*dr2));
txt=ptsframe.add("Text3D","Content":'Winter zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr2);
txt.UnitY=txts*dr2;
txt.UnitX=txts*vector(0,0,1);


SetViewPosition(0,deg2rad(51));
root.time=time(2011,6,27,0,0,0);
root.TimeSpeedFactor=3000;



#=================================================================================
# ANIMATION
#=================================================================================

animate(0.5);
SkyFrameAppear;
animate(50);
SkyFrameDisAppear;
animate(20);

#show special points
ptsframe.visible=true;

mydata.earthoverlayframe.visible=true;

Cam_Rotate(root.Viewports.main,vecnorm(vector(1,1,-1)),0.5);
animate(0.5);

ptsframe.visible=false;

JumpToDate(time(2011,7,21,0,0,0));
animate(0.25);

ZoomIn;
animate(0.25);
mydata.shadowframe.visible=true;

Cam_Rotate(root.Viewports.main,vecnorm(vector(1,0,-1)),0.5);
animate(0.5);

SkyFrameAppear;
CreateSunTrace("tracefall",GetColor("Red"));
animate(10);

FadeViewportOut(root.Viewports.Sky,0.6);
SetViewPosition(0,deg2rad(-60));
FadeViewportIn(root.Viewports.Sky,0.6);

animate(7);
FreezeSunTrace("tracefall");

SkyFrameDisappear;
ZoomOut;

JumpToDate(time(2011,12,21,5,0,0));

ZoomIn;
SkyFrameAppear;

CreateSunTrace("tracewinter",color(1,0.5,0));
animate(7);
DelSunTrace("tracefall");
DelSunTrace("tracewinter");


animate(4);

animate(40);

stop;