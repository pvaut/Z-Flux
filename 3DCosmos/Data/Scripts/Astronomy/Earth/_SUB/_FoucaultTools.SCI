codeblock readtextfile(ScriptFilePath+"\_SUB\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_AnimateTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_CamMoveTools.sci");


#Earth radius
radius=6378.0;


###############################################################################
# START
###############################################################################

ssysframe=ssys_create;
myviewport=GetViewPort;

root.framerate=30;
ssys_createearth(1);
ssys_showstarback(true);
ssys_createmilkyway;
GetPlanetOrbit("Earth").visible=false;

#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;

#create clock
CreateClock(false);
root.SC.Universe.ClockFrame.addignoreviewport("main");
root.time=time(2009,8,1,9,0,0);
root.TimeSpeedFactor=1200;


##################################
lattitude=0;
function setposition(lattit)
{
   lattitude=lattit;
   locframe.transf.reset;
   locframe.transf.rotate(vector(0,1,0),Pi/2-lattit);
   locframe.transf.translate(vector(0,0,radius));
   rotindicframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.RotIndic;
   rotindicframe.transf.reset;
   rotindicframe.transf.rotate(vector(0,1,0),lattit);
}




#initialise shadow
root.SC.VolumeShadowAdd(0,color(0,0,0,0.5),10,10000);



earthframe=GetPlanetFrame("Earth");
globeframe=GetPlanetBodyFrame("Earth");

#Create frame for local position on Earth
locframe=globeframe.addsubframe("LocFrame");

#create pundulum object

pendsize=3000;
pendw=1300;
pendcolor1=color(0.5,0.5,0.5);
pendcolor2=color(1.0,0.8,0.0);
pendspeccolor1=color(0.6,0.6,0.6);pendspecval=30;
pendobjframe=locframe.addsubframe("PendObject");
pendobjframe.transf.origin=point(0,0,0.15*pendw);
pillw=100;
pendobjframe.add("Bar","Position":point(pendw-pillw*1.1,-0.5*pillw,0),
                 "Color":pendcolor1,"SpecularColor":pendspeccolor1,"SpecularValue":pendspecval,
                 "SizeX":(pillw),"SizeY":(pillw),"SizeZ":(pendsize));
pendobjframe.add("Bar","Position":point(0.1*pillw-pendw,-0.5*pillw,0),
                 "Color":pendcolor1,"SpecularColor":pendspeccolor1,"SpecularValue":pendspecval,
                 "SizeX":(pillw),"SizeY":(pillw),"SizeZ":(pendsize));

pendobjframe.add("Bar","Position":point(0.1*pillw-pendw,-0.5*pillw,pendsize),
                 "Color":pendcolor1,"SpecularColor":pendspeccolor1,"SpecularValue":pendspecval,
                 "SizeX":(2*pendw-0.2*pillw),"SizeY":(pillw),"SizeZ":(pillw));

pendobjframe.add("Bar","Position":point(-1*pendw-pillw,-1*pendw-pillw,-700),
                 "Color":color(0.3,0.3,0.3),"SpecularColor":pendspeccolor1,"SpecularValue":pendspecval,
                 "SizeX":(2*pendw+2*pillw),"SizeY":(2*pendw+2*pillw),"SizeZ":(700));
                 
#pendobjframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject;
pendobjframe.add("Bar","position":point(-1400,1400,-700),"SizeX":2800,"SizeY":500,"SizeZ":400,
                 "Color":color(0.3,0.3,0.3),"SpecularColor":pendspeccolor1,"SpecularValue":pendspecval
                 );
figframe=pendobjframe.addsubframe("figframe");
figframe.transf.translate(vector(0,1700,-350));
figframe.transf.rotate(vector(0,0,1),-0.5*pi);
CreateFigure(figframe,700);


                 
pendsuspframe=pendobjframe.addsubframe("PendSusp");pendsuspframe.transf.origin=point(0,0,pendsize);
pendframe=pendsuspframe.addsubframe("Pend");

#gravity arrow
gravarrowframe=pendsuspframe.addsubframe("GravArrow");pendsuspframe.transf.origin=point(0,0,pendsize);
gravarrowframe.add("Arrow","Color":color(1,1,0,0.4),"BlendType":BlendTranslucent,"EnableLight":false,
         "Position":point(0,0,0),"Axis":vector(0,0,-0.7*pendsize),"NormAxis":vector(0,1,0),
         "Width":(0.1*pendsize),"Thickness":0
         );
gravarrowframe.visible=false;

#earth rotation indicator
rotindicframe=pendsuspframe.addsubframe("RotIndic");
cv=rotindicframe.add("Curve","Color":color(0,0.7,0),"Size":3,"CurveRenderType":CurveRenderDashDot);
cv.makeline(point(-0.6*pendsize,0,0),point(0.6*pendsize,0,0));
for arrnr=0 to 1 do {
   ps0=point(-0.5*pendsize,0,0);
   if arrnr==1 then ps0=point(0.5*pendsize,0,0);
   cv=rotindicframe.add("Curve","Color":color(0,0.7,0),"Size":1);
   cv.makearc(ps0,vector(0,0,1),vecnorm(vector(0,0.9,-1)),0.2*pendsize,40);
   cv=rotindicframe.add("Curve","Color":color(0,0.7,0),"Size":1,"Arrow2Size":(0.05*pendsize));
   cv.makearc(ps0,vecnorm(vector(0,1,-1)),vector(0,-1,0),0.2*pendsize,40);
}
rotindicframe.visible=false;

