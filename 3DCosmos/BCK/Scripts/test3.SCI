cs=flatcontourset;
choice=1;
if choice==0 then {
   cs.addpoint(point(0,0,0));
   cs.addpoint(point(1,0,0));
   cs.addpoint(point(1,-1,0));
   cs.addpoint(point(2,-1,0));
   cs.addpoint(point(2,0,0));
   cs.addpoint(point(3,0,0));
   cs.addpoint(point(3,1,0));
   cs.addpoint(point(2,1,0));
   cs.addpoint(point(2,2,0));
   cs.newcontour;
   cs.addpoint(point(1.2,-0.5,0));
   cs.addpoint(point(1.2,0.8,0));
   cs.addpoint(point(2.5,0.8,0));
}
if choice==1 then {
   cs.addpoint(point(-1,-1,0));
   cs.addpoint(point(1,-1,0));
   cs.addpoint(point(1,1,0));
   cs.addpoint(point(-1,1,0));
   cs.newcontour;
   cs.addpoint(point(-0.5,-0.5,0));
   cs.addpoint(point(-0.5,0.5,0));
   cs.addpoint(point(0.5,0.5,0));
   cs.addpoint(point(0.5,-0.5,0));
}
if choice==2 then {
   fnc=functor("point((1+0.3*sin(2*a))*cos(a),(1+0.3*sin(2*a))*sin(a),0)","a");
   cs.generate(fnc,0,2*Pi,60);
//   fnc=functor("point(1.5*cos(-1*a),1.0*sin(-1*a),0)","a");
//   cs.newcontour;
//   cs.generate(fnc,0,2*Pi,40);
//   fnc=functor("point(0.3*cos(-1*a),1.2+0.3*sin(-1*a),0)","a");
//   cs.newcontour;
//   cs.generate(fnc,0,2*Pi,40);
//   fnc=functor("point(0.3*cos(-1*a),-1.2+0.3*sin(-1*a),0)","a");
//   cs.newcontour;
//   cs.generate(fnc,0,2*Pi,40);
}

vp=all.viewports.main;
resetallscenes;
//resetallmotions;

vp.cameradir=vecnorm(vector(0,-0.5,-1));
vp.camerapos=point(0,0,0)-8*vp.cameradir;

sss=addscene;sss.name="SolarSystem";
sss.light0pos=point(100,30,100);
vp.setscene(sss);
sss.ambientlightcolor=color(0.1,0.1,0.1);
sss.start;
//sss.createtexture("earth","C:\miscprojects\3DCosmos\3DCosmos\textures\earth.bmp");
//sss.VolumeShadowAdd(0,color(0,0,0,0.5), 0.00015, 6.0 );

sf=sss.addsubframe("sf");

//o1=sf.addobject("sphere");
//o1.radius=0.3;

ob=sf.addobject("SolidObject","ob");
//ob.CreateFlatShape(cs);
ob.Extrude(cs,3,true,2);
ob.color=color(0.8,0.8,0.8);
//ob.texture="earth";
ob.position=point(0,-1,0);
ob.specularvalue=80;
//ob.generatevertexproperty(functor("color((p.x+2)*0.25,(2-p.x)*0.25,1)","p"),VertexpropertyColor);
//ob.generatevertexproperty(functor("(p.x+2)*0.25","p"),VertexpropertyTC1);
//ob.generatevertexproperty(functor("(p.y+2)*0.25","p"),VertexpropertyTC2);

tf=transformation.rotate(vector(1,1,1),0.6);
//ob.transform(tf);
ob.warpspiral(0.006);
//ob.warpconalpinch(8);

br=sf.addobject("bar");
br.position=point(0,2,0);
br.sizex=0.5;br.sizey=0.5;br.sizez=0.5;

//ob.renderback=true;

//motion1=MotionRotate.create("Rotation1");
//motion1.normdir=vector(0,1,0);
//motion1.rotspeed=0.015;
//sf.motion=motion1;


for i=1 to 1200 do {
   incrtime;
   render;
}

stop;
