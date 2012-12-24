
resetallscenes;

vp=all.viewports.main;
vp.cameradir=vecnorm(vector(0,-0.5,-1));
vp.camerapos=point(0,0,0)-8*vp.cameradir;
vp.cameraupdir=vector(0,1,0);
vp.enableusernavigation=true;

vp.NearClipPlane=0.01;
vp.FarClipPlane=50;
vp.FocalDistance=1;
vp.EyeSeparation=0.3;


sss=addscene;sss.name="SolarSystem";
sss.light0pos=point(-100,30,30);
vp.setscene(sss);
sss.ambientlightcolor=color(0.31,0.31,0.31);
sss.start;
sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

sf1=sss.addsubframe("sf1");

bol=sf1.add("sphere");
bol.color=color(0.7,0.6,0.4);
bol.resolution=60;
bol.radius=0.5;
bol.canbuffer=true;

#ring=sf1.addobject("surface");
#fnc=functor("point(v*cos(u),0,v*sin(u))","u","v");
#ring.generate(fnc,0,2*Pi,100,1.3,2.2,10);
#fnc=functor("point(u,v,0.1*sin(u*v))","u","v");
#ring.generate(fnc,-2,2,40,-2,2,40);
#ring.RenderBack=true;

cset=FlatContourSet;
cset.addpoint(point(1,-0.1,0));
cset.addpoint(point(2,-0.1,0));
cset.addpoint(point(2,+0.1,0));
cset.addpoint(point(1,+0.1,0));
cset.close;
cset.calcflatnormals;

rframe=sf1.addsubframe;
#rframe.transf.rotate(vector(1,0,0),Pi/2);
ring=rframe.add("SolidObject");
ring.Revolve(cset,200);

ring.color=color(1,1,0,1);
#ring.blendtype=BlendTranslucent;
#ring.DepthMask=DepthMaskDisable;

tx=rframe.createtexture("satring",DataDir+"\textures\star2.bmp");
ring.texture=tx.name;

fnc=functor("0.25*distance(pt,point(0,0,0))","pt");
ring.GenerateVertexProperty(fnc,VertexPropertyTC1);
fnc=functor("0.5","pt");
ring.GenerateVertexProperty(fnc,VertexPropertyTC2);
#ring.canbuffer=true;



root.time=time(2008,1,1,0,0,0);
#root.TimeSpeed=3;

while true do {
   incrtime;
   render;
}
