
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\_Movie\_PlanetariumTools.sci");


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
mydata.starsizefrac=0.004;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
mydata.camh=0.1;

InitPlanetarium(ref(mydata));
CreateClock;
CreateBackDrop(ref(mydata));




root.time=time(2010,12,31,0,0,0);
#root.time=CurrentTimeUT;
myviewport=T_getviewport;
myviewport.FocalDistance=0.06*mydata.scfac;
myviewport.Aperture=70/180*Pi;


root.TimeSpeedFactor=100;
while true do {
   incrtime;

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
