au2km=149598000;
zoomf=2000;
planetorbitcolor=color(0.0,0.3,0.6,1);

function createstarback()
{
   starbackframe=root.SC.Universe.addsubframe("StarBackFrame");
   starbackframe.nearclipplane=5000*au2km;
   starbackframe.farclipplane=100000*au2km;
   tx=starbackframe.createtexture("star",DataDir+"\textures\star2.bmp");
   sg=starbackframe.add("StarGlobe");
   sg.texture=tx.name;
   sg.radius=20000*au2km;
   sg.starsize=100*au2km;
   sg.DepthTest=DepthTestDisable;
   sg.IsBackGround=true;
}

function createsun()
{
   sunframe=root.SC.Universe.SolarSystem.addsubframe("Sun");
   sunglobe=sunframe.add("LightPoint","Name":"Object");
   sunglobe.color=color(1,1,0.25);
   sunglobe.Size=0.2*au2km;
   sunsub=sunframe.addviewdirframe(point(0,0,0));
   sunsub.EnableLight=false;
}

function createluna()
{
   earthframe=root.SC.Universe.SolarSystem.Earth;

   lunaframe=earthframe.addsubframe("Luna");
   tx=lunaframe.createtexture("luna",DataDir+"\textures\moon.jpg");

   lunaglobe=lunaframe.add("sphere","Name":"Luna");
   lunaframe.transf.origin=point(0.2*au2km,0,0);
   lunaglobe.radius=1737.4*zoomf;
   lunaglobe.texture=tx.name;
   lunaglobe.canbuffer=true;
   
   sf=lunaframe.addviewdirframe(point(0,0,0));
   sf.addbar(point(0,0,0),5000*zoomf,3000*zoomf,3000*zoomf);
   lunaframe.addflattext("Moon",point(0,0,2000*zoomf),2000*zoomf);
   
   mt2=MotionLuna.create(lunaframe,"LunaOrbit");
   mt2.load;
   lunaframe.motion=mt2;
   
   orb=root.SC.Universe.SolarSystem.Earth.add("Curve","Name":"LunaOrbit");
   orb.color=planetorbitcolor;
}

#this function calculates the orbit of Luna at a given time
function calclunaorbit(tm,resol)
{
   MotionLuna=root.SC.Universe.SolarSystem.Earth.Luna.Motions.LunaOrbit;
   LunaOrbit=root.SC.Universe.SolarSystem.Earth.LunaOrbit;
   MotionLuna.CreateCurve(LunaOrbit,tm,resol);
}

function createearth()
{
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

   earthaxis=globerenderframe.add("cylinder","Name":"EarthAxis");
   earthaxis.radius=6378.0*zoomf/20;
   earthaxis.Position=point(0,0,-1.5*6378.0*zoomf);
   earthaxis.Axis=3*6378.0*zoomf*vector(0,0,1);
   
   
   mtr=MotionRotate.create(globeframe);
   mtr.normdir=vector(0,0,1);
   mtr.RotSpeed=2*Pi/(24*60*60);
   mtr.TimeType=ST0;
   mtr.offsetangle=0;
   globeframe.motion=mtr;
}


function createplanet(pname)
{
   planframe=root.SC.Universe.SolarSystem.addsubframe(pname);
   
   orb=root.SC.Universe.SolarSystem.add("Curve","Name":(pname+"Orbit"));
   orb.color=planetorbitcolor;
   orb.Size=2;
   #orb.CurveRenderType=CurveRenderSmooth;orb.Size=zoomf*2000;
   
   mt2=MotionCyclOrbit.create(planframe,pname+"Orbit");
   mt2.loadfile(pname,au2km,0.00001);
   planframe.motion=mt2;
   mt2.CreateCurve(orb,root.time,200);
   
   inclinframe=planframe.addsubframe("Inclin");
   inclinframe.Transf.rotate(vector(1,0,0),deg2rad(-23.4457889));
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
   mtr.RotSpeed=2*Pi/(24*60*60);
   mtr.TimeType=ST0;
   mtr.offsetangle=0;
   globeframe.motion=mtr;
}



function createsolarsystem()
{
   resetallscenes;
   resetallviewports;
   
   displayname=root.displays.getmembers.get(0).name;
   
   vp=addviewport(0,0,1,1,displayname,displayname);
   vp.name="main";
   vp.start;
   
   vp.cameradir=vecnorm(vector(0,-1,-0.5));
   vp.camerapos=point(0,0,0)-20000*zoomf*5*vp.cameradir;
   vp.cameraupdir=vector(0,0,1);
   vp.enableusernavigation=true;
   vp.NearClipPlane=500*zoomf*5;
   vp.FarClipPlane=350000*zoomf*5;
   vp.FocalDistance=10000*zoomf*5;
   vp.EyeSeparation=600*zoomf*5;
   sc=addscene;sc.name="SC";
   sc.light0pos=point(10,20,30);
   vp.setscene(sc);
   scene.ambientlightcolor=color(0.1,0.1,0.1);
   scene.start;
   universe=sc.addsubframe("Universe");
   
   universe.AddSubFrame("SolarSystem");
   
   if true then {
      createplanet("Mercurius");
      createplanet("Venus");
      createplanet("Mars");
      createplanet("Jupiter");
      createplanet("Saturnus");
      createplanet("Uranus");
      createplanet("Neptunus");
   }
}

