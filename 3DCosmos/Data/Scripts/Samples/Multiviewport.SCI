codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.3,0.3,0.3);

#create secundary viewport
displayname=root.displays.getmembers.get(0).name;
vp2=addviewport(0,0.6,0.6,1.0,displayname,displayname);
vp2.name="main";
vp2.UseStereo=false;
vp2.start;

vp2.camerapos=point(0,6,10);
vp2.cameradir=vecnorm(point(0,0,0)-vp2.camerapos);
vp2.cameraupdir=vector(0,1,0);
vp2.NearClipPlane=0.5;
vp2.FarClipPlane=50;
vp2.FocalDistance=10;
vp2.setscene(sss);
vp2.EraseBackground=true;
vp2.framesize=0.01;

refframe=sss.addsubframe("refframe");

sf1=sss.addsubframe("sf1");
sf1.motion=motionrotate.create(sf1);
sf1.motion.normdir=vector(0,1,0);
sf1.motion.rotspeed=0.02;

br=sf1.add("bar");
br.color=color(1,0.5,0.25);
br.sizex=0.5;br.sizey=0.5;br.sizez=0.5;
br.position=point(-0.5*br.sizex,-0.5*br.sizey,-0.5*br.sizez);

bol2=sf1.add("sphere");
bol2.position=point(1,0,0);
bol2.color=color(0.5,1,0);
bol2.radius=0.25;
bol2.resolution=20;

lsf=sf1.addsubframe;
lsf.cancache=true;

sf2=sss.addsubframe("sf2");
sf2.motion=motionrotate.create(sf2);
sf2.motion.normdir=vector(0,1,0);
sf2.motion.rotspeed=-0.04;

br=sf2.add("bar");
br.color=color(1,0.25,0.5);
br.sizex=0.5;br.sizey=0.5;br.sizez=0.5;
br.position=point(-0.5*br.sizex,2-0.5*br.sizey,-0.5*br.sizez);

bol2=sf2.add("sphere");
bol2.position=point(1,2,0);
bol2.color=color(1,0.5,0);
bol2.radius=0.25;
bol2.resolution=20;

root.time=time(2008,1,1,0,0,0);
root.TimeSpeedFactor=20;

while true do {
   incrtime;
   refframe.clearobjects;
   crv=refframe.add("Curve");
   crv.CurveRenderType=CurveRenderDot;
   crv.Size=2;
   crv.Arrow1Size=0.15;
   crv.Arrow2Size=0.15;
   pt1=sf1.transf*point(1,0,0);
   pt2=sf2.transf*point(1,2,0);
   crv.makeline(6*pt1+1*pt2,1*pt1+6*pt2);
   render;
}
