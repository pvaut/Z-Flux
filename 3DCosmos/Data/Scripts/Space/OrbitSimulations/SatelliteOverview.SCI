
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

ssysframe=ssys_create;
ssys_createearth(1);
#ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

root.time=time(2009,9,1,9,0,0);
root.TimeSpeedFactor=300;

earthframe=GetPlanetFrame("Earth");

pengtexture=ssysframe.CreateTexture("pengt",datadir+"\textures\star2.bmp");
peng=earthframe.Inclin.add("PEngine");
peng.PointSize=1000;
peng.Texture=pengtexture.name;
peng.color=color(1,0.5,0.3);
motions=list;
motionpoints=list;

satlist1=list;satlist2=list;satlist3=list;
ReadObject(satlist3,datadir+"\Satellites\gps.bfl");
ReadObject(satlist1,datadir+"\Satellites\geostationary.bfl");
ReadObject(satlist2,datadir+"\Satellites\eccentrics.bfl");
satlist=satlist1+satlist2+satlist3;

ct=0;
foreach sat in satlist do {
   if (sat.Eccentricity<0.95) and (ct<1200) then {
      mt=MotionKepler.Create(earthframe.Inclin,"Motion"+str(ct));
      mt.SemiMajorAxis=sat.SemiMajorAxis;
      mt.Eccentricity=sat.Eccentricity;
      mt.Inclination=sat.Inclination;
      mt.AscendingNode=sat.AscendingNode;
      mt.ArgPerigee=sat.ArgOfPericenter;
      mt.period=sat.Period;
      mt.PeriTime=timeJD(sat.Epoch);
      mt.PeriTime.AddDays(-1*sat.MeanAnomaly/360.0*sat.Period);
      motions.add(mt);
      peng.add;
      ct=ct+1;
#      cv=earthframe.inclin.add("Curve");
#      mt.CreateCurve(cv,200);
   }
}




#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=20000;
vp.FarClipPlane=400000;
vp.FocalDistance=120000;
vp.cameradir=vecnorm(earthframe.Transf.origin-point(0,0,0))-vector(0,0,0.4);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;

#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);


while true do {
   incrtime;
   
   
   
   #co-move camera with Earth movement (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #keep planet in focal plane
   vp.FocalDistance=1.1*distance(vp.camerapos,point(0,0,0));

   EvalMotionList(motions,motionpoints);
   peng.SetPpositionVec(motionpoints);
   
   
   render;
}

