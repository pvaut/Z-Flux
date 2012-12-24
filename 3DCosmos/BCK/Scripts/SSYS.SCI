#Solar System functions

au2km=149598000;
planetorbitcolor=color(0.0,0.3,0.6,1);

coseps=0.9174369381;
sineps=0.3978812030;

zoomf=1;
#zoomf=200;

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
#   sg.LineSize=60*au2km;
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

#Create Solar system framework
function ssys_create()
{
   resetallscenes;
   resetallviewports;
   
   displayname=root.displays.getmembers.get(0).name;
   
   vp=addviewport(0,0,1,1,displayname,displayname);
   vp.name="main";
   vp.usestereo=false;
   vp.framesize=0;
   vp.start;
   
   vp.cameradir=vecnorm(vector(0,-1,-0.5));
   vp.camerapos=point(0,0,0)-4*au2km*vp.cameradir;
   vp.cameraupdir=vector(0,0,1);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.NearClipPlane=0.5*au2km;
   vp.FarClipPlane=20*au2km;
   vp.FocalDistance=4*au2km;
   vp.EyeSeparation=0.5*au2km;
   sc=addscene;sc.name="SC";
   sc.light0pos=point(0,0,0);
   vp.setscene(sc);
   scene.ambientlightcolor=color(0.1,0.1,0.1);
   scene.start;
   universe=sc.addsubframe("Universe");
   ssys_createstarback;
   universe.AddSubFrame("SolarSystem");
   return(root.SC.Universe.SolarSystem);
}

function ssys_showstarback(newstatus)
{
   root.SC.Universe.StarBackFrame.visible=newstatus;
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
   tx=globeframe.createtexture("earth",DataDir+"\textures\earth.bmp");
   
   globerenderframe=globeframe.addsubframe("GlobeRendering");
   globerenderframe.Transf.rotate(vector(0,0,1),Pi);
   
   earthglobe=globerenderframe.add("sphere","Name":"Earth");
   earthglobe.canbuffer=true;
   earthglobe.radius=6378.0*zoomf;
   earthglobe.resolution=20;
   earthglobe.texture="earth";

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

   lunaglobe=lunaframe.add("sphere","Name":"Luna");
   lunaframe.transf.origin=point(0.2*au2km,0,0);
   lunaglobe.radius=1737.4*zoomf;
   lunaglobe.texture=tx.name;
   lunaglobe.canbuffer=true;
   
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


#Creates a planet of the Solar System
function ssys_createplanet(pname,izoomf)
{
   zoomf=izoomf;
   if pname=="Earth" then {
      return(ssys_createearth(zoomf));
   }
   planframe=root.SC.Universe.SolarSystem.addsubframe(pname);
   
   orb=root.SC.Universe.SolarSystem.add("Curve","Name":(pname+"Orbit"));
   orb.color=planetorbitcolor;
   orb.Size=2;
   
   mt2=MotionCyclOrbit.create(planframe,pname+"Orbit");
   mt2.loadfile(pname,au2km,0.00001);
   planframe.motion=mt2;
   mt2.CreateCurve(orb,root.time,200);
   
   inclin=0;
   poledir=vector(0,0.1,1);
   rotperiod=24;#sidereal period, in hours
   rotoffset=0;
   if pname=="mars" then {
      inclin=deg2rad(25.19);
      ls=equat2eclipt(deg2rad(317.68143),deg2rad(52.88650));
      poledir=@vector(radial2point(1,ls(0),ls(1)));
      rotperiod=24.6229;
      rotoffset=2.5;#!!!approximative!!!
   }
   
   inclinframe=planframe.addsubframe("Inclin");
   inclinframe.Transf.Xaxis=vecnorm(poledir*vector(0,0,1));
   inclinframe.Transf.Yaxis=poledir*inclinframe.Transf.Xaxis;
   inclinframe.Transf.Zaxis=poledir;
   globeframe=inclinframe.addsubframe("Globe");
   tx=globeframe.createtexture(pname,DataDir+"\textures\moon.jpg");
   
   globerenderframe=globeframe.addsubframe("GlobeRendering");
   globerenderframe.Transf.rotate(vector(0,0,1),Pi);
   
   globe=globerenderframe.add("sphere","Name":pname);
   globe.radius=6378.0*zoomf;
   globe.resolution=30;
   globe.texture=pname;
   globe.canbuffer=true;
   
   mtr=MotionRotate.create(globeframe);
   mtr.normdir=vector(0,0,1);
   mtr.RotSpeed=2*Pi/(rotperiod*60*60);
   mtr.TimeType=UT0;
   mtr.offsetangle=rotoffset;
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


