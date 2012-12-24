codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;
sss.ambientlightcolor=color(0.05,0.05,0.05);

rootframe=sss.Universe.addsubframe("Root");

pts=list;
pts.add(point(1,0,0));
pts.add(point(0.7,0.7,0.3));
pts.add(point(0,1,0.7));
pts.add(point(-0.7,0.7,1.3));
pts.add(point(-1,0,2));
pts.add(point(-0.7,-0.7,2.5));
pts.add(point(0,-1,1));
pts.add(point(0.7,-0.7,0.5));
bz=SplineCurve;
for i=0 to pts.size-1 do {
   sr=rootframe.add("sphere","radius":0.05,"position":pts(i),"color":color(1,0,0));
   bz.addpoint(i/(pts.size),pts(i));
}
bz.close(1);

crv=rootframe.add("curve",
   "Color":color(1,1,0.5),
   "CurveRenderType":CurveRenderSmooth,
   "Size":0.05
   ).GenerateSplineCurve(bz,200);
   
obj=rootframe.addsubframe("obj");
obj.add("sphere","radius":0.1);
mt=MotionSpline.Create(obj,"Motion");obj.motion=mt;
mt.spline=bz;
mt.timefactor=0.0001;

root.time=time(2000,1,1,0,0,0);
root.TimeSpeedFactor=1000;

while true do {
   incrtime;
   render;
   st=UIGetKeyPressed;
   if st=="a" then stop;
}
