

resetallscenes;

vp=all.viewports.main;
vp.cameradir=vecnorm(vector(0,-0.5,-1));
vp.camerapos=point(0,0,0)-8*vp.cameradir;
vp.enableusernavigation=true;

sss=addscene;sss.name="SolarSystem";
sss.light0pos=point(100,30,100);
vp.setscene(sss);
sss.ambientlightcolor=color(0.3,0.3,0.3);
sss.start;
sf=sss.addsubframe("sf");

line=sf.addobject("Curve");line.makeline(point(0,0,0),point(1,0,0));line.Arrow2Size=0.15;line.size=2;
line=sf.addobject("Curve");line.makeline(point(0,0,0),point(0,1,0));line.Arrow2Size=0.15;line.size=2;
line=sf.addobject("Curve");line.makeline(point(0,0,0),point(0,0,1));line.Arrow2Size=0.15;line.size=2;

surf=sf.addobject("surface");
func=functor("radial2point(1+0.5*sin((u+v)/2),u,v)","u","v");
surf.generate(func,0-Pi,0,125,0-Pi/2,Pi/2,125);
surf.renderback=true;
surf.color=color(0.5,0.5,0.8);
surf.SpecularValue=40;
surf.SpecularColor=color(0.3,0.5,0.5);
#surf.canbuffer=true;

crv=sf.addobject("curve");
func=functor("radial2point(1.5+1*sin(u),sin(2*u),cos(u))","u");
crv.generate(func,0,2*Pi,600);
crv.curverendertype=CurveRenderSmooth;
crv.size=0.06;
crv.color=color(1,0,0);

root.time=time(2008,1,1,0,0,0);

while root.time-time(2008,1,1,3,0,0)<0 do {
   incrtime;
   render;
   ttm=objectroot.time+0;
}
   