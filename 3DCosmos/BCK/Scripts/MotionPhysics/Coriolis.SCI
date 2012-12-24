codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

refframe=rootframe.addsubframe("refframe");
rmt=MotionRotate.create(refframe,"rot");
rmt.NormDir=vector(0,1,0);
rmt.RotSpeed=0.1;
refframe.motion=rmt;


mt=MotionForceField.create(refframe);

refframe.add("sphere","radius":0.05);
refframe.add("sphere","radius":0.05,"position":point(1,0,0),"color":color(0.25,0.25,0.25));
refframe.add("sphere","radius":0.05,"position":point(0,1,0),"color":color(0,1,0));
refframe.add("sphere","radius":0.05,"position":point(0,0,1),"color":color(0,0,1));

b=refframe.add("Sphere","radius":1,"color":color(0.2,0.3,0.4,0.5));
b.BlendType=BlendTransparent;
b.DepthMask=DepthMaskDisable;
b.RenderBack=true;


mt.ForceField.RestrictToSphere(point(0,0,0),1);
mt.ForceField.SetAccuracy(1,0.001);
mt.ForceField.AddCentrifugal(point(0,0,0),vector(0,0.1,0));
mt.ForceField.AddCoriolis(point(0,0,0),vector(0,0.1,0));

objframe=refframe.addsubframe("ObjFrame");
objframe.Transf.origin=@point(vecnorm(vector(1,1,1)));
objframe.velocity=vector(0.05,0.05,0);
objframe.motion=mt;
sph=objframe.add("sphere","radius":0.1,"color":color(1,1,1));
crv=refframe.add("Curve","Color":color(0.25,0.5,0.75,0.5),"Size":2);
crv.track(objframe,0.01);

crv2=rootframe.add("Curve","Color":color(0.75,0.5,0.25,0.5),"Size":2);
crv2.track(objframe,0.01);


incr=0.01;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeed=incr;

while true do {
   incrtime;
   render;
}
