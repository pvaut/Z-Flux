codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=12;

objectspeed=0.5;
objectdistance=4;

sss.ambientlightcolor=color(0.25,0.25,0.25);

######################################################################################
# SETTINGS DIALOG BOX
######################################################################################
txtsize=0.03;

cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);

rc1=cframe.add("FrameControl");
rc1.position=point(0,0.81);
rc1.SizeX=0.33;
rc1.SizeY=0.3;
rc1.color=color(0.2,0.2,0.2);

txt1=cframe.add("TextControl");
txt1.position=point(0.025,0.96);
txt1.Content='Speed:';
txt1.size=txtsize;

vlspeed=cframe.add("ScalarControl");
vlspeed.position=point(0.14,0.96);
vlspeed.size=txtsize;
vlspeed.SizeX=0.08;vlspeed.RangeSizeX=0.1;
vlspeed.min=0;vlspeed.max=1;vlspeed.step=0.1;vlspeed.DecimalCount=1;
vlspeed.value=0;

txt1=cframe.add("TextControl");
txt1.position=point(0.025,0.92);
txt1.Content='Angle:';
txt1.size=txtsize;

vlangle=cframe.add("ScalarControl");
vlangle.position=point(0.14,0.92);
vlangle.size=txtsize;
vlangle.SizeX=0.08;vlangle.RangeSizeX=0.1;
vlangle.min=-45;vlangle.max=45;vlangle.step=5;vlangle.DecimalCount=1;
vlangle.value=0;

txt1=cframe.add("TextControl");
txt1.position=point(0.025,0.88);
txt1.Content='Strength:';
txt1.size=txtsize;

vlstrength=cframe.add("ScalarControl");
vlstrength.position=point(0.14,0.88);
vlstrength.size=txtsize;
vlstrength.SizeX=0.08;vlstrength.RangeSizeX=0.1;
vlstrength.min=0;vlstrength.max=0.2;vlstrength.step=0.01;vlstrength.DecimalCount=2;
vlstrength.value=0.05;


btadd=cframe.add("ButtonControl");
btadd.content='Simulate';
btadd.position=point(0.025,0.83);
btadd.size=txtsize;
btadd.sizex=0.15;

startframe=sss.Universe.addsubframe("StartFrame");

####################################################################################

function start()
{
   startframe.clearobjects;
   centerspeed=vlspeed.Value;
   objectspeedang=deg2rad(vlangle.value);
   
   rootframe=startframe.addsubframe("Root");
   rootframe.transf.origin=point(-1*objectdistance/objectspeed*centerspeed,0,0);
   rootframe.velocity=vector(centerspeed,0,0);
   
   crv=startframe.add("Curve",
      "Color":color(1,0.5,0,0.5),
      "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
      "Size":2);
   crv.track(rootframe,0.1);
   
   
   rootframe.add("sphere","radius":0.07,"Color":color(0.3,0.4,0.7),"Resolution":15);
   
   mt=MotionForceField.create(rootframe);
   mt.ForceField.SetAccuracy(1,0.001);
   mt.ForceField.AddElectricPointCharge(point(0,0,0),vlstrength.value);
   
   objects=list;
   ctx=5;
   cty=2;
   for ix=0 to ctx do {
      for iy=0 to cty do {
         objframe=rootframe.addsubframe("ObjFrame_"+Str(ix)+"_"+str(iy));
         objframe.charge=1.0;
         objspd=vector(-1*centerspeed,0,-1*objectspeed);
         objspd=vecrotate(objspd,vector(0,1,0),objectspeedang);
         objframe.Transf.origin=point((ix-ctx/2)/2,(iy-cty/2)/2,0)-objectdistance*objspd;
         objframe.velocity=objspd;
         objframe.motion=mt;
         objects.add(objframe);
         sph=objframe.add("sphere","radius":0.05,"color":color(0.2,0.4,0.4),"Resolution":10);
         crv=startframe.add("Curve",
            "Color":color(0.5,0.5,0.5,0.5),
            "BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
            "Size":2);
         crv.track(objframe,0.02);
      }
   }
   
   
   starttm=time(2009,1,1,0,0,0);
   root.time=starttm;
   root.TimeSpeedFactor=2;
   
   tmilast=0;
   ticktm=0.5;
   spdstart=-1;
   running=true;
   root.Pauzed=false;
}

createvar(starttm);
createvar(tmilast);
createvar(ticktm);
createvar(spdstart);
createvar(objects);
createvar(rootframe);

running=false;


while true do {
   incrtime;
   if btadd.WasModified then start;
   if running then {
      tmi=root.time.DiffSecs(starttm);
      if floor(tmi/ticktm)>floor(tmilast/ticktm) then {
         output(str(tmi)+" "+str(floor(tmi/ticktm))+" "+str(floor(tmilast/ticktm)));
         shift=@vector(rootframe.transf.origin);
         foreach obj in objects do {
            pt=obj.transf.origin+shift;
            spd=(obj.velocity+rootframe.velocity).size;
            if spdstart<0 then spdstart=spd;
            fr=0.6*spd/spdstart;
            cr=sqrt(fr);
            cg=1-fr*fr;
            cb=0.2;
            startframe.add("SolidPoint","Size":5,"Color":color(cr,cg,cb,0.7),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"Position":pt);
         }
         pt=point(0,0,0)+shift;
         startframe.add("SolidPoint","Size":4,"Color":color(1,1,0,0.35),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"Position":pt);
      }
      tmilast=tmi;
      if tmi>20 then root.Pauzed=true;
   }
   render;
}
