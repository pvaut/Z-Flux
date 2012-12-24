codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\Astronomy\_SUB\LoadPlanetMoonsKepler.sci");

au2km=149598000;


JID=0;

ssysframe=ssys_create;

showconstellations=true;
ssys_showconstellations(showconstellations);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
ssys_createmilkyway;
ssys_createsun;


pttx=ssysframe.createtexture("point",DataDir+"\textures\star1.bmp");
function addlightpoint(frame,sz)
{
   sz=sz/2;
   rc=frame.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0));
   rc.Texture=pttx.name;
   rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   return(rc);
}


jupiterframe=ssys_createplanet("Jupiter",1);
jupmt=MotionTable.create(ssysframe,"JupiterEnhanced");
jupmt.loadfile(ScriptFilePath+"\ulyss_jup_jup.pth");
jupiterframe.MotionName=jupmt.name;
root.SC.Universe.SolarSystem.JupiterOrbit.dispose;
jupiterorbit=ssysframe.add("Curve","Color":color(0,0.5,1,0.4),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
jupiterorbit.size=2;
jupiterorbit.Track(jupiterframe,0.01);

craftmt=MotionTable.create(ssysframe,"craft");
craftmt.loadfile(ScriptFilePath+"\ulyss_jup_ulyss.pth");
craft=ssysframe.addsubframe("craft");
craft.MotionName=craftmt.name;
textframe=craft.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.0006)).color=color(1,0.5,0);
addlightpoint(textframe,autokm(0.0003)).color=color(1,1,0.5);
craftorbit=ssysframe.add("Curve","Color":color(1,0.5,0,0.6),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
craftorbit.size=2;
craftorbit.Track(craft,0.01);


######################################################################################
# LOAD JUPITER MOONS
######################################################################################

LoadPlanetMoonsKepler(jupiterframe,datadir+"\astrodata\jupiter_moons.txt");

moons=jupiterframe.inclin.moonsframe.getmembers;
moonorbits=jupiterframe.inclin.moonorbitsframe.getmembers;
for i=4 to moons.size-1 do {
   moons(i).visible=false;
   moonorbits(i).visible=false;
}
for i=0 to 3 do {
   moonorbits(i).size=1;
   moonorbits(i).color=color(0.1,0.3,0.1);
}



######################################################################################
# START VIEWPORT
######################################################################################

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.015);
myviewport.FarClipPlane=autokm(3);
myviewport.FocalDistance=autokm(0.06);
myviewport.enableusernavigation=true;

myviewport.camerapos=point(0,0,0)+myviewport.FocalDistance*vecnorm(vector(2,1,1));
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);

clockframe=ssysframe.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.065,0.065,0),"Size":0.04,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);

starttm=time(1992,2,4,0,0,0);
curvedurat=15;

   


#Set camera at Jupiter position
root.time=time(1992,2,8,0,0,0);
root.sc.universe.transf.reset;
ttf=jupiterframe.totaltransformation;ttf.invert;
root.sc.universe.transf=ttf;
root.sc.light0pos=ttf*point(0,0,0);

root.time=starttm;
root.TimeSpeedFactor=50000;
tmlast=root.time;


xpslast=-1;
while true do {
   incrtime;
   vposit=craftmt.position;
   jposit=jupmt.position;
   if root.time.day>tmlast.day then {
      ssysframe.add("Curve","Color":color(1,0.5,0),"Size":2).makeline(vposit-vector(0,0,100000),vposit+vector(0,0,100000));
      st=str(root.time.day);
      textframe=ssysframe.addviewdirframe(vposit-vector(0,0,200000));
      txt=textframe.add("TextFlat","Position":point(-30000,0,0),"Content":st,
         "Size":100000,"Color":color(1,0.5,0));

      ssysframe.add("Curve","Color":color(0,0.5,1),"Size":2).makeline(jposit-vector(0,0,100000),jposit+vector(0,0,100000));
      st=str(root.time.day);
      textframe=ssysframe.addviewdirframe(jposit-vector(0,0,200000));
      txt=textframe.add("TextFlat","Position":point(-30000,0,0),"Content":st,
         "Size":100000,"Color":color(0,0.5,1));

      tmlast=root.time;
   }
   if RightMouseClicked then stop;
   render;
}