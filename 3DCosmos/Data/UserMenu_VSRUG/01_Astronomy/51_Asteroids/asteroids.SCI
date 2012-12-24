
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

ssysframe=ssys_create;

filecontent=readtextfile(DataDir+"\AstroData\asteroids.txt");
st="";while st.substring(0,4)!="-----" do st=filecontent.split("~n");

pengtexture=ssysframe.CreateTexture("pengt",datadir+"\textures\star2.bmp");
peng=ssysframe.add("PEngine");
peng.PointSize=autokm(0.06);
peng.Texture=pengtexture.name;
peng.color=color(1,0.5,0.3);
motions=list;
motionpoints=list;

epoch=time(2000,1,1,1,0,0);
ct=0;
totct=1300;
while (filecontent.length>0) and (ct<totct) do {
   st=filecontent.split("~n");
   a=ToScalar(st.substring(30,39));
   e=ToScalar(st.substring(41,50));
   ii=ToScalar(st.substring(52,60));
   w=ToScalar(st.substring(62,70));
   O=ToScalar(st.substring(72,80));
   M=ToScalar(st.substring(82,92));
   T=360.0/(0.9856076686/(a^1.5));
   peritm=epoch;peritm.adddays(0-M/360.0*T);
   ct=ct+1;
   mt=MotionKepler.Create(ssysframe,"Motion"+str(ct));
   mt.SemiMajorAxis=autokm(a);
   mt.Eccentricity=e;
   mt.Inclination=deg2rad(ii);
   mt.AscendingNode=deg2rad(O);
   mt.ArgPerigee=deg2rad(w);
   mt.period=T;
   mt.PeriTime=peritm;
   motions.add(mt);
   peng.add;
}

ssys_createsun;
planetnamelist=ssys_getplanetlist;
foreach planetname in planetnamelist do ssys_createplanet(planetname,300);

root.SC.Universe.StarBackFrame.StarBack.starsize=10471860000/2;

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.13);
myviewport.FarClipPlane=autokm(80);
myviewport.FocalDistance=autokm(17);
myviewport.enableusernavigation=true;

myviewport.camerapos=point(autokm(10),autokm(5),autokm(5));
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);

clockframe=ssysframe.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.065,0.065,0),"Size":0.06,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);

root.time=CurrentTimeUT;
#root.time=time(2001,9,1,0,0,0);
root.TimeSpeedFactor=300000;
while true do {
   incrtime;
   
   EvalMotionList(motions,motionpoints);
   peng.SetPpositionVec(motionpoints);
   if RightMouseClicked then stop;
   
   render;
}