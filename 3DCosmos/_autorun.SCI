
resetallscenes;

ExecuteScript(ScriptDir+"\test.sci");
//stop;

cs=flatcontourset;
choice=0;
if choice==0 then {
   cs.addpoint(point(1,0,0));
   cs.addpoint(point(1.5,0,0));
   cs.addpoint(point(1.25,0.5,0));
   cs.close;
   cs.calcflatnormals;
}
if choice==1 then {
   fnc=functor("point(1.5*(cos(a)+0.4*sin(2*a)),sin(a)+0.2*cos(2*a),0)","a");
   cs.generate(fnc,-1*Pi/2,Pi/2,60);
}
if choice==2 then {
   fnc=functor("point(1.5+1.5*cos(a),0.5*sin(a),0)","a");
   cs.generate(fnc,0,2*Pi,60);
}

vp=all.viewports.main;
resetallscenes;
vp.cameradir=vecnorm(vector(0,-0.5,-1));
vp.camerapos=point(0,0,0)-8*vp.cameradir;

sss=addscene;sss.name="SolarSystem";
sss.light0pos=point(100,30,100);
vp.setscene(sss);
sss.ambientlightcolor=color(0.1,0.1,0.1);
sss.start;
//sss.VolumeShadowAdd(0,color(0,0,0,0.5), 0.00015, 6.0 );

sf=sss.addsubframe("sf");
sf.createtexture("earth","C:\miscprojects\3DCosmos\3DCosmos\textures\earth.bmp");

//o1=sf.addobject("sphere");
//o1.radius=0.3;

ob=sf.addobject("SolidObject","ob");
//ob.CreateFlatShape(cs);
ob.revolve(cs,90);
ob.color=color(0.8,0.8,0.8);
//ob.texture="earth";
ob.position=point(0,-1,0);
ob.specularvalue=80;
//ob.generatevertexproperty(functor("color((p.x+2)*0.25,(2-p.x)*0.25,1)","p"),VertexpropertyColor);
//ob.generatevertexproperty(functor("(p.x+2)*0.25","p"),VertexpropertyTC1);
//ob.generatevertexproperty(functor("(p.y+2)*0.25","p"),VertexpropertyTC2);

tf=transformation.rotate(vector(1,0,1),-0.8);
ob.transform(tf);
ob.warpspiral(0.3);
ob.warpconalpinch(3);

//==
br=sf.addobject("bar");
br.position=point(0,2,0);
br.sizex=0.5;br.sizey=0.5;br.sizez=0.5;

motion1=MotionRotate.create(sf,"Rotation1");
motion1.normdir=vector(0,1,0);
motion1.rotspeed=0.015;
sf.motion=motion1;


for i=1 to 1200 do {
   incrtime;
   render;
}

stop;
