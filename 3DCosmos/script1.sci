
root.multithreaded=false;

resetall;
vp=addviewport(0.003,0.003,0.997,0.997,"\\.\DISPLAY1","\\.\DISPLAY1");
vp.name="main";
//vp.eyeseparation=5;
//vp.focaldistance=15;
vp.aperture=deg2rad(60.0);
disp1=root.displays.get("\\.\DISPLAY1");
disp1.start;
vp.start;
vp.cameradir=vecnorm(vector(0,-0.2,1));
vp.camerapos=@point(-2*vp.cameradir);

sc=addscene;sc.name="SolarSystem";
vp.setscene(sc);
sc.start;

f1=sc.addsubframe("sf");
f1.color=color(1,1,1);

sc.createtexture("earth","C:\miscprojects\3DCosmos\3DCosmos\textures\earth.bmp");
sc.createtexture("mars","C:\miscprojects\3DCosmos\3DCosmos\textures\mars.bmp");

sp0=f1.addobject("Sphere","s");sp0.radius=0.1;
sp0.position=point(0,0,0);
sp0.texture="earth";

sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(0,0.5,0);
sp.texture="mars";
sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(0,-0.5,0);
sp.renderback=true;
sp.angle2min=deg2rad(30);sp.angle2max=deg2rad(120);

sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(0.5,0,0);
sp.color=color(1,0.5,0.5);
sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(-0.5,0,0);
sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(0,0,0.5);
sp=f1.addobject("Sphere","s");sp.radius=0.1;sp.position=point(0,0,-0.5);

dd=20;
su=f1.addobject("Surface");
fu=functor("point(u,0.1*cos(12*sqrt(u*u+v*v)),v)","u","v");
su.generate(fu,-0.5,0.5,dd,-0.5,0.5,dd);
su.position=point(0,-0.75,0);
su.texture="earth";
su.renderback=true;

//break;

for i=1 to 200 do {
   sp0.position=point(0.5*sin(i/30),0,0);
   vp.camerapos=@point(vecrotate(@vector(vp.camerapos),vector(0,1,0),0.025));
   vp.cameradir=-1*vecnorm(@vector(vp.camerapos));
   render;
}


//break;
sc.dispose;
//vp.dispose;