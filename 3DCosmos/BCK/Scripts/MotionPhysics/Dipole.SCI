codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.25,0.25,0.25);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.2);
rootframe.add("sphere","radius":0.05,"position":point(1,0,0),"color":color(0.25,0.25,0.25));
rootframe.add("sphere","radius":0.05,"position":point(0,1,0),"color":color(0,1,0));
rootframe.add("sphere","radius":0.05,"position":point(0,0,1),"color":color(0,0,1));

ff=ForceField;
ff.SetAccuracy(1,0.001);
ff.AddElectricDipole(point(0,0,0),vector(0,0,1));

#draw force field
rdd=0.2;
for i1=0 to 5 do {
   for i2=0 to 8 do {
      ang1=(i1+1)/6*Pi/2*0.5;
      ang2=i2/9*2*Pi;
      pt=point(rdd*cos(ang2)*sin(ang1),rdd*sin(ang2)*sin(ang1),rdd*cos(ang1));
      crv=rootframe.add("Curve","Color":color(0.25,0.5,0.25,0.5),"Size":1,
         "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
      finished=false;ct=0;
      while not(finished) and (ct<1000) do {
         crv.addpoint(pt);
         ff.IntegrateForce(pt,1,1,time(2009,1,1,0,0,0),0.08);
         if distance(pt,point(0,0,0))<rdd then finished=true;
         ct=ct+1;
      }
   }
}

#create magnetic dipole force field
mt=MotionForceField.create(rootframe);
mt.ForceField.SetAccuracy(1,0.001);
mt.ForceField.AddMagneticDipole(point(0,0,0),vector(0,0,1));


#create motion object
for i=1 to 60 do {
   objframe=rootframe.addsubframe("ObjFrame");
   objframe.Transf.origin=point(0,0.12*i/2,4);
   objframe.velocity=vector(0,0,-0.2);
   objframe.charge=1;
   objframe.motion=mt;
   sph=objframe.add("sphere","radius":0.03,"color":color(1,1,1));
   crv=rootframe.add("Curve","Color":color(0.25,0.5,0.95,1),"Size":1,
#      "CurveRenderType":CurveRenderSmooth,
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable
      );
   crv.track(objframe,0.02);
}


incr=0.02;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeed=incr;

ctt=0;
while true do {
   incrtime;
   render;
   ctt=ctt+1;
}
