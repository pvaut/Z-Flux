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
vp.cameradir=vecnorm(vector(0,-100,-1));
vp.camerapos=point(0,0,0)-4*au2km*vp.cameradir;
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
//scene.VolumeShadowAdd(1,color(1,0,0),0.01*au2km,1*au2km);
ssys=scene.addsubframe("SolarSystem");

sunglobe=ssys.addobject("LightPoint","Sun");
sunglobe.color=color(1,1,0.25);
sunglobe.Size=0.1*au2km;
ln=ssys.addobject("Curve","SunPolarAxis");
ln.color=color(0.5,0.5,0.5);
ln.makeline(point(0,0,-1*au2km),point(0,0,+1*au2km));


//Venus
venusframe=ssys.addsubframe("Venus");
venusframe.transf.origin=point(au2km*1,0,0);
mt=KeplerMotion.create(venusframe);
venusframe.motion=mt;
mt.PeriTime=time(1999,12,1,12,0,0);
mt.SemiMajorAxis=0.723329*au2km;
mt.Eccentricity=0.006771;
mt.Inclination=deg2rad(3.394);
mt.AscendingNode=deg2rad(76.679);
mt.ArgPerigee=deg2rad(54.8847);
mt.Period=A2T(0.723329);

venusglobe=venusframe.addobject("sphere","Venus");
venusglobe.color=color(1,0.75,0.5);
venusglobe.radius=0.1*au2km;

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
orb.Size=0.03*au2km;
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
root.timespeed=24*60*60/48/3;

//root.universe.solarsystem.venusframe.visible=false;

orbits.get(0).color=color(0.5,0,0);

createvar(tf);

for i=1 to 10000 do {
   incrtime;
   ssys.transf.reset;
   tf=globeframe.totaltransformation;
   tf.translate(vector(0,0,0.2*au2km));
   tf.invert;
   ssys.transf=tf;
   scene.light0pos=tf*point(0,0,0);
   render;
}
   