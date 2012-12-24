#Solar System functions

au2km=149598000;
planetorbitcolor=color(0.0,0.3,0.6,0.6);

coseps=0.9174369381;
sineps=0.3978812030;

zoomf=1;

#to be used whenever a 'home' position is needed
HLongitude=deg2rad(ReadSetting("Longitude",0));
HLattitude=deg2rad(ReadSetting("Lattitude",51));

function HomeLongitude() { return(HLongitude); }
function HomeLattitude() { return(HLattitude); }


#converts astronomical units to km
function autokm(ival)
{
   return(149598000*ival);
}

function kmtoau(ival)
{
   return(ival/149598000);
}

#converts right ascension & declination to ecliptic angle coordinates)
function equat2eclipt(alpha,delta)
{
   lr1=coseps*sin(delta)-sineps*cos(delta)*sin(alpha);
   ecl2=asin(lr1);
   lr1=cos(alpha);
   lr2=sin(alpha)*coseps+tan(delta)*sineps;
   ecl1=angle(lr1,lr2);
   return(list(ecl1,ecl2));
}

function eclipt2equat(ecl1,ecl2,alpha,delta)
{
   xx=cos(ecl1);
   yy=sin(ecl1)*coseps-tan(ecl2)*sineps;
   alpha=angle(xx,yy);
   delta=asin(sin(ecl2)*coseps+cos(ecl2)*sineps*sin(ecl1));
}


#returns the viewport
function GetViewPort()
{
   return(root.Viewports.main);
}

#returns a list of all the planets in the Solar System
function ssys_getplanetlist()
{
   return(list("Mercurius","Venus","Earth","Mars","Jupiter","Saturnus","Neptunus","Uranus"));
}

function ssys_createstarback()
{
   starbackframe=root.SC.Universe.addsubframe("StarBackFrame");
   starbackframe.nearclipplane=5000*au2km;
   starbackframe.farclipplane=100000*au2km;
   tx=starbackframe.createtexture("star",DataDir+"\textures\star3.bmp");
   sg=starbackframe.add("StarGlobe","Name":"StarBack");
   sg.texture=tx.name;
   sg.radius=20000*au2km;
   sg.starsize=70*au2km;
   sg.DepthTest=DepthTestDisable;
   sg.RenderFront=false;
   sg.IsBackGround=true;
   sg.LineColor=color(0,0.5,1,0.5);
}

function ssys_showconstellations(status)
{
   if status then root.SC.Universe.StarBackFrame.StarBack.LineSize=60*au2km;
   else root.SC.Universe.StarBackFrame.StarBack.LineSize=0;
}


function ssys_createmilkyway()
{
   #galactic pole
   glong=179.32095/180*Pi;
   glatt=29.811954/180*Pi;
   ez=-1*vector(cos(glong)*cos(glatt),sin(glong)*cos(glatt),sin(glatt));
   #galactic center
   glong=266.14097/180*Pi;
   glatt=-5.52967943/180*Pi; 
   ex=vector(cos(glong)*cos(glatt),sin(glong)*cos(glatt),sin(glatt));
   ey=vecnorm(ez*ex);
   mwf=root.SC.Universe.StarBackFrame.addsubframe("MilkyWay");
   mwf.transf.Xaxis=-1*ex;
   mwf.transf.Yaxis=-1*ey;
   mwf.transf.Zaxis=ez;
   tx=mwf.createtexture("MilkyWay",DataDir+"\textures\milkyway.png");
   mw=mwf.add("sphere","EnableLight":false);
   mw.color=color(0.3,0.5,1,0.25);
   mw.texture=tx.name;
   mw.IsBackGround=true;
   mw.BlendType=BlendTransparent;mw.DepthMask=DepthMaskDisable;
   mw.renderback=true;mw.renderfront=false;
   mw.radius=20000*au2km;
   return(mwf);
}


