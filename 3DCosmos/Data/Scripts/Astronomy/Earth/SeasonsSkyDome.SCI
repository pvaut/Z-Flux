codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_SkyViewTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Figure.sci");

#=================================================================================
# SETUP
#=================================================================================

mydata=map;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
InitSkyView(ref(mydata));



#equinoxes & solstices frame
poledir=root.SC.Universe.SolarSystem.Earth.Inclin.transf.Zaxis;
dr1=vecnorm(vector(0,0,1)*poledir);
dr2=vecnorm(vector(0,0,1)*dr1);
dd0=0.8;dd=1.1;
txts=autokm(0.075);
ptsframe=root.SC.Universe.SolarSystem.addsubframe("PTS");
ptsframe.visible=false;


line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr1),@point(autokm(dd)*dr1));
txt=ptsframe.addviewdirframe(@point(autokm(dd*1.01)*dr1)).add("Text3D","Content":'Lente equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);

line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr1),@point(autokm(-1*dd)*dr1));
txt=ptsframe.addviewdirframe(@point(autokm(-1*dd*1.01)*dr1)).add("Text3D","Content":'Herfst equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);



line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr2),@point(autokm(dd)*dr2));
txt=ptsframe.addviewdirframe(@point(autokm(dd*1.01)*dr2)).add("Text3D","Content":'Zomer zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);

line=ptsframe.add("Curve","color":color(0,0.25,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr2),@point(autokm(-1*dd)*dr2));
txt=ptsframe.addviewdirframe(@point(autokm(-1*dd*1.01)*dr2)).add("Text3D","Content":'Winter zonnewende',"EnableLight":false,"color":color(0,0.5,0));
txt.position=point(0.25*txts,0,0);
txt.UnitX=txts*vector(0,1,0);
txt.UnitY=txts*vector(-1,0,0);


SetViewPosition(0,deg2rad(51));
root.time=time(2011,6,21,15,0,0);
root.TimeSpeedFactor=3000;


#######################################################################################################"

function JumpToDate2(newtime)
{
   prevspeed=root.TimeSpeedFactor;
   root.TimeSpeedFactor=0;
   animate(1);
   while root.time-newtime<0 do {
      root.time=root.time+24*60*60;
      root.Viewports.main.CameraDir=vecrotate(root.Viewports.main.CameraDir,vector(0,0,1),1/(365.25)*2*Pi);
      for ct=1 to 3 do AnimateStep;
   }
   animate(1);
   root.TimeSpeedFactor=prevspeed;
}


function ShowSummerDay()
{
   mydata.earthoverlayframe.visible=true;
   Cam_Rotate(root.Viewports.main,vecnorm(vector(0.749070976, -0.378340884, -0.543829797)),6);
   ZoomIn;
   animate(2);
   mydata.shadowframe.visible=true;
   animate(2);
   #animate a summer day in sky view
   SkyFrameAppear;
   CreateSunTrace("trace_summer",color(0.8,0.8,0));
   animate(10);
   FlashIntro(root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.TropicCancer);
   animate(15);
   SkyFrameDisAppear;
   FreezeSunTrace("trace_summer");
   #go back to solar overview
   ZoomOut;
   mydata.shadowframe.visible=false;
   animate(3);
}

function ShowFallDay()
{
   #Jump to fall & zoom in
   JumpToDate(time(2011,9,20,18,0,0));
   animate(3);
   ZoomIn;
   animate(2);
   mydata.shadowframe.visible=true;
   animate(2);
   #animate a fall day in sky view
   SkyFrameAppear;
   CreateSunTrace("trace_fall",GetColor("Red"));
   animate(20);
   SkyFrameDisAppear;
   FreezeSunTrace("trace_fall");
   #go back to solar overview
   ZoomOut;
   mydata.shadowframe.visible=false;
   animate(3);
}

function ShowWinterDay()
{
   #Jump to winter & zoom in
   JumpToDate(time(2011,12,20,22,0,0));
   animate(3);
   Cam_Rotate(root.Viewports.main,vecnorm(vector(0.786549147, 0.305521838, -0.536653376)),6);
   ZoomIn;
   animate(2);
   mydata.shadowframe.visible=true;
   animate(2);
   #animate a winter day in sky view
   SkyFrameAppear;
   CreateSunTrace("trace_winter",color(0.8,0.8,0));
   animate(7);
   FlashIntro(root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.TropicCapriCorn);
   #rotate the camera
   Cam_RotateHor(root.Viewports.Sky,-2*Pi,40);
   animate(20);
   SkyFrameDisAppear;
   FreezeSunTrace("trace_winter");
   #go back to solar overview
   ZoomOut;
   mydata.shadowframe.visible=false;
}

function ShowSpecialPoints()
{
   mydata.earthoverlayframe.visible=true;
   mydata.shadowframe.visible=true;
   oldspeedfactor=root.TimeSpeedFactor;
   root.TimeSpeedFactor=0;
   #show special points & rotate
   ptsframe.visible=true;
   animate(2);
   Cam_RotateHor(root.Viewports.main,2*Pi,30);
   root.Viewports.main.custom.cam_rotdir=vecnorm(vector(0,-0.5,1));
   for i=0 to 12 do {
      i2=i-4*floor(i/4);
      if i2==0 then root.time=time(2011,12,21,12,0,0);
      if i2==1 then root.time=time(2011,3,21,12,0,0);
      if i2==2 then root.time=time(2011,6,21,12,0,0);
      if i2==3 then root.time=time(2011,9,22,12,0,0);
      animate(30/12);
   }
#   animate(30);
   ptsframe.visible=false;
   animate(3);
   root.time=time(2011,12,21,15,0,0);
   root.TimeSpeedFactor=oldspeedfactor;
}

function ShowWinterSH()
{
   #winter in southern hemisphere
   SetViewPosition(0,deg2rad(-51));
   Cam_Rotate(root.Viewports.main,vecnorm(vector(0.783771914, 0.167010832, 0.598171354)),5);
   ZoomIn;
   mydata.earthoverlayframe.visible=true;
   mydata.shadowframe.visible=true;
   animate(4);
   SkyFrameAppear;
   #animate winter day on southern hemisphere
   CreateSunTrace("tracewinterZ",color(1,0.5,0));
   animate(15);
   FlashIntro(root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.ArcticCircleS);
   FlashIntro(root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.ArcticCircleN);
   animate(10);
   DelSunTrace("tracewinterZ");
   #animate winter day near south pole
   animate(3);
   SetViewPosition(0,deg2rad(-79));
   CreateSunTrace("tracewinterZ2",color(1,0.5,0));
   animate(10);
   animate(22);
   DelSunTrace("tracewinterZ2");
}

function ShowMidnightSunN()
{
   #Goto summer date
   Cam_Rotate(root.Viewports.main,vecnorm(vector(0.18274524, 0.888753824, -0.420381752)),4);
   SkyFrameDisAppear;
   animate(3);
   JumpToDate2(time(2012,6,20,21,0,0));
   animate(3);
   SetViewPosition(0,deg2rad(90-23.4457889));
   SkyFrameAppear;
   CreateSunTrace("tracesummerN2",color(1,0.5,0));
   animate(10);
   animate(22);
}



#function CustomAnimateStepPre() {
#   return(0);
#}

function differ(b1,b2) {
   if b1 then return(not(b2));
   else return(b2);
}


#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;
dialogframe.DepthTest=DepthTestDisable;
dialogframe.DepthMask=DepthMaskDisable;
root.SC.Universe.DialogFrame.addignoreviewport("Sky");

x0=0.025;

chk_zoom=dialogframe.add("CheckControl","Size":0.03,"Position":point(x0,yps));
chk_zoom.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(x0+0.04,yps),"Content":'Zoom in');
yps=yps-ysep;