function getplanetorbit(name)
{
   return(root.SC.Universe.SolarSystem.get(name+"Orbit"));
}

#/////////////////////////////////////////////////////////////////////////
# MAIN
#/////////////////////////////////////////////////////////////////////////


root.time=time(1999,8,11,0,0,0);
root.timespeed=2*60;
root.timespeed=2*60*60;


planets=list;
createsolarsystem;
createstarback;
createsun;
createearth;
createluna;


#viewport frame
vpframe=root.SC.universe.addscreenframe("vpframe");
vpframe.EnableLight=false;
rc=vpframe.add("rectangle");
rc.color=color(0.5,0.5,0.5,0.5);
rc.BlendType=BlendTransparent;
rc.axis1=vector(0.25,0,0);
rc.axis2=vector(0,0.25,0);

tx=vpframe.add("Text3D");
tx.position=point(0,0.1,0);
tx.content="Dit is een test";
tx.unitx=vector(0.03,0,0);
tx.unity=vector(0,0.03,0);

#enable shadow
sunradius=695500;
#root.SC.VolumeShadowAdd(0.99*sunradius,color(0,0,0,0.35),10*zoomf,700000*zoomf);
#root.SC.VolumeShadowAdd(-1*sunradius,color(0,0,0,0.75),10*zoomf,500000*zoomf);


aaa=planets(0);
#break;

#create object on earth
#break;
earthglobe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe;
obj=earthglobe.addsubframe("obj");
obj.Transf.rotate(vector(0,1,0),-1*Pi/4);
obj.Transf.translate(vector(6378.0*zoomf,0,0));
obj.Transf.rotate(vector(0,1,0),1.1*Pi/2);
ttf1=obj.transf;
#break;
sokkel=obj.add("bar");
sokkel.color=color(1,1,0.5);
sokkel.SizeX=4000*zoomf;
sokkel.SizeY=4000*zoomf;
sokkel.SizeZ=600*zoomf;
sokkel.Position=point(-0.5*sokkel.SizeX,-0.5*sokkel.SizeY,-0.5*sokkel.SizeZ);

cl=obj.add("cylinder");
cl.Radius=100*zoomf;
cl.Axis=2000*zoomf*vector(0,0,1);


#root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.visible=false;

getplanetorbit("Earth").color=color(0.5,0,0.5);
#break;

calclunaorbit(root.time,200);

createvar(tf1);
createvar(tf2);
createvar(tf3);
createvar(lpt);

#tf1=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.obj.transf;
#tf2=tf1;tf2.invert;
#tf3=tf2*tf1;
#break;

root.viewports.main.XMaxFrac=1.0;
root.viewports.main.YMaxFrac=1.0;

for i=1 to 10000 do {
   incrtime;

   if false then {
      if i-100*floor(i/100)==50 then {
         vp1=root.viewports.main;
         vp1.YMaxFrac=0.49;
         vp2=addviewport(0,0.5,1,1,vp1.LeftDisplay,vp1.RightDisplay);
         vp2.name="main2";
         vp2.start;
         vp2.setscene(root.SC);
         vp2.cameradir=vecnorm(vector(0,-1,-2.5));
         vp2.camerapos=point(0,0,0)-40000*zoomf*5*vp2.cameradir;
         vp2.cameraupdir=vector(0,0,1);
         vp2.enableusernavigation=true;
         vp2.NearClipPlane=500*zoomf*5;
         vp2.FarClipPlane=150000*zoomf*5;
         vp2.FocalDistance=10000*zoomf*5;
         vp2.EyeSeparation=20*zoomf*5;
      }
   
      if i-100*floor(i/100)==99 then {
         root.viewports.main2.dispose;
         vp1=root.viewports.main;
         vp1.YMaxFrac=1.0;
      }
   }
   
   
   #mechanism to co-move camera with earth
   if true then {
      root.SC.Universe.transf.reset;
#      tf1=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.totaltransformation;
      tf1=root.SC.Universe.SolarSystem.Earth.totaltransformation;
#      tf1=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.obj.totaltransformation;
      tf2=tf1;tf2.invert;
      #root.viewports.main.transf=tf2;
      root.sc.universe.transf=tf2;
      root.sc.light0pos=tf2*point(0,0,0);
   }
   
   
   render;
}
