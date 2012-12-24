codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

mu=398600.4418;
sats=list;
gravforce=ForceField;
gval=6.6742/1E11;
emass=5.9736E24;
gravforce.AddCentralGravity(point(0,0,0),mu);
gravforce.SetAccuracy(100000000,10);

function addsat(dst,spd,ang)
{
   sat=satsetframe.addsubframe("sframe");
   sats.add(sat);
   sat.custom.active=true;
   
   mt=MotionForceField.create(sat,"Motion");
   mt.ForceField=gravforce;
   sat.Transf.origin=point(dst,0,0);
   sat.velocity=spd*vector(cos(ang),sin(ang),0);
   
   sat.motion=mt;
   sat.add("Sphere","Radius":400,"Color":color(0.8,0.3,0),"SpecularColor":color(0.51,0.51,0.51),"SpecularValue":30);
   
   orbit=satsetframe.add("Curve","Color":color(1,0.5,0));
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
vp.FocalDistance=90000;
vp.cameradir=vector(-0.444062594, -0.980280787, 0.04307151);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;


startdistance=36000;
earthradius=6356;


menu=T_createmenu;
menu.sizex=0.4;
menu.add("",'Velocity size variation',"VS");
menu.add("",'Velocity direction variation (2.5 km/s)',"VD25");
menu.add("",'Velocity direction variation (4.0 km/s)',"VD40");

menu.add("",'Distance variation (2.5 km/s)',"VDST25");
menu.add("",'Distance variation (4.0 km/s)',"VDST40");

menu.add("",'Circular orbits',"CIRC");


#for i=0 to 30 do addsat(i/4);

#for i=0 to 30 do addsat(2.5,2*Pi*(i+0.5)/30);




while true do {
   incrtime;
   
   if menu.wasmodified then {
      satsetframe.clearobjects;
      sats.clear;
      if menu.SelectID=="VS" then for i=0 to 30 do addsat(startdistance,i/6,Pi/2);
      if menu.SelectID=="VD25" then for i=0 to 29 do addsat(startdistance,2.5,2*Pi*(i+0.5)/30);
      if menu.SelectID=="VD40" then for i=0 to 29 do addsat(startdistance,4,2*Pi*(i+0.5)/30);
      if menu.SelectID=="VDST25" then for i=3 to 29 do addsat(i/29*60000,2.5,Pi/2);
      if menu.SelectID=="VDST40" then for i=3 to 29 do addsat(i/29*60000,4.0,Pi/2);
      if menu.SelectID=="CIRC" then {
         for i=3 to 29 do {
            dst=i/29*60000;
            spd=sqrt(mu/dst);
            addsat(dst,spd,Pi/2);
         }
      }
   }
   
   #co-move camera with earth (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #keep planet in focal plane
   vp.FocalDistance=1.0*distance(vp.camerapos,point(0,0,0));

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
         sat.arrow1.dispose;
         sat.arrow2.dispose;
      }
   }
      
   render;
}
