
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptFilePath+"\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_PlanetariumTools.sci");

###########################################################################################################
###########################################################################################################
###########################################################################################################

juststarted=true;
introduration=0;




function CustomAnimateStep()
{
   apos=root.SC.Universe.SolarSystem.Earth.transf.origin;
   vpos=root.SC.Universe.SolarSystem.Venus.transf.origin;
   beam1.makeline(apos,vpos);
   beam2.makeline(apos,point(0,0,0));
   thepie.Position=apos;
   thepie.Axis1=vecnorm(point(0,0,0)-apos);
   thepie.Axis2=vecnorm(vpos-apos);
   thepie.Radius2=autokm(0.25);
   root.Viewports.Sun.cameradir=vecnorm(point(0,0,0)-root.Viewports.Sun.camerapos);
}

viewdir=Pi;
viewrd=95;


function AnimateSunRiseSet(maxdurat,which)
{


   root.SC.Universe.theoverlay.opkomst.visible=not(which);
   root.SC.Universe.theoverlay.ondergang.visible=which;
   busy=true;
   tpe=1;sgn=-1;
   targetviewdir=Pi;
   if which then {
      tpe=0;
      sgn=1;
      targetviewdir=0;
   }
   
   root.Viewports.main.camerapos=point(viewrd*sin(viewdir),viewrd*cos(viewdir),-1*viewrd);
   root.Viewports.main.cameradir=vecnorm(vector(-1*sin(viewdir),-1*cos(viewdir),1));
   root.Viewports.main.focaldistance=100;
   
   if abs(targetviewdir-viewdir)>0.1 then {
      viewdir0=viewdir;
      iictr=120;
      for ii=0 to iictr do {
         fr=SmoothRamp1(ii/iictr,0.5);
         viewdir=fr*targetviewdir+(1-fr)*viewdir0;
         root.Viewports.main.camerapos=point(viewrd*sin(viewdir),viewrd*cos(viewdir),-1*viewrd);
         root.Viewports.main.cameradir=vecnorm(vector(-1*sin(viewdir),-1*cos(viewdir),1));
         render;
      }
   }

   chr=Chrono;chr.start;
   repcount=0;
   while busy do {
      root.time=root.time+24*60*60;
      calcpass(root.SC.Universe.SolarSystem.Sun,-0/180*Pi,tpe,mydata.lattit,mydata.longit);
      epos=root.SC.Universe.SolarSystem.Earth.transf.origin;
      venusdir=vecnorm(root.SC.Universe.SolarSystem.Venus.transf.origin-epos);
      sundir=vecnorm(root.SC.Universe.SolarSystem.Sun.transf.origin-epos);
      if juststarted then {
         FadeViewPortsIn;
         animate(introduration);
         juststarted=false;
      }
      for ctt=1 to 3 do animatestep;
      hh=((venusdir*sundir)^vector(0,0,1));
      if which then {
         if (hh>0.02) and (repcount>5) then return(0);
      } else {
         if (hh<-0.02) and (repcount>5) then return(0);
      }
      if (maxdurat>0) and (chr.elapsed>maxdurat) then
         return(0);
      repcount=repcount+1;
   }
}



#=================================================================================
# SETUP
#=================================================================================


mydata=map;
mydata.scfac=100;
mydata.starsizefrac=0.008;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
mydata.camh=-1.99;
InitPlanetarium(ref(mydata));

textcl=color(0.6,0.6,0.6);
textsize=0.06;
overlay=root.SC.Universe.addscreenframe("theoverlay");
overlay.addignoreviewport("Sun");
#root.SC.Universe.moveobject("theoverlay",-1000);
overlay.CastVolumeShadow=false;
itxt=overlay.add("Text3D","name":"opkomst","Position":point(0.05,0.95,0),"UnitX":vector(textsize,0,0),"UnitY":vector(0,textsize,0),"Content":"Zonsopkomst",
   "EnableLight":false,"Color":textcl,"DepthTest":DepthTestDisable,"DepthMask":DepthMaskDisable,"BlendType":BlendTranslucent);
itxt=overlay.add("Text3D","name":"ondergang","Position":point(0.5,0.95,0),"UnitX":vector(textsize,0,0),"UnitY":vector(0,textsize,0),"Content":"Zonsondergang",
   "EnableLight":false,"Color":textcl,"DepthTest":DepthTestDisable,"DepthMask":DepthMaskDisable,"BlendType":BlendTranslucent);

ssys_createplanet("Venus",1000);
mydata.clipbelowhorizon=true;
CreatePlanetariumClock(true);
CreateBackDrop(ref(mydata));