function ssys_createconstellationnames()
{
   cnf=root.SC.Universe.StarBackFrame.addsubframe("ConstellationNames");
   cnf.Transf.rotate(vector(1,0,0),deg2rad(-23.4457889));
   filecontent=readtextfile(DataDir+"\AstroData\connames.txt");
   radius=20000*au2km;
   while filecontent.length>0 do {
      fileline=filecontent.split("~n");
      name=fileline.split("~t");
      ra=ToScalar(fileline.split("~t"))/180*Pi;
      dec=ToScalar(fileline.split("~t"))/180*Pi;
      cnf.add("TextFlat",
         "Position":(radial2point(radius,ra,dec)),
         "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
         "Size":(radius/20),
         "Color":color(0.5,0,0.5,0.5),"isbackground":true,
         "Content":(name));
   }
   return(cnf);
}


function ssys_createstarnames()
{
   snf=root.SC.Universe.StarBackFrame.addsubframe("StarNames");
   snf.Transf.rotate(vector(1,0,0),deg2rad(-23.4457889));
   filecontent=readtextfile(DataDir+"\AstroData\starnames.txt");
   radius=2991960000000;
   while filecontent.length>0 do {
      fileline=filecontent.split("~n");
      starname=fileline.split("~t");
      ra=ToScalar(fileline.split("~t"))/180*Pi;
      dec=ToScalar(fileline.split("~t"))/180*Pi;
      snf.add("TextFlat",
         "Position":(radial2point(radius,ra,dec)),
         "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
         "Size":(radius/40),
         "Color":color(0,0.5,0.5),"isbackground":true,
         "Content":(" "+starname));
   }
}