pendframe.add("Cylinder","Position":point(0,0,0),
                 "Color":pendcolor2,"SpecularColor":color(1,1,1),"SpecularValue":60,
                 "Resolution":20,
                 "Radius":20,"Axis":vector(0,0,-0.92*pendsize));
pendframe.add("Sphere","Position":point(0,0,-0.92*pendsize),
                 "Color":pendcolor2,"SpecularColor":color(1,1,1),"SpecularValue":60,
                 "Resolution":20,
                 "Radius":100);
                 
                 


pendtrackframe=pendobjframe.addsubframe("PendTrack");
pendtrackpointcount=0;
pendtrackframe.CastVolumeShadow=false;
pendtrack=pendtrackframe.add("Curve","Color":GetColor("Red"),"EnableLight":false,"CurveRenderType":CurveRenderCube,"Size":200);

pendmotion=MotionSwing.Create(pendframe,"PendMotion");
pendframe.motion=pendmotion;
pendmotion.Period=2000;
swingangle=0.4;
pendmotion.swingangle=swingangle;
pendmotion.NormDir=vector(0,1,0);

pendangle=0;prevpendangle=0;
pendprevtime=root.time;

function AdjustPendSwingPlane()
{
   timestep=root.time-pendprevtime;
   pendprevtime=root.time;
   incrang=-2*Pi*timestep/(24*60*60)*cos(Pi/2-lattitude);
   ndir=vecnorm(vecrotate(pendmotion.NormDir,vector(0,0,1),incrang));
   pendmotion.NormDir=ndir;
   pendplane=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane;
   pendplane.transf.yaxis=ndir;
   pendplane.transf.xaxis=ndir*vector(0,0,1);
   pendangle=pendangle+incrang;
   if (abs(pendangle-prevpendangle)>0.003) and (pendangle<=2*Pi+0.05) then {
      pendtrack.addpoint(point(0.8*pendw*cos(pendangle),0.8*pendw*sin(pendangle),-90));
      pendtrackpointcount=pendtrackpointcount+1;
      pendtrack.visible=(pendtrackpointcount>2);
      prevpendangle=pendangle;
   }
}

pendframe.setprerenderaction(FunctionFunctor("AdjustPendSwingPlane"));

function StartPendulum(dotrack)
{
   pendmotion.RefTime=root.time;
   pendmotion.NormDir=vector(0,1,0);
   pendangle=0;prevpendangle=0;
   pendprevtime=root.time;
   pendtrack.reset;
   pendtrackpointcount=0;
}

###############################################################################################



corotatefrac=0;


function AnimateStep()
{
   if isfunctiondefined("CustomAnimateStepPre") then CustomAnimateStepPre;
   
   Cam_Adjust(myviewport);
   camdist=distance(vp.camerapos,point(0,0,0));
   vp.camerapos=point(0,0,0)-camdist*vp.cameradir;
   
   
   createvar(tftot);createvar(tftotinv);
   tftot=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.totaltransformation;
   tftotinv=tftot;tftotinv.invert;
   root.viewports.Sky.transf=tftotinv;
   skydistance=11500;
   root.viewports.sky.camerapos=point(0,0,0.0*skydistance)+(-1.2*skydistance)*root.viewports.sky.cameradir;


   lasttimestep=incrtime;
   #co-move camera with Earth movement (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
  if corotatefrac>0 then {#co-move camera with Earth rotation
     inclin=deg2rad(-23.4457889);
     poledir=vector(0,-1*sin(inclin),cos(inclin));
     if not(root.Pauzed) then {
        rotang=lasttimestep*2*Pi*365/364/(24*60*60)*corotatefrac;
        vp.CameraPos=point(0,0,0)+vecrotate(vp.CameraPos-point(0,0,0),poledir,rotang);
        vp.CameraDir=vecrotate(vp.CameraDir,poledir,rotang);
        vp.CameraUpDir=vecrotate(vp.CameraUpDir,poledir,rotang);
      }
  }
   render;
}

function StartCorotate()
{
   corotatefrac=1;
}

function StopCorotate()
{
   corotatefrac=0;
}

function FastStopCorotate()
{
   corotatefrac=0;
}



function MovePendulum(newlatt)
{
   lattit0=lattitude;
   chr=Chrono;chr.start;
   while chr.Elapsed<3 do {
      fr=chr.elapsed/3;
      fr=1-(1-fr)*(1-fr);fr=fr*fr;
      setposition(lattit0+fr*(newlatt-lattit0));
      AnimateStep;
   }
}




function stoptime()
{
   chr=Chrono;chr.start;
   while chr.Elapsed<1 do {
      pendmotion.swingangle=swingangle*(1-chr.elapsed);
      root.TimeSpeedFactor=1200*(1-chr.elapsed);
      AnimateStep;
   }
}

function starttime()
{
   chr=Chrono;chr.start;
   while chr.Elapsed<1 do {
      pendmotion.swingangle=swingangle*(chr.elapsed);
      root.TimeSpeedFactor=1200*chr.elapsed;
      AnimateStep;
   }
}




###############################################################################################
###############################################################################################
###############################################################################################



#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.FocalDistance=25000;
vp.NearClipPlane=0.1*vp.FocalDistance;
vp.FarClipPlane=5*vp.FocalDistance;
#vp.cameradir=vecnorm(earthframe.Transf.origin-point(0,0,0))-vector(0,0,0.4);
vp.cameradir=vecnorm(vector(0.3,-0.5,-0.5));
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;
root.SC.ambientlightcolor=color(0.2,0.2,0.2);

Cam_Init(myviewport);

