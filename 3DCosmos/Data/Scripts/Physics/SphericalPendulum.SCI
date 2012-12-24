codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
vp=T_getviewport;
sss.ambientlightcolor=color(0.15,0.15,0.15);
rootframe=sss.Universe;

vp.camerapos=point(0,3,3);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=distance(point(0,0,0),vp.camerapos);

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

refframe=rootframe.addsubframe("RefFrame");
createvar(objframe);createvar(stk);

function creatependulum()
{
   refframe.dispose;
   refframe=rootframe.addsubframe("RefFrame");
   b=refframe.add("Sphere","radius":1,"color":color(0.2,0.3,0.4,0.25),"resolution":30);
   b.BlendType=BlendTransparent;b.DepthMask=DepthMaskDisable;b.RenderBack=true;
   
   mt=MotionForceField.create(refframe);
   mt.ForceField.RestrictToSphere(point(0,0,0),1);
   mt.ForceField.SetAccuracy(1,0.001);
   mt.ForceField.AddParallelGravity(vector(0,-1,0));
   mt.ForceField.AddLinearDrag(sdrag.Value);
   
   refframe.add("Curve","Color":color(0.75,0.2,0.2),"Size":2).makeline(point(0,1,0),point(0,-1,0));
   refframe.add("Curve","Color":color(0,0.5,0),"Size":1).makecircle(point(0,0,0),vector(0,1,0),1,80);
   refframe.add("sphere","radius":0.02,"color":color(0.5,0.5,0.5));
   
   h=deg2rad(sangle.Value);
   spd=sspeed.Value;
   
   objframe=refframe.addsubframe("ObjFrame");
   objframe.Transf.origin=point(sin(h),-1*cos(h),0);
   objframe.velocity=vector(0,0,spd);objframe.motion=mt;
   sph=objframe.add("sphere","radius":0.05,"color":color(0.5,0.5,0.5));
   crv=refframe.add("Curve","Color":color(0.25,0.5,0.75,0.3),"Size":2);
   crv.BlendType=BlendTransparent;
   crv.DepthMask=DepthMaskDisable;
   crv.track(objframe,0.01);
   
   stk=refframe.add("Cylinder","Radius":0.02,"color":color(0.5,0.5,0.5));
}


######################################################################################
# SETTINGS DIALOG BOX
######################################################################################

cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);

txt1=cframe.add("TextControl","Position":point(0.025,0.95),"Content":'Start angle',"size":0.04);
sangle=cframe.add("ScalarControl");
sangle.position=point(0.025,0.9);
sangle.size=0.04;
sangle.SizeX=0.13;sangle.RangeSizeX=0.2;
sangle.min=0;sangle.max=180;sangle.step=5;sangle.DecimalCount=0;
sangle.value=60;

txt1=cframe.add("TextControl","Position":point(0.025,0.85),"Content":'Start speed',"size":0.04);
sspeed=cframe.add("ScalarControl");
sspeed.position=point(0.025,0.8);
sspeed.size=0.04;
sspeed.SizeX=0.13;sspeed.RangeSizeX=0.2;
sspeed.min=0;sspeed.max=2;sspeed.step=0.02;sspeed.DecimalCount=2;
sspeed.value=0.2;


txt1=cframe.add("TextControl","Position":point(0.025,0.75),"Content":'Drag',"size":0.04);
sdrag=cframe.add("ScalarControl");
sdrag.position=point(0.025,0.7);
sdrag.size=0.04;
sdrag.SizeX=0.13;sdrag.RangeSizeX=0.2;
sdrag.min=0;sdrag.max=0.4;sdrag.step=0.01;sdrag.DecimalCount=2;
sdrag.value=0.05;

btadd=cframe.add("ButtonControl");
btadd.content="Simulate";
btadd.position=point(0.025,0.60);
btadd.size=0.04;
btadd.sizex=0.2;


creatependulum;

incr=3;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeedFactor=incr;

while true do {
   incrtime;
   stk.Axis=objframe.transf.origin-point(0,0,0);
   
   #position light
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
   
   render;
   if btadd.wasmodified then creatependulum;
}
