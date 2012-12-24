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
jupmt.loadfile(ScriptFilePath+"\voy2jup_jup.pth");
jupiterframe.MotionName=jupmt.name;
root.SC.Universe.SolarSystem.JupiterOrbit.dispose;
jupiterorbit=ssysframe.add("Curve","Color":color(0,0.5,1,0.4),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
jupiterorbit.size=2;
jupiterorbit.Track(jupiterframe,0.01);

voyagermt=MotionTable.create(ssysframe,"Voyager");
#voyagermt.loadfile(datadir+"\paths\voyager2.pth");
voyagermt.loadfile(ScriptFilePath+"\voy2jup_voy2.pth");
voyager=ssysframe.addsubframe("Voyager");
voyager.MotionName=voyagermt.name;
textframe=voyager.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.0006)).color=color(1,0.5,0);
addlightpoint(textframe,autokm(0.0003)).color=color(1,1,0.5);
voyagerorbit=ssysframe.add("Curve","Color":color(1,0.5,0,0.6),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
voyagerorbit.size=2;
voyagerorbit.Track(voyager,0.01);


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

starttm=time(1979,7,5,0,0,0);;
curvedurat=15;

   
######################################################################################
# START GRAPH
######################################################################################

gsizex=myviewport.aspectratio;
gsizey=0.2;
goffsy=0.8;
goffsx=0.03;


gframe=ssysframe.addscreenframe("Graphics");
gframe.enablelight=false;
gframe.add("Rectangle","Position":point(0,1-gsizey-0.025,0),"Axis1":vector(gsizex,0,0),"Axis2":vector(0,gsizey+0.025,0),
   "Color":color(1,1,1,0.1),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   
# plot years
tm1=starttm;tm1.month=1;tm1.day=1;tm1.hour=0;tm1.min=0;tm1.sec=0;
for i=0 to 50 do {
   tm1.year=tm1.year+1;
   xps=goffsx+tm1.DiffDays(starttm)/curvedurat*gsizex;
   gframe.add("Curve","Color":color(0.5,0.5,0.5)).makeline(point(xps,1,0),point(xps,goffsy,0));
   st=str(tm1.year);
   txt=gframe.add("Text3D","Content":st,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Color":color(0.5,0.5,0.5));
   txt.Position=point(xps-0.5*0.02*txt.GetSizeX,goffsy-0.02,0);
}

# plot y scale
for i=0 to 10 do {
   spd=i*10/40;
   yps=goffsy+spd*gsizey;
   gframe.add("Curve","Color":color(0.5,0.5,0.5)).makeline(point(0,yps,0),point(gsizex,yps,0));
}   
crvdist=gframe.add("Curve","Color":color(1,0.5,0),"Size":2);



#Set camera at Jupiter position
root.time=time(1979,7,9,0,0,0);
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
   vposit=voyagermt.position;
   jposit=jupmt.position;
   xps=goffsx+root.time.DiffDays(starttm)/curvedurat*gsizex;
   if (xps>xpslast+0.0005) and (xps<gsizex) then {
      spd=voyagermt.speed.size/40;
      crvdist.addpoint(point(xps,goffsy+spd*gsizey,0));
      xpslast=xps;
   }
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