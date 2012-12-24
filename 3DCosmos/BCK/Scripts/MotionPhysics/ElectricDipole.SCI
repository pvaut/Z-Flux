codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.1);
rootframe.add("sphere","radius":0.1,"position":point(1,0,0),"color":color(0.25,0.25,0.25));
rootframe.add("sphere","radius":0.1,"position":point(0,1,0),"color":color(0,1,0));
rootframe.add("sphere","radius":0.1,"position":point(0,0,1),"color":color(0,0,1));

mt=MotionForceField.create(rootframe);
mt.ForceField.SetAccuracy(1,0.001);
#mt.ForceField.AddCentralGravity(point(0,0,0),0.2);
#mt.ForceField.AddCentralGravity(point(0,1,0),0.2);
mt.ForceField.AddElectricDipole(point(0,0,0),vector(0,0.01,0));

for ix=0 to 9 do {
   for iy=0 to 9 do {
      objframe=rootframe.addsubframe("ObjFrame_"+Str(ix)+"_"+str(iy));
      objframe.charge=1.0;
      objframe.Transf.origin=point((ix-4.5)/2,(iy-4.5)/2,2);
      objframe.velocity=vector(0,0,-0.5);
      objframe.motion=mt;
      sph=objframe.add("sphere","radius":0.025,"color":color(1,1,1),"Resolution":10);
      crv=rootframe.add("Curve",
         "Color":color(0.25,0.5,0.75,0.5),
         "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
         "Size":1);
#      crv.CurveRenderType=CurveRenderDash;
      crv.track(objframe,0.1);
   }
}


incr=0.02;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeed=incr;

while true do {
   incrtime;
   render;
}
