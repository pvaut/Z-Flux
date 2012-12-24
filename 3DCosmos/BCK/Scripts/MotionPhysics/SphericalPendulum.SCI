codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.05);
rootframe.add("sphere","radius":0.05,"position":point(1,0,0),"color":color(0.25,0.25,0.25));
rootframe.add("sphere","radius":0.05,"position":point(0,1,0),"color":color(0,1,0));
rootframe.add("sphere","radius":0.05,"position":point(0,0,1),"color":color(0,0,1));

b=rootframe.add("Sphere","radius":1,"color":color(0.2,0.3,0.4,0.5));
b.BlendType=BlendTransparent;
b.DepthMask=DepthMaskDisable;
b.RenderBack=true;

mt=MotionForceField.create(rootframe);
mt.ForceField.RestrictToSphere(point(0,0,0),1);
mt.ForceField.SetAccuracy(1,0.001);
mt.ForceField.AddParallelGravity(vector(0,-1,0));

objframe=rootframe.addsubframe("ObjFrame");
objframe.Transf.origin=point(1,0,0);
objframe.velocity=vector(0,0,0.1);
objframe.motion=mt;
sph=objframe.add("sphere","radius":0.1,"color":color(1,1,1));
crv=rootframe.add("Curve","Color":color(0.25,0.5,0.75,0.5),"Size":2);
crv.track(objframe,0.01);


incr=0.03;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeed=incr;

while true do {
   incrtime;
   render;
}
