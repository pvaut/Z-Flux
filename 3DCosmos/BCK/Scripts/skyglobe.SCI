
codeblock readtextfile(ScriptDir+"\common.sci");
c=test(5,3);

resetallscenes;

vp=all.viewports.main;
vp.cameradir=vecnorm(vector(0,-0.5,-1));
vp.camerapos=point(0,0,0)-8*vp.cameradir;
vp.enableusernavigation=true;


sss=addscene;sss.name="SolarSystem";
sss.light0pos=point(100,30,100);
vp.setscene(sss);
sss.ambientlightcolor=color(0.1,0.1,0.1);
sss.start;


starglobeframe=sss.addsubframe("StarGlobeFrame");
starglobeframe.nearclipplane=1000;
starglobeframe.farclipplane=30000;
tx=starglobeframe.createtexture("star",DataDir+"\textures\star2.bmp");
sg=starglobeframe.add("StarGlobe");
sg.texture="star";
sg.radius=10000;
sg.starsize=50;
sg.DepthTest=DepthTestDisable;
sg.IsBackGround=true;
#sg.CanCache=true;

sf=sss.addsubframe("sf");

sf.addclipplane(plane(0,1,0,0.05));


s1=sf.add("cylinder");
s1.axis=vector(0,0.02,0);
s1.radius=0.99*2;
s1.resolution=50;


bol=sf.add("sphere");
bol.color=color(0.5,0.5,1,0.3);
bol.radius=2;
bol.resolution=30;
bol.blendtype=BlendTransLucent;
bol.RenderBack=true;

crv=sf.add("Curve");
crv.makecircle(point(0,0,0),vector(1,0,0),2.005,100);
crv.Size=0.02;
crv.CurveRenderType=CurveRenderSmooth;
crv.color=color(1,0.5,0.5);

crv=sf.add("Curve");
crv.makecircle(point(0,0,0),vector(0,0,1),2.005,100);
crv.Size=0.02;
crv.CurveRenderType=CurveRenderRibbon;
crv.color=color(0.5,1,0.5);


root.time=time(2008,1,1,0,0,0);

while root.time-time(2008,1,1,3,0,0)<0 do {
   starglobeframe.Transf.rotate(vector(-1,-2,1),0.001);
   sf.Transf.rotate(vector(1,1,1),0.006);
   incrtime;
   render;
}
   