codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.1,"color":color(0.25,0.5,0.25));

mt=MotionForceField.create(rootframe);
mt.ForceField.SetAccuracy(1,0.001);
mt.ForceField.AddElectricDipole(point(0,0,0),vector(0,0.02,0));

ct=9;
sizef=2;

for ix=0 to ct do {
   for iy=0 to ct do {
      objframe=rootframe.addsubframe("ObjFrame_"+Str(ix)+"_"+str(iy));
      objframe.charge=1.0;
      objframe.Transf.origin=point(sizef*(ix-ct/2)/ct,sizef*(iy-ct/2)/ct,2);
      objframe.velocity=vector(0,0,-0.5);
      objframe.motion=mt;
      sph=objframe.add("sphere","radius":0.025,"color":color(1,1,1),"Resolution":10);
      crv=rootframe.add("Curve",
         "Color":color(0.25,0.5,0.75,0.5),
         "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
         "Size":1);
      crv.track(objframe,0.1);
   }
}


incr=2;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeedFactor=incr;

while true do {
   incrtime;
   render;
}