chk_skyframe=dialogframe.add("CheckControl","Size":0.03,"Position":point(x0,yps));
chk_skyframe.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(x0+0.04,yps),"Content":'Show sky');
yps=yps-ysep;


x0=x0+0.22;
yps=0.95;

bt_jumptime=dialogframe.add("ButtonControl");
bt_jumptime.content='Next time point';
bt_jumptime.position=point(x0,yps);
bt_jumptime.size=0.03;
bt_jumptime.sizex=0.20;
yps=yps-ysep;

bt_cleartrace=dialogframe.add("ButtonControl");
bt_cleartrace.content='Clear traces';
bt_cleartrace.position=point(x0,yps);
bt_cleartrace.size=0.03;
bt_cleartrace.sizex=0.20;
yps=yps-ysep;



x0=x0+0.25;
yps=0.95;

lst_position=dialogframe.add("ListControl","Size":0.03,"Position":point(x0,yps),"CountY":1,"SizeX":0.25);
positions=list;
lst_position.List.add('North Pole');          positions.add(88.0);
lst_position.List.add('Northern hemisphere');  positions.add(51.0);
lst_position.List.add('Equator');             positions.add(0.0);
lst_position.List.add('Southern hemisphere'); positions.add(-45.0);
lst_position.List.add('South Pole');          positions.add(-88.0);
lst_position.SelectIdx=1;
yps=yps-ysep;

ActivateControl(chk_zoom);



#=================================================================================
# ANIMATION
#=================================================================================
root.Viewports.Sky.EnableUserNavigation=false;
animdurat=Chrono;animdurat.start;
#FadeViewportsIn;

mydata.earthoverlayframe.visible=true;
mydata.shadowframe.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.TropicCancer.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.TropicCapriCorn.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.ArcticCircleN.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.OverlayFrame.ArcticCircleS.visible=true;

positionindex=1;
zoomed=false;
skyvisible=false;
CreateSunTrace("",color(0.8,0.8,0));

while true do {
   AnimateStep;

   if differ(chk_zoom.Checked,zoomed) then {
      if chk_zoom.Checked then zoomin();
      else zoomout();
      zoomed=chk_zoom.Checked;
   }
   
   if differ(chk_skyframe.checked,skyvisible) then {
      if chk_skyframe.Checked then {
         DelAllSunTraces();
         SkyFrameAppear();
         CreateSunTrace("",color(0.8,0.8,0));
      }
      else SkyFrameDisAppear();
      skyvisible=chk_skyframe.Checked;
   }
   
   if positionindex!=lst_position.SelectIdx then {
      FreezeSunTrace("",color(0.8,0.8,0));
      SetViewPosition(0,deg2rad(positions(lst_position.SelectIdx)));
      positionindex=lst_position.SelectIdx;
      CreateSunTrace("",color(0.8,0.8,0));
   }

   if bt_jumptime.WasModified then { 
      FreezeSunTrace("",color(0.8,0.8,0));
      NextTimePoint();
      #JumpToDate(time(2011,9,20,18,0,0));
      CreateSunTrace("",color(0.8,0.8,0));
   }
   
   if bt_cleartrace.WasModified then {
      DelAllSunTraces();
      CreateSunTrace("",color(0.8,0.8,0));
   }
}

stop;


if true then {
   animate(3);
   ShowSummerDay;
   ShowFallDay;
   ShowWinterDay;
   animate(3);
   DelSunTrace("trace_summer");
   DelSunTrace("trace_fall");
   DelSunTrace("trace_winter");
   ShowSpecialPoints;
}

ShowWinterSH;
ShowMidnightSunN;


FadeViewportsOut;
output("Duration: "+str(animdurat.elapsed));
stop;
#####
