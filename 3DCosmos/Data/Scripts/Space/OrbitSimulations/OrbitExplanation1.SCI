codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

sats=list;
gravforce=ForceField;
gval=6.6742/1E11;
emass=5.9736E24;
gravforce.AddCentralGravity(point(0,0,0),398600.4418);
gravforce.SetAccuracy(100,10000000);

function addsat(spd)
{
   sat=inclinframe.addsubframe("sframe");
   sats.add(sat);
   sat.custom.active=true;
   
   if false then {
      mt=MotionKepler.Create(sat,"Motion");
      mt.Eccentricity=e;
      mt.SemiMajorAxis=startdistance/(1+mt.Eccentricity);
      mt.period=mt.SemiMajorAxis^1.5/8684551.229;
      mt.Inclination=deg2rad(0);
      mt.AscendingNode=deg2rad(180);
      mt.ArgPerigee=deg2rad(60);
      mt.PeriTime=Root.time+(mt.period/2)*24*60*60;
   }
   mt=MotionForceField.create(sat,"Motion");
   mt.ForceField=gravforce;
   sat.Transf.origin=point(startdistance,0,0);
   sat.velocity=vector(0,spd,0);
   
   sat.motion=mt;
   sat.add("Sphere","Radius":400,"Color":color(0.8,0.3,0),"SpecularColor":color(0.51,0.51,0.51),"SpecularValue":30);
   
   orbit=inclinframe.add("Curve","Color":color(1,0.5,0));
   orbit.track(sat,100);
   
   sf=sat.add("Arrow","Name":"Arrow1","Position":point(0,0,10),"Axis":vector(2000,0,0),
      "NormAxis":vector(0,0,1),"Width":400,"Thickness":0,"EnableLight":false,"Color":color(0,0.7,0,0.7),
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);

   sf=sat.add("Arrow","Name":"Arrow2","Position":point(0,0,-10),"Axis":vector(2000,0,0),
      "NormAxis":vector(0,0,1),"Width":400,"Thickness":0,"EnableLight":false,"Color":color(0.7,0,0,0.7),
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
}

ssysframe=ssys_create;
ssys_createearth(1);
ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

root.time=time(2000,6,1,0,0,0);
root.TimeSpeedFactor=1200;


#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);

earthframe=GetPlanetFrame("Earth");
inclinframe=earthframe.inclin;
globeframe=GetPlanetBodyFrame("Earth");


#enhanced texture on Earth
#eglobe=GetPlanetBodyFrame("Earth");
#etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_4.jpg");
#eglobe.GlobeRendering.Earth.Texture=etx2.name;


#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=7000;
vp.FarClipPlane=1000000;
vp.FocalDistance=70000;
vp.cameradir=vector(-0.749367791, -0.465955086, -0.617519988);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;

startdistance=25000;
earthradius=6356;



#addsat(3);

cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);
cframe.EnabeMouseArrow(point(0.25,0.75),point(0,0),point(1.3,1),0.03);


rc1=cframe.add("FrameControl");
rc1.position=point(0,0.8);
rc1.SizeX=0.4;
rc1.SizeY=0.3;
rc1.color=color(0.2,0.2,0.2);

txt1=cframe.add("TextControl");
txt1.position=point(0.025,0.95);
txt1.Content='Speed (km/s) :';
txt1.size=0.04;


vlspeed=cframe.add("ScalarControl");
vlspeed.position=point(0.025,0.9);
vlspeed.size=0.04;
vlspeed.SizeX=0.13;vlspeed.RangeSizeX=0.2;
vlspeed.min=0;vlspeed.max=15;vlspeed.step=0.2;vlspeed.DecimalCount=1;
vlspeed.value=0;


btadd=cframe.add("ButtonControl");
btadd.content='Add';
btadd.position=point(0.025,0.83);
btadd.size=0.04;
btadd.sizex=0.2;





while true do {
   incrtime;
  
  
   if btadd.wasmodified then addsat(vlspeed.Value);
   
   #co-move camera with earth (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   foreach sat in sats do if sat.custom.active then {
      sat.Arrow1.Axis=500*sat.velocity;
      dr=vecnorm(point(0,0,0)-sat.Transf.origin);
      dst=distance(point(0,0,0),sat.Transf.origin);
      sat.Arrow2.Axis=(30000000/dst)*dr;
      if dst<earthradius then {
         sat.custom.active=false;
         sat.motion.dispose;
         sat.motionname="";
         sat.velocity=vector(0,0,0);
      }
   }
      
   render;
}
