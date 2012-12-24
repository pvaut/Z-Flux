
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptFilePath+"\_FoucaultTools.sci");

codeblock readtextfile(ScriptFilePath+"\_Colors.sci");

codeblock readtextfile(ScriptFilePath+"\_AnimateTools.sci");

codeblock readtextfile(ScriptFilePath+"\_CamMoveTools.sci");

planesize=2500;
pendframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject;
planeframe=pendframe.addsubframe("PendPlane");
planeframe.CastVolumeShadow=false;
planeframe.add("Rectangle",
   "color":color(0,0.5,1,0.3),"enablelight":false,"RenderBack":true,"BlendType":BlendTranslucent,
   "DepthMask":DepthMaskDisable,
   "Position":point(-1*planesize,0,-2*planesize+3500),"Axis1":vector(2*planesize,0,0),"Axis2":vector(0,0,2*planesize));
planeframe.visible=false;
#planeframe.addignoreviewport("sky");


pendcolor1=color(0.5,0.5,0.5);
pendcolor2=color(1.0,0.8,0.0);
pendspeccolor1=color(0.6,0.6,0.6);pendspecval=30;


##########################################################################

#Create sky viewport
   try { DelObject(root.Viewports.Sky); }
   au2km=149598000;
   
   myviewport=CreateNewViewPort(0.5,0,1,1);
   myviewport.name="Sky";
   myviewport.Framesize=0.005;
   myviewport.FrameColor=color(0.2,0.2,0.2);
   myviewport.FrameRight=false;myviewport.FrameTop=false;myviewport.FrameBottom=false;
   myviewport.start;
   myviewport.setscene(root.SC);
   myviewport.FocalDistance=21000;
   myviewport.cameradir=vecnorm(vector(-0.4,-1.5,-2));
   myviewport.cameraupdir=vector(0,0,1);
   myviewport.enableusernavigation=root.Viewports.main.enableusernavigation;
   myviewport.EnableUserTimeControl=root.Viewports.main.EnableUserTimeControl;
   myviewport.NearClipPlane=0.1*myviewport.FocalDistance;
   myviewport.FarClipPlane=20*myviewport.FocalDistance;
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.addignoreviewport("sky");
   Cam_Init(myviewport);

   root.Viewports.Sky.Active=true;
   root.Viewports.main.XMaxFrac=0.63;
   root.Viewports.Sky.XMinFrac=0.63;
   root.Viewports.Sky.XMaxFrac=1.0;


#=================================================================================
# SETUP
#=================================================================================

root.time=time(2009,8,1,22,0,0);
root.TimeSpeedFactor=0;
root.Viewports.main.cameradir=vecnorm(vector(0.2,-0.6,-0.7));

#This sets the pendulum position to a particular lattitude position on Earth
setposition(deg2rad(89.9));

#=================================================================================
# ANIMATION
#=================================================================================
StartPendulum(true);
FadeViewportsIn;
Animate(2);

#start the time
starttime;
animate(7);

#show gravity arrow
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.GravArrow.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.GravArrow.blinkperiod=0.4;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.GravArrow.maxblinkcount=5;
animate(9);

#show rotation indication
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.RotIndic.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.RotIndic.blinkperiod=0.4;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.RotIndic.maxblinkcount=5;
animate(9);


#show swing plane
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.blinkperiod=0.4;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.maxblinkcount=2;
animate(10);



#move pendulum to equator
stoptime;
#stopcorotate;
animate(2);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=false;
MovePendulum(deg2rad(0.0));
animate(0.5);
Cam_Rotate(root.Viewports.main,vector(0.486147924, -0.833475705, 0.26263748),4);
animate(5);

#start the time
StartPendulum(true);
starttime;
animate(12);


#corotate with earth
#startcorotate;
#animate(4);




#set at belgium position
FadeViewportsOut;
root.time=time(2009,8,1,7,0,0);
root.TimeSpeedFactor=0;
setposition(deg2rad(51.0));
FastStopCorotate;
root.Viewports.main.cameradir=vector(-0.002367378, -0.99255836, -0.121746861);
StartPendulum(true);
FadeViewportsIn;
animate(2);
starttime;
animate(6);
#show swing plane
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=true;
animate(12);



#set at south pole
FadeViewportsOut;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=false;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeedFactor=0;
setposition(deg2rad(-89.0));
FastStopCorotate;
root.Viewports.main.cameradir=vector(0.142389847, 0.629207954, 0.764082773);
StartPendulum(true);
FadeViewportsIn;
animate(2);
starttime;
animate(6);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=true;
animate(12);





FadeViewportsOut;
stop;