root.time=time(2011,1,27,0,0,0);
#root.time=CurrentTimeUT;
myviewport=T_getviewport;
myviewport.Cameradir=vecnorm(vector(0.00001,0,1));
myviewport.FocalDistance=2.0*mydata.scfac;
myviewport.EyeSeparation=0.05*myviewport.FocalDistance;
myviewport.Aperture=60/180*Pi;

############## create other viewports
myviewport.XMaxFrac=0.57;
Planetarium_CreateViewPort_Sun(ref(mydata));
mydata.viewport_sun.xminfrac=0.57;

mydata.viewport_sun.yminfrac=0.0;
mydata.viewport_sun.ymaxfrac=1.0;
mydata.viewport_sun.FrameTop=false;mydata.viewport_sun.FrameBottom=false;mydata.viewport_sun.FrameRight=false;
mydata.viewport_sun.EnableUserNavigation=true;
mydata.viewport_sun.aperture=1.0;
mydata.viewport_sun.FrameColor=color(0.25,0.25,0.25);
mydata.viewport_sun.Framesize=0.003;
mydata.viewport_sun.cameradir=vecnorm(vector(0,0.5,-1));
sundistance=3.3;
mydata.viewport_sun.focaldistance=1.75*autokm(sundistance);
mydata.viewport_sun.camerapos=point(0,0,0)-autokm(sundistance)*mydata.viewport_sun.cameradir;

root.SC.MaskFrame.addignoreviewport("Earth");
root.SC.MaskFrame.addignoreviewport("Sun");

#Hide all other planets besides Venus
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Mercurius.visible=false;
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Mars.visible=false;
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Jupiter.visible=false;
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Saturnus.visible=false;
#Special treatment for Venus indication
pt=root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Venus.Point;
pt.color=GetColor("Red");
pt.size=12;
pt.BlendType=BlendTransparent;
txt=root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Venus.Name;
txt.color=GetColor("Red");
txt.size=8;
root.SC.Universe.SolarSystem.Earth.moveobject("PlanetsIndicators",999);

#enlarge earth a bit
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.radius=2*3189000;

#hide moon
root.SC.Universe.SolarSystem.Earth.MoonHalo.visible=false;
root.SC.Universe.SolarSystem.Earth.Luna.visible=false;
root.SC.Universe.SolarSystem.Earth.LunaOrbit.visible=false;

pieframe=root.SC.Universe.SolarSystem.AddSubFrame("PieFrame");
pieframe.addignoreviewport("main");
redcol1=GetColor("Red");redcol1.a=0.5;
redcol2=GetColor("Red");redcol2.a=0.3;
beam1=pieframe.Add("Curve","enablelight":false,"color":redcol1,"size":2,"BlendType":BlendTranslucent);
beam2=pieframe.Add("Curve","enablelight":false,"color":redcol1,"size":2,"BlendType":BlendTranslucent);
thepie=pieframe.Add("Pie","enablelight":false,"color":redcol2,"BlendType":BlendTranslucent);
thepie.Radius1=0;
thepie.Radius2=autokm(1.0);


root.time=time(2009,10,1,3,0,0);
root.TimeSpeedFactor=0;

#=================================================================================
# ANIMATION
#=================================================================================

root.SC.MaskFrame.MaskHorizonFrame.color=color(1,1,1,0.7);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.n.color=color(0,0,0);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.o.color=color(0,0,0);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.z.color=color(0,0,0);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.w.color=color(0,0,0);

oo=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.o;
oo.Position.x=oo.Position.x-10;

mydata.viewport_sun.camerapos=point(0,0,0)-autokm(sundistance)*vecnorm(vector(0.02,-0.02,-1));


introduration=2;
AnimateSunRiseSet(-1,false);

animate(2);
AnimateSunRiseSet(0.0001,true);
animate(2);


AnimateSunRiseSet(3,true);
mydata.comoving=true;
AnimateSunRiseSet(4,true);
Cam_RotatePos(mydata.viewport_sun,point(0,0,0)-autokm(sundistance)*vecnorm(vector(0,1,-0.2)),4);
AnimateSunRiseSet(4,true);
AnimateSunRiseSet(-1,true);

animate(2);
AnimateSunRiseSet(4,false);
Cam_RotatePos(mydata.viewport_sun,point(0,0,0)-autokm(sundistance)*vecnorm(vector(0.01,1,0)),6);
AnimateSunRiseSet(-1,false);

animate(2);
AnimateSunRiseSet(5,true);

FadeViewPortsOut;
stop;