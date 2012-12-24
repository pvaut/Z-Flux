codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptFilePath+"\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_AnimateTools.sci");
codeblock readtextfile(ScriptFilePath+"\_CamMoveTools.sci");


JID=0;

ssysframe=ssys_create;

ssys_showconstellations(true);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
ssys_createmilkyway;
ssys_createsun;

ssys_createplanet("earth",2500);


myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.2);
myviewport.FarClipPlane=autokm(400);
myviewport.FocalDistance=autokm(3);
myviewport.enableusernavigation=true;
root.framerate=30;


CreateClock(true);
   

earthframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe;
backframe=root.SC.Universe.StarBackFrame;


dirlineframe0=earthframe.addsubframe("Dir");
dirlineframe0.add("Fog","Type":FogExponential,"Density":(1/autokm(50)),"Color":color(0,0,0));
dirlineframe=dirlineframe0.addsubframe("Dir");
dirlineframe0.add("Fog","Type":FogNone);

#target stellar position
stardist=autokm(20000);
starpoint0=point(0,0,0)+stardist*vecnorm(vector(-0.5,-1,0));





function CreateEarthDirLine(name,targetpoint,col)
{
   tf=dirlineframe.totaltransformation;tf.invert;
   dirpt=point(0,0,0)+stardist*vecnorm(tf*targetpoint-point(0,0,0));
   dirline=dirlineframe.add("Curve","name":name,"color":col,"BlendType":BlendTransparent);
   dirline.Size=2;
   dirline.makeline(point(0,0,0),dirpt);
   dirline.Custom.dirpoint=dirpt;
}



function MarkDirPoint(dirname,sizefactor,col,doblink)
{
   ptframe=backframe.addsubframe("pt");
   if doblink then {
      ptframe.BlinkPeriod=0.25;
      ptframe.MaxBlinkCount=6;
   }
   dirline=dirlineframe.get(dirname);
   dirpt=dirline.custom.dirpoint;
   dirpt=earthframe.totaltransformation*dirpt;
   ptframe.add("Sphere","Position":dirpt,
        "radius":(0.005*stardist*sizefactor),
        "color":col,"enablelight":false,"DepthMask":DepthMaskDisable);
}

earthprogress=ssysframe.add("Pie");
earthprogress.Height=0;
earthprogress.Radius1=0;
earthprogress.Radius2=autokm(1);
redcol1=GetColor("Red");redcol1.a=0.25;
earthprogress.color=redcol1;
earthprogress.BlendType=BlendTranslucent;
earthprogress.EnableLight=false;
earthprogressstarted=false;

function AnimateStep()
{
   incrtime;
   Cam_Adjust(myviewport);
   myviewport.camerapos=point(0,0,0)-0.8*myviewport.focaldistance*myviewport.cameradir;
   earthprogress.axis2=vecnorm(GetPlanetPosition("earth")-point(0,0,0));
   earthprogress.InvertDir=(earthprogress.axis1*earthprogress.axis2)^vector(0,0,1)<0;
   if not(earthprogressstarted) then {
      earthprogress.axis1=earthprogress.axis2;
      earthprogressstarted=true;
   }
#   vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;
   render;
}

function MoveDayST(durat)
{
   timestart=root.time;
   ct=round(durat*30);
   for i=0 to ct do {
      fr=i/ct;
      fr=SmoothRamp1(fr,0.6);
      root.time=timestart+24*60*60*365.25/366.25*fr;
      AnimateStep;
   }
}

function MoveDay(durat)
{
   timestart=root.time;
   ct=round(durat*30);
   for i=0 to ct do {
      fr=i/ct;
      fr=SmoothRamp1(fr,0.6);
      root.time=timestart+24*60*60*fr*1.00001;
      AnimateStep;
   }
}



function AnimateSolarDay1()
{
   CreateEarthDirLine("sundir0",point(0,0,0),color(0.5,0.4,0));
   animate(1);
   earthprogress.visible=true;
   
   for j=1 to 3 do {
      Animate(2);
      MoveDay(5);
   }
}

function AnimateSolarDay2()
{
   FadeViewportsOut;
   root.time=time(2012,12,21,0,0,0);
   root.TimeSpeedFactor=0;
   FadeViewportsIn;
   CreateEarthDirLine("stardir0",point(0,0,0)+stardist*vector(-0.5,-1,0),color(0,0.4,0));
   MarkDirPoint("stardir0",1,color(0,0.5,0),true);
   
   animate(1.0);
   for j=1 to 3 do {
      Animate(2);
      MoveDay(5);
      MarkDirPoint("stardir0",0.5,color(0,0.5,0),true);
   }
   
   animate(2);
   #fast progress per solar day
   for j=1 to 20 do {
      Animate(0.35);
      root.time=root.time+24*60*60;
      MarkDirPoint("stardir0",0.5,color(0,0.5,0),false);
   }
   animate(2);
}


function AnimateSiderealDay1()
{
   FadeViewportsOut;
   root.time=time(2012,12,21,0,0,0);
   root.TimeSpeedFactor=0;
   FadeViewportsIn;
   
   dayct=3;
   for j=1 to dayct do {
      Animate(2);
      MoveDayST(5);
   }
   
   animate(2);
   
   #fast progress per sidereal day
   for j=1 to 365-dayct+1 do {
      root.time=root.time+24*60*60*365.25/366.25;
      earthprogress.axis2=vecnorm(GetPlanetPosition("earth")-point(0,0,0));
      earthprogress.InvertDir=(earthprogress.axis1*earthprogress.axis2)^vector(0,0,1)<0;
      for ii=1 to 3 do render;
   }
}


Cam_init(myviewport);

root.time=time(2012,12,21,0,0,0);

#=================================================================================
# ANIMATION
#=================================================================================

showtime=time(2012,12,21,0,0,0);

#Start
#animate rotation of the Earth for a while
introdurat=3;
introspeedfactor=10000;
earthprogress.visible=false;
root.TimeSpeedFactor=0;
myviewport.cameradir=vecnorm(vector(0.8,0.1,-0.5));
myviewport.camerapos=point(0,0,0)-myviewport.focaldistance*myviewport.cameradir;
root.time=showtime-introdurat*introspeedfactor;
FadeViewportsIn;
introchr=chrono;
while introchr.elapsed<=introdurat do {
   dff=introdurat-introchr.elapsed;
   if dff<0 then dff=0;
   root.time=showtime-dff*introspeedfactor;
   render;
}
root.time=showtime;
render;


#move to position for showing time steps
root.TimeSpeedFactor=0;
animate(1);
movedurat=4;
Cam_Rotate(myviewport,vecnorm(vector(0.0,-0.8,-0.2)),movedurat);
animate(movedurat);
animate(1);


# First animation: per solar day, sun direction only
AnimateSolarDay1;

# Second animation: per solar day, sun direction and star direction
AnimateSolarDay2;

# Third animation: per sidereal
AnimateSiderealDay1;

animate(5000);
FadeViewportsOut;
stop;
