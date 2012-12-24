codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

mu=398600.4418;
sats=list;
gravforce=ForceField;
gval=6.6742/1E11;
emass=5.9736E24;
gravforce.AddCentralGravity(point(0,0,0),mu);
gravforce.SetAccuracy(100000000,10);

function addsat(dst,spd,ang1,ang2)
{
   sat=satsetframe.addsubframe("sframe");
   sats.add(sat);
   sat.custom.active=true;
   
   mt=MotionForceField.create(sat,"Motion");
   mt.ForceField=gravforce;
   sat.Transf.origin=point(dst,0,0);
   sat.velocity=vecrotate(@vector(radial2point(spd,ang1,ang2)),vector(0,1,0),Pi/2);
   sat.velocity=sat.velocity+vector(0,0.46,0);
   
   sat.motion=mt;
   sat.add("Sphere","Radius":200,"resolution":6,"Color":color(0.8,0.3,0),"SpecularColor":color(0.51,0.51,0.51),"SpecularValue":30);
   
   orbit=satsetframe.add("Curve","Color":color(1,0.5,0,0.5),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   orbit.track(sat,100);
   
   sf=sat.add("Arrow","Name":"Arrow1","Position":point(0,0,10),"Axis":vector(2000,0,0),
      "NormAxis":vector(0,0,1),"Width":200,"Thickness":0,"EnableLight":false,"Color":color(0,0.7,0,0.7),
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);

   sf=sat.add("Arrow","Name":"Arrow2","Position":point(0,0,-10),"Axis":vector(2000,0,0),
      "NormAxis":vector(0,0,1),"Width":200,"Thickness":0,"EnableLight":false,"Color":color(0.7,0,0,0.7),
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
}


function addsatset(spd)
{
   ct2=8;   
   for i2=0 to ct2 do {
      ang2=(0.1+0.9*(i2/(ct2)))*Pi/2;
      ct1=round(9*((Pi/2-abs(ang2))/(Pi/2)),0);
      randstart=random*2*Pi;
      for i1=0 to ct1 do {
         ang1=i1/(ct1+1)*2*Pi+randstart;
         addsat(startdistance,spd,ang1,ang2);
   
      }
   }
}


ssysframe=ssys_create;
ssys_createearth(1);
ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

root.time=time(2000,6,1,0,0,0);
root.TimeSpeedFactor=800;


#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);

earthframe=GetPlanetFrame("Earth");
inclinframe=earthframe.inclin;
satsetframe=inclinframe.addsubframe("SatSetFrame");
globeframe=GetPlanetBodyFrame("Earth");


#enhanced texture on Earth
#eglobe=GetPlanetBodyFrame("Earth");
#etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_4.jpg");
#eglobe.GlobeRendering.Earth.Texture=etx2.name;


#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=7000;
vp.FarClipPlane=600000;
vp.FocalDistance=50000;
vp.cameradir=vector(-0.444062594, -0.980280787, 0.04307151);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;


earthradius=6356;
startdistance=earthradius*1.01;


menu=T_createmenu;
menu.sizex=0.4;
menu.add("",'4.0 km/s',"EQ040");
menu.add("",'6.0 km/s',"EQ060");
menu.add("",'8.0 km/s',"EQ080");
menu.add("",'9.0 km/s',"EQ090");






while true do {
   incrtime;
   
   if menu.wasmodified then {
      satsetframe.clearobjects;
      sats.clear;
      if menu.SelectID=="EQ040" then addsatset(4.0);
      if menu.SelectID=="EQ060" then addsatset(6.0);
      if menu.SelectID=="EQ080" then addsatset(8.0);
      if menu.SelectID=="EQ090" then addsatset(9.0);
   }
   
   #co-move camera with earth (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #keep planet in focal plane
   vp.FocalDistance=1.0*distance(vp.camerapos,point(0,0,0));

   foreach sat in sats do if sat.custom.active then {
      sat.Arrow1.Axis=0.4*500*sat.velocity;
      dr=vecnorm(point(0,0,0)-sat.Transf.origin);
      dst=distance(point(0,0,0),sat.Transf.origin);
      sat.Arrow2.Axis=0.4*(30000000/dst)*dr;
      if dst<earthradius then {
         sat.custom.active=false;
         sat.motion.dispose;
         sat.motionname="";
         sat.velocity=vector(0,0,0);
         sat.arrow1.dispose;
         sat.arrow2.dispose;
      }
   }
      
   render;
}