function ssys_getplanetinfo(name)
{
   mp=map;
#----------------------------------------------------------------------------------
   if name=="Mercurius" then {
      mp.radius=4880/2;
      mp.inclin=deg2rad(2.11);
      mp.rotperiod=58.646*24;
      mp.rotoffset=0;
      mp.flattening=1;
      ls=equat2eclipt(deg2rad(281.01),deg2rad(61.45));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   if name=="Venus" then {
      mp.radius=12104/2;
      mp.inclin=deg2rad(177.3);
      mp.rotperiod=243*24;
      mp.rotoffset=0;
      mp.flattening=1;
      ls=equat2eclipt(deg2rad(272.76),deg2rad(67.16));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   #WARNING: Earth is treated as a special case during construction
   if name=="Earth" then {
      mp.radius=6378.0;
      mp.inclin=deg2rad(23.4457889);
      mp.rotperiod=0.99726968*24;
      mp.rotoffset=0;
      mp.flattening=1;
      ls=equat2eclipt(deg2rad(0),deg2rad(90));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   if name=="Mars" then {
      mp.radius=6796/2;
      mp.inclin=deg2rad(25.19);
      ls=equat2eclipt(deg2rad(317.68143),deg2rad(52.88650));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
      mp.rotperiod=24.6229;
      mp.rotoffset=2.5;#!!!approximative!!!
   }
#----------------------------------------------------------------------------------
   if name=="Jupiter" then {
      mp.radius=142984/2;
      mp.inclin=deg2rad(3.13);
      mp.rotperiod=9.925;
      mp.rotoffset=0;
      mp.flattening=0.94;
      ls=equat2eclipt(deg2rad(268.057),deg2rad(64.496));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   if name=="Saturnus" then {
      mp.radius=120536/2;
      mp.inclin=deg2rad(26.73);
      mp.rotperiod=10+47/60;
      mp.rotoffset=0;
      mp.flattening=0.9;
      ls=equat2eclipt(deg2rad(40.589),deg2rad(83.537));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   if name=="Uranus" then {
      mp.radius=51118/2;
      mp.inclin=deg2rad(97.77);
      mp.rotperiod=17+14/60;
      mp.rotoffset=0;
      mp.flattening=1;
      ls=equat2eclipt(deg2rad(257.311),deg2rad(-15.175));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   if name=="Neptunus" then {
      mp.radius=24764;
      mp.inclin=deg2rad(28.32);
      mp.rotperiod=16+6.5/60;
      mp.rotoffset=0;
      mp.flattening=1;
      ls=equat2eclipt(deg2rad(299,25),deg2rad(42.950));
      mp.poledir=@vector(radial2point(1,ls(0),ls(1)));
   }
#----------------------------------------------------------------------------------
   return(mp);
}


#Create Solar system framework
function ssys_create()
{
   T_scene_create;
   vp=T_getviewport;
   sc=T_getscene;
   
   vp.cameradir=vecnorm(vector(0,-1,-0.5));
   vp.camerapos=point(0,0,0)-4*au2km*vp.cameradir;
   vp.cameraupdir=vector(0,0,1);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.NearClipPlane=0.5*au2km;
   vp.FarClipPlane=20*au2km;
   vp.FocalDistance=4*au2km;
   sc.light0pos=point(0,0,0);
   sc.ambientlightcolor=color(0.05,0.05,0.05);
   ssys_createstarback;
   root.SC.universe.AddSubFrame("SolarSystem");
   return(root.SC.Universe.SolarSystem);
}

function ssys_showstarback(newstatus)
{
   root.SC.Universe.StarBackFrame.visible=newstatus;
}

function ssys_createsun()
{
   sunframe=root.SC.Universe.SolarSystem.addviewdirframe(point(0,0,0),"Sun");
   suntx=sunframe.createtexture("suntx",DataDir+"\textures\star1.bmp");
   lightpoints=list;
   lightpoints.add(map("S":autokm(0.10),"C":color(1.0,0.5,0.0)));
   lightpoints.add(map("S":autokm(0.05),"C":color(1.0,1.0,0.5)));
   lightpoints.add(map("S":autokm(0.02),"C":color(1.0,1.0,1.0)));
   foreach lp in lightpoints do {
      rc=sunframe.add("Rectangle","Position":point(-1*lp.S,-1*lp.S,0),"Axis1":vector(2*lp.S,0,0),"Axis2":vector(0,2*lp.S,0));
      rc.Color=lp.C;
      rc.Texture=suntx.name;
      rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   }
}


function ssys_createearth(izoomf)
{
   zoomf=izoomf;
   earthframe=root.SC.Universe.SolarSystem.addsubframe("Earth");
   
   orb=root.SC.Universe.SolarSystem.add("Curve","Name":"EarthOrbit");
   orb.color=planetorbitcolor;
   orb.Size=2;
   
   mt2=MotionCyclOrbit.create(earthframe,"EarthOrbit");
   mt2.loadfile("earth",au2km,0.00001);
   earthframe.motion=mt2;
   mt2.CreateCurve(orb,root.time,200);
   
   inclinframe=earthframe.addsubframe("Inclin");
   inclinframe.Transf.rotate(vector(1,0,0),deg2rad(-23.4457889));
   globeframe=inclinframe.addsubframe("Globe");
   tx=globeframe.createtexture("earth",DataDir+"\textures\earth.jpg");
   
   globerenderframe=globeframe.addsubframe("GlobeRendering");
   globerenderframe.Transf.rotate(vector(0,0,1),Pi);
   
   if zoomf>0 then {
      earthglobe=globerenderframe.add("sphere","Name":"Earth");
      earthglobe.canbuffer=true;
      earthglobe.radius=6378.0*zoomf;
      earthglobe.resolution=40;
      earthglobe.texture="earth";
   }

   mtr=MotionRotate.create(globeframe);
   mtr.normdir=vector(0,0,1);
   mtr.RotSpeed=2*Pi/(24*60*60);
   mtr.TimeType=ST0;
   mtr.offsetangle=0;
   globeframe.motion=mtr;
   
   return(earthframe);
}

#Create the moon of the Earth
function ssys_createluna(izoomf)
{
   zoomf=izoomf;
   earthframe=root.SC.Universe.SolarSystem.Earth;

   lunaframe=earthframe.addsubframe("Luna");
   tx=lunaframe.createtexture("luna",DataDir+"\textures\moon.jpg");
   
   if zoomf>0 then {
   
      lunaglobe=lunaframe.add("sphere","Name":"Luna");
      lunaframe.transf.origin=point(0.2*au2km,0,0);
      lunaglobe.radius=1737.4*zoomf;
      lunaglobe.texture=tx.name;
      lunaglobe.canbuffer=true;
      lunaglobe.resolution=40;
   }
   
   mt2=MotionLuna.create(lunaframe,"LunaOrbit");
   mt2.load;
   lunaframe.motion=mt2;
   
   orb=root.SC.Universe.SolarSystem.Earth.add("Curve","Name":"LunaOrbit");
   orb.color=planetorbitcolor;
}

#this function calculates the orbit of Luna at a given time
function ssys_calclunaorbit(tm,resol)
{
   MotionLuna=root.SC.Universe.SolarSystem.Earth.Luna.Motions.LunaOrbit;
   LunaOrbit=root.SC.Universe.SolarSystem.Earth.LunaOrbit;
   MotionLuna.CreateCurve(LunaOrbit,tm,resol);
}


#Create Saturn rings
function ssys_createsaturnusrings(satframe,satradius)
{
   rframe=satframe.addsubframe("Rings");
   rframe.transf.rotate(vector(1,0,0),Pi/2);
   tx=rframe.createtexture("satring",DataDir+"\textures\saturnring.bmp");
   
   rings=list;
   rings.add(map("ps1":1.2,"ps2":1.70,"fr1":0.00,"fr2":0.6));
   rings.add(map("ps1":1.8,"ps2":2.03,"fr1":0.66,"fr2":0.8));
   rings.add(map("ps1":2.1,"ps2":2.23,"fr1":0.85,"fr2":1.0));
   
   foreach ring in rings do {
      cset=FlatContourSet;
      cset.addpoint(point(satradius*ring.ps1,-0.00001*satradius,0));
      cset.addpoint(point(satradius*ring.ps2,-0.00001*satradius,0));
      cset.addpoint(point(satradius*ring.ps2,+0.00001*satradius,0));
      cset.addpoint(point(satradius*ring.ps1,+0.00001*satradius,0));
      cset.close;cset.calcflatnormals;
      ringobj=rframe.add("SolidObject");
      ringobj.Revolve(cset,200);
      ringobj.texture=tx.name;
      ringobj.color=color(0.75,0.75,0.75);
      
      fnc=functor("(distance(pt,point(0,0,0))/"+str(satradius)+"-1.2)/0.9","pt");
      ringobj.GenerateVertexProperty(fnc,VertexPropertyTC1);
      fnc=functor("0.5","pt");
      ringobj.GenerateVertexProperty(fnc,VertexPropertyTC2);
      ringobj.canbuffer=true;
   }
}


#Creates a planet of the Solar System
function ssys_createplanet(pname,izoomf)
{
   zoomf=izoomf;
   if pname=="Earth" then {
      return(ssys_createearth(zoomf));
   }
   
   pinfo=ssys_getplanetinfo(pname);
   
   planframe=root.SC.Universe.SolarSystem.addsubframe(pname);
   
   orb=root.SC.Universe.SolarSystem.add("Curve","Name":(pname+"Orbit"));
   orb.color=planetorbitcolor;
   orb.Size=2;
   orb.BlendType=BlendTransparent;
   orb.DepthMask=DepthMaskDisable;
   
   mt2=MotionCyclOrbit.create(planframe,pname+"Orbit");
   mt2.loadfile(pname,au2km,0.00001);
   planframe.motion=mt2;
   mt2.CreateCurve(orb,root.time,200);
   
   
   inclinframe=planframe.addsubframe("Inclin");
   inclinframe.Transf.Xaxis=vecnorm(pinfo.poledir*vector(0,0,1));
   inclinframe.Transf.Yaxis=pinfo.poledir*inclinframe.Transf.Xaxis;
   inclinframe.Transf.Zaxis=pinfo.poledir;
   globeframe=inclinframe.addsubframe("Globe");
   tx=globeframe.createtexture(pname,DataDir+"\textures\"+pname+".jpg");
   
   globerenderframe=globeframe.addsubframe("GlobeRendering");
   globerenderframe.Transf.rotate(vector(0,0,1),Pi);
   
   if zoomf>0 then {
      globe=globerenderframe.add("sphere","Name":pname);
      globe.radius=pinfo.radius*zoomf;
      globe.resolution=30;
      globe.texture=pname;
      globe.canbuffer=true;
      
      if pname=="Saturnus" then ssys_createsaturnusrings(globerenderframe,pinfo.radius*zoomf);
   }
   
   mtr=MotionRotate.create(globeframe);
   mtr.normdir=vector(0,0,1);
   mtr.RotSpeed=2*Pi/(pinfo.rotperiod*60*60);
   mtr.TimeType=UT0;
   mtr.offsetangle=pinfo.rotoffset;
   globeframe.motion=mtr;
   
   return(planframe);
}



#returns the frame of the motion of a planet around the sun
function GetPlanetFrame(planetname)
{
   return(root.SC.Universe.SolarSystem.get(planetname));
}

#returns the position of a planet
function GetPlanetPosition(planetname)
{
   return(root.SC.Universe.SolarSystem.get(planetname).Transf.origin);
}


#returns the frame of the body of a planet
function GetPlanetBodyFrame(planetname)
{
   return(root.SC.Universe.SolarSystem.get(planetname).Inclin.Globe);
}

#returns the orbit object of a planet
function GetPlanetOrbit(name)
{
   return(root.SC.Universe.SolarSystem.get(name+"Orbit"));
}


#returns the frame of the motion of Luna
function GetLunaFrame()
{
   return(root.SC.Universe.SolarSystem.Earth.Luna);
}

#returns the position of Luna relative to Earth
function GetLunaPosition()
{
   return(root.SC.Universe.SolarSystem.Earth.Luna.Transf.origin);
}


function calcpass2(tm,h,alpha,delta,lattit,longit,passtype)
{
   tm.hour=0;tm.min=0;tm.sec=0;
   rs1=(tm.JD-2415020.0)/36525.0;
   th=0.276919398+100.0021359*rs1+0.000001075*rs1*rs1;
   th=th-floor(th);
   st00=th*2*Pi;
   csH=(sin(h)-sin(lattit)*sin(delta))/(cos(lattit)*cos(delta));
   if abs(csH)>1 then
   {
      if fabs(delta-lattit)<=Pi/2 then return(0); else return(-1);
   }
   HH1=acos(csH);
   
   m0=(alpha-longit-st00)/(2*Pi);m=m0;
   if passtype==1 then m=m0-HH1/(2*Pi);
   if passtype==0 then m=m0+HH1/(2*Pi);
   #else: transit
   
   while m>1 do m=m-1;
   while m<0 do m=m+1;
   tm=tm+m*24*60*60;
   return(1);
}

#sets the universe time at the moment the objectframe passes through local viewers height h, 
#passtype=1 : rising
#passtype=0 : setting
function calcpass(objectframe,h,passtype,lattit,longit)
{
   for ctr=1 to 3 do {
      x=objectframe.transf.origin.x-root.SC.Universe.SolarSystem.Earth.transf.origin.x;
      y=objectframe.transf.origin.y-root.SC.Universe.SolarSystem.Earth.transf.origin.y;
      z=objectframe.transf.origin.z-root.SC.Universe.SolarSystem.Earth.transf.origin.z;
      ecl1=angle(x,y);
      ecl2=atan(Z/sqrt(X*X+Y*Y));
      alpha=0;delta=0;
      eclipt2equat(ecl1,ecl2,ref(alpha),ref(delta));
      tm=root.time;
      if calcpass2(ref(tm),h,alpha,delta,lattit,longit,passtype)!=1 then return(false);
      root.time=tm;
   }
}
