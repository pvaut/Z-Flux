codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.05,0.05,0.05);



rootframe=sss.Universe.addsubframe("Root");
if false then {
   rootframe.add("sphere","radius":0.1);
   rootframe.add("sphere","radius":0.1,"position":point(1,0,0),"color":color(1,0,0));
   rootframe.add("sphere","radius":0.1,"position":point(0,1,0),"color":color(0,1,0));
   rootframe.add("sphere","radius":0.1,"position":point(0,0,1),"color":color(0,0,1));
}

if true then {
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
}


if false then {
   pts=list;
   
   bz=SplineSurface;
   for i1=0 to 6 do
      for i2=0 to 4 do {
         pt=point(i1+0.5*sin(i2),i2+0.5*cos(i1),0.5*sin(i1*i2));
         bz.AddPoint(i1,i2,pt);
         pts.add(pt);
      }
         
   vl=bz.Eval(0,0.2);
   
   srf=rootframe.add("Surface","Color":color(0.7,0.6,0.5));
   srf.RenderBack=true;
   srf.SpecularValue=30;
   srf.SpecularColor=color(0.35,0.35,0.35);
   srf.GenerateSplineSurface(bz,200,200);
   srf.canbuffer=true;

   for i=0 to pts.size-1 do
      sr=rootframe.add("sphere","radius":0.05,"position":pts(i),"color":color(1,0,0));
      
}


root.time=time(2000,1,1,0,0,0);
root.TimeSpeed=10;

while true do {
   incrtime;
   render;
   st=UIGetKeyPressed;
   if st=="a" then stop;
}
