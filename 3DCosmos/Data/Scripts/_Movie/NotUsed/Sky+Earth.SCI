
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptDir+"\_Movie\_PlanetariumTools.sci");

###########################################################################################################
###########################################################################################################
###########################################################################################################

function CreateBackDrop(mydata)
{
   #black obscuring object with hole for the sky dome
   bdframe=root.SC.addsubframe("sf");
   surf1=bdframe.add("Surface","name":"surface","enablelight":false,"color":color(0,0,0));
   rd=1.003*mydata.scfac;
   fnc1=functor("radial2point(r,a,0)-vector(0,0,0.01*"+str(rd)+")","r","a");
   dm=rd;
   surf1.Generate(fnc1,rd,3*rd,2,0,2*pi,60);
   surf1.canbuffer=true;
}



###########################################################################################################
###########################################################################################################
###############                                          START                              ###############
###########################################################################################################
###########################################################################################################


mydata=map;
mydata.scfac=100;
mydata.starsizefrac=0.008;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
mydata.camh=-1.99;
InitPlanetarium(ref(mydata));
mydata.clipbelowhorizon=true;
CreateClock;


#backdrop
CreateBackDrop(ref(mydata));

root.time=time(2011,1,27,0,0,0);
#root.time=CurrentTimeUT;
myviewport=T_getviewport;
myviewport.Cameradir=vecnorm(vector(0.00001,0,1));
myviewport.FocalDistance=2.0*mydata.scfac;
myviewport.EyeSeparation=0.05*myviewport.FocalDistance;
myviewport.Aperture=60/180*Pi;

############## create other viewports
myviewport.XMaxFrac=0.55;
Planetarium_CreateViewPort_Earth(ref(mydata));
Planetarium_CreateViewPort_Sun(ref(mydata));

mydata.viewport_earth.EnableUserNavigation=true;
mydata.viewport_sun.EnableUserNavigation=true;

root.SC.sf.addignoreviewport("Earth");
root.SC.sf.addignoreviewport("Sun");


#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;






root.FrameRate=30;
#root.VSync=2;

root.TimeSpeedFactor=100;
while true do {
   incrtime;
#root.time=CurrentTimeUT;
#   root.time=CurrentTimeUT+0*60*60;


   AdjustPlanetarium(ref(mydata));
   
   
   render;
   
   if UIGetAxisActive(UIAxisX,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisX,UIAxisLevel0);
      myviewport.CameraDir=vecrotate(myviewport.CameraDir,vector(0,0,1),0.03*vl);
   }
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then {
      ok=false;
      ang0=vecangle(myviewport.CameraDir,vector(0,0,1));
      vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
      if vl>2 then vl=2;
      if vl<-2 then vl=-2;
      if (vl>0) and (ang0>5/180*Pi) then ok=true;
      if (vl<0) and (ang0<75/180*Pi) then ok=true;
      ok=true;
      if ok then {
         dr1=vecnorm(myviewport.CameraDir*vector(0,0,1));
         myviewport.CameraDir=vecrotate(myviewport.CameraDir,dr1,0.03*vl);
      }
   }
   if UIGetAxisActive(UIAxisZ,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisZ,UIAxisLevel0);
      myviewport.Aperture=myviewport.Aperture*(1-vl/20);
      if myviewport.Aperture<30/180*Pi then myviewport.Aperture=30/180*Pi;
      if myviewport.Aperture>170/180*Pi then myviewport.Aperture=170/180*Pi;
   }
   
}
