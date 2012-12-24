
st1="Dit-is-een-test";
st1.replace("-","<->");
st2="";

while st1!="" do {
   st2=st1.split("-");
}

planetorbitcolor=color(0.0,0.3,0.6,1);
orbits=list;

function A2T(a)
{
   return(360.0/(0.9856076686/(a*sqrt(a))));
}

au2km=149598000;
//au2km=1;
resetallscenes;

vp=all.viewports.main;
vp.cameradir=vecnorm(vector(0,-100,-50));
vp.camerapos=point(0,0,0)-40*au2km*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;

vp.NearClipPlane=0.01*au2km;
vp.FarClipPlane=50*au2km;
vp.FocalDistance=10*au2km;
vp.EyeSeparation=0.05*au2km;


scene=addscene;scene.name="Universe";
scene.light0pos=point(0,0,0);
vp.setscene(scene);
scene.ambientlightcolor=color(0.1,0.1,0.1);
scene.start;
ssys=scene.addsubframe("SolarSystem");

sunglobe=ssys.addobject("LightPoint","Sun");
sunglobe.color=color(1,1,0.25);
sunglobe.Size=0.1*au2km;
ln=ssys.addobject("Curve","SunPolarAxis");
ln.color=color(0.5,0.5,0.5);
ln.makeline(point(0,0,-1*au2km),point(0,0,+1*au2km));


//Saturnus
saturnusframe=ssys.addsubframe("Saturnus");
saturnusframe.transf.origin=point(au2km*1,0,0);
mt0=MotionTable.create(saturnusframe);
mt0.loadfile(datadir+"\paths\saturnus.pth");
mt0.InterpolType=InterpolQuad;
saturnusframe.motion=mt0;
saturnusglobe=saturnusframe.addobject("sphere","Saturnus");
saturnusglobe.color=color(1,0.75,0.5);
saturnusglobe.radius=0.3*au2km;

//Saturnus curve
orb=ssys.addobject("Curve","SaturnusOrbit");
orb.color=planetorbitcolor;
orb.CurveRenderType=CurveRenderSmooth;
orb.Size=0.2*au2km;
mt0.CreateCurve(orb,200,time(1980,1,1,0,0,0),time(2009,1,1,0,0,0));
ssys.SaturnusOrbit.IsClosed=true;



//Earth
tx=ssys.createtexture("earth",DataDir+"\textures\earth.bmp");
earthframe=ssys.addsubframe("Earth");
earthframe.transf.origin=point(au2km*1,0,0);
mt=KeplerMotion.create(earthframe);
earthframe.motion=mt;
mt.PeriTime=time(2000,1,4,12,0,0);
mt.SemiMajorAxis=1.000001018*au2km;
mt.Eccentricity=0.016708;
mt.Inclination=deg2rad(0.0);
mt.AscendingNode=deg2rad(0.0);
mt.ArgPerigee=deg2rad(102.937);
mt.Period=365.2524;

orb=ssys.addobject("Curve","EarthOrbit");
orb.color=planetorbitcolor;
orb.CurveRenderType=CurveRenderSmooth;
orb.Size=0.2*au2km;
mt.CreateCurve(orb,200);
//orb.transform(transformation.scale(1.5));
orbits.set(0,orb);


inclinframe=earthframe.addsubframe("Inclin");
inclinframe.Transf.rotate(vector(1,0,0),deg2rad(23.4457889));
globeframe=inclinframe.addsubframe("Globe");
earthglobe=globeframe.addobject("sphere","Earth");
//earthglobe.color=color(0.5,0.75,1);
earthglobe.radius=0.3*au2km;
earthglobe.texture="earth";
mtr=MotionRotate.create(globeframe);
mtr.normdir=vector(0,0,1);
mtr.RotSpeed=2*Pi/(24*60*60);
mtr.TimeType=ST0;
mtr.offsetangle=Pi;
globeframe.motion=mtr;



root.time=time(2000,1,1,12,0,0);
root.timespeed=24*60*60;

//root.universe.solarsystem.venusframe.visible=false;

orbits.get(0).color=color(0.5,0,0);

//delobject(root.universe.solarsystem.saturnusorbit);


for i=1 to 10000 do {
   incrtime;
//   earthframe.transf.origin=mt.position;
   render;
}
   