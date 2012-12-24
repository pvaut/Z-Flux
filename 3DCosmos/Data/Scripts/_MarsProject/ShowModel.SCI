
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");

modelname=ScriptArgument;
if not(isdefined(modelname)) then modelname="iss";

#sf=T_scene_create;

#Create solar system
ssysframe=ssys_create;
sss=T_getscene;
myviewport=T_getviewport;vp=myviewport;
SUB_init;
#PanelLeft=SUB_createleftpanel;
SUB_showloading(true);

myviewport.camerapos=radial2point(-10,0,0);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,0,1);

#vp.camerapos=point(0,6,10);
#vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
#vp.cameraupdir=vecnorm((vp.cameradir*vector(0,0,1))*vector(0,0,1));
vp.enableusernavigation=true;
vp.EnableUserTimeControl=true;
vp.NearClipPlane=0.05;
vp.FarClipPlane=30;
vp.FocalDistance=10;
vp.EyeSeparation=0.5;




ssys_showconstellations(false);
ssys_createmilkyway;

myviewport.enableusernavigation=false;
JID=0;#joystick ID

sss=T_getscene;

sss.ambientlightcolor=color(0.15,0.15,0.15);
#sss.VolumeShadowAdd(0,color(0,0,0,0.75),0.002,5);

refframe=sss.addsubframe("refframe");


sf1=sss.addsubframe("sf1");
sf1.cancache=false;
#sf1.color=color(0.8,0.8,0.8);

#sf1.transf.rotate(vector(1,0,0),-0.5*Pi);
#sf1.transf.rotate(vector(0,0,1),Pi);

objectpath="";objectname="";scalefactor=0;
if modelname=="iss" then {
   objectpath="C:/3DCosmos/3DCosmos/Models/ISS_Building";
   #objectname="iss_1A";
   objectname="isscomplete";
   scalefactor=1.1;
#   sf1.transf.rotate(vector(1,0,0),-0.5*Pi);
#   sf1.transf.rotate(vector(0,0,1),Pi);
   sf1.transf.translate(vector(-1.5,-1.0,0));
}

if modelname=="mer" then {
   objectpath="C:/3DCosmos/3DCosmos/Models/MER";
   #objectname="iss_1A";
   objectname="mer2";
   scalefactor=0.0075;
   sf1.transf.translate(vector(0,-0.15,-0.5));
   root.SC.Universe.StarBackFrame.visible=false;
}

if false then {
   objectpath="J:\Astro3D\models_3ds\cassini";
   objectname="cassini";
   scalefactor=1.1;
}

sf1.LoadObject(objectpath,objectname+".3ds","3DS",scalefactor);


comps=sf1.getmembers;
for i=0 to comps.size-1 do if typeof(comps(i))=="SolidObject" then {
##   comps(i).cancache=true;
#   comps(i).canbuffer=true;
   comps(i).enablelight=true;
}

SUB_showloading(false);


root.time=time(2008,1,1,0,0,0);
root.TimeSpeedFactor=0.001;

distaxislast=-2;
distfac=1;
doalign=false;

while true do {
   incrtime;
   
   #distance throttle control
   distaxis=JoystickGetAxis(JID,3);
   if abs(distaxislast-distaxis)>0.015 then {
      distaxislast=distaxis;
      dist=1.4+1.1*sqr(1+distaxislast);
      myviewport.camerapos=point(0,0,0)+dist*vecnorm(myviewport.camerapos-point(0,0,0));
      distfac=dist/3;
      cursorposchanged=true;
   }
   
   JoyAxis1=JoystickGetAxis(JID,0);if abs(JoyAxis1)<0.02 then JoyAxis1=0;
   JoyAxis1=distfac*JoyAxis1/60;
   JoyAxis2=JoystickGetAxis(JID,1);if abs(JoyAxis2)<0.02 then JoyAxis2=0;
   JoyAxis2=-1*distfac*JoyAxis2/60;
   JoyAxis3=JoystickGetAxis(JID,2);if abs(JoyAxis3)<0.02 then JoyAxis3=0;
   JoyAxis3=JoyAxis3/60;
   
   
   if JoyStickButtonDown(0,5) then doalign=true;#Align to North
   if doalign then {
      cdir=vector(0,0,1)-(vector(0,0,1)^myviewport.cameradir)*myviewport.cameradir;
      ang=vecangle(cdir,myviewport.cameraupdir);
      if abs(ang)>0.01 then {
         if (vector(0,0,1)*myviewport.cameraupdir)^myviewport.cameradir<0 then ang=-1*ang;
         JoyAxis3=-0.02*(ang+ang/abs(ang));
      } else doalign=false;
   }
   
   
   rtdir=myviewport.cameraupdir;
   myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,JoyAxis1);
   myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,JoyAxis1);
   myviewport.cameraupdir=vecrotate(myviewport.cameraupdir,rtdir,JoyAxis1);
   
   rtdir=vecnorm(myviewport.cameradir*myviewport.cameraupdir);
   myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,JoyAxis2);
   myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,JoyAxis2);
   myviewport.cameraupdir=vecrotate(myviewport.cameraupdir,rtdir,JoyAxis2);

   rtdir=myviewport.cameradir;
   myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,JoyAxis3);
   myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,JoyAxis3);
   myviewport.cameraupdir=vecrotate(myviewport.cameraupdir,rtdir,JoyAxis3);
   
   
   sss.Light0Pos=point(0,0,0)-100*myviewport.cameradir
      +30*myviewport.cameraupdir
      -60*myviewport.cameradir*myviewport.cameraupdir;
   
   SUB_testcmd();
   
   render;
}
