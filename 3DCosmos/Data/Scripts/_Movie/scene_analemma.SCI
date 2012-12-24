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
dd0=0.8;dd=1.1;
txts=autokm(0.075);
ptsframe=root.SC.Universe.SolarSystem.addsubframe("PTS");
ptsframe.visible=false;


line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr1),@point(autokm(dd)*dr1));
txt=ptsframe.addviewdirframe(@point(autokm(dd*1.01)*dr1)).add("Text3D","Content":'Lente equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);

line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr1),@point(autokm(-1*dd)*dr1));
txt=ptsframe.addviewdirframe(@point(autokm(-1*dd*1.01)*dr1)).add("Text3D","Content":'Herfst equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);



line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr2),@point(autokm(dd)*dr2));
txt=ptsframe.addviewdirframe(@point(autokm(dd*1.01)*dr2)).add("Text3D","Content":'Zomer zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);

line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr2),@point(autokm(-1*dd)*dr2));
txt=ptsframe.addviewdirframe(@point(autokm(-1*dd*1.01)*dr2)).add("Text3D","Content":'Winter zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);


SetViewPosition(0,deg2rad(51));
root.time=time(2011,6,21,15,0,0);
root.TimeSpeedFactor=3000;




#=================================================================================
# ANIMATION
#=================================================================================
animdurat=Chrono;animdurat.start;
SkyFrameVisible(true);
mydata.zoomfrac=1.0;
root.Viewports.main.cameradir=vecnorm(vector(0.275581337, -0.773311005, -0.571003517));
root.Viewports.sky.cameradir=vecnorm(vector(-0.6, -0.1, -0.3));

root.time=time(2012,6,20,12,0,0);
root.TimeSpeedFactor=0;
mydata.earthoverlayframe.visible=true;
mydata.shadowframe.visible=true;

crv=CreateSunTrace("trace1",color(1,0.5,0));
crv.CurveRenderType=CurveRenderNormal;

FadeViewportsIn;
animate(2);

for i=1 to 400 do {
   root.time=root.time+24*60*60;
   root.Viewports.main.CameraDir=vecrotate(root.Viewports.main.CameraDir,vector(0,0,1),1/(365.25)*2*Pi);
   for ct=1 to 2 do AnimateStep;
}

FreezeSunTrace("trace1");
animate(2);

root.time=time(2012,6,20,14,0,0);
animate(2);
crv=CreateSunTrace("trace2",color(1,0.5,0));
Cam_RotateHor(root.Viewports.sky,-0.6*Pi/2,12);
for i=1 to 366 do {
   root.time=root.time+24*60*60;
   root.Viewports.main.CameraDir=vecrotate(root.Viewports.main.CameraDir,vector(0,0,1),1/(365.25)*2*Pi);
   for ct=1 to 2 do AnimateStep;
}
FreezeSunTrace("trace2");
animate(2);


if false then {
crv=CreateSunTrace("trace3",color(1,0.5,0));
root.time=time(2012,6,20,16,0,0);
animate(2);
for i=1 to 366 do {
   root.time=root.time+24*60*60;
   root.Viewports.main.CameraDir=vecrotate(root.Viewports.main.CameraDir,vector(0,0,1),1/(365.25)*2*Pi);
   for ct=1 to 2 do AnimateStep;
}
FreezeSunTrace("trace2");
animate(2);
}

animate(5);
FadeViewportsOut;

stop;
