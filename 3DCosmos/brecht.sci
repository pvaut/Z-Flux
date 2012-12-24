
//root.multithreaded=false;

resetall;
vp=addviewport(0.003,0.003,0.997,0.997,"\\.\DISPLAY1","\\.\DISPLAY1");
vp.name="main";
vp.eyeseparation=1;
//vp.focaldistance=15;
vp.aperture=deg2rad(60.0);
disp1=root.displays.get("\\.\DISPLAY1");
disp1.start;
vp.start;
vp.cameradir=vecnorm(vector(0,-0.2,1));
vp.camerapos=@point(-8*vp.cameradir);

sc=addscene;sc.name="SolarSystem";
vp.setscene(sc);
sc.start;


ct=40;
for i=0 to ct do {
   sp=sc.addobject("Sphere","cirkel"+str(i));sp.radius=0.04;
   sp.position=point(sin(i/ct*2*pi),cos(i/ct*2*pi),0);
   sp.color=color(1,1-i/ct,1-i/ct);
}

//for i=0 to 10 do {
//   sp=sc.addobject("Sphere");sp.radius=0.04;sp.position=point(0,i/10,0);
//   sp.color=color(1-i/10,1,1-i/10);
//}

for i=0 to 10 do {
   sp=sc.addobject("Sphere");sp.radius=0.04;sp.position=point(0,0,i/10*0.8);
   sp.color=color(1-i/10,1-i/10,1);
}

sp=sc.addobject("Sphere");sp.radius=2;sp.resolution=30;
sp.renderback=true;
sp.position=point(0,0,2);
sp.angle2min=Pi-Pi/10;



sp=sc.addobject("Sphere");sp.radius=0.1;sp.color=color(1,1,0);
sp.position=point(0,0,1);


render;
//break;
ct=600;
for i=1 to ct do {
//   sp0.position=point(0.5*sin(i/30),0,0);
   vp.camerapos=@point(vecrotate(0.9985*@vector(vp.camerapos),vector(0,1,0),0.02));
   vp.cameradir=-1*vecnorm(@vector(vp.camerapos));
   ang=i/3;
   vv=0.5+3*sin(ang);
   if vv<0 then vv=0;
   if vv>1 then vv=1;
   sp.color=color(vv,vv,1-vv);
   render;
}
