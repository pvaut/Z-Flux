
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptFilePath+"\_SUB\_FoucaultTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_AnimateTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_CamMoveTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Figure.sci");



lastposindex=-1;
function CustomAnimateStepPre()
{
   if lst_position.SelectIdx!=lastposindex then {
      setposition(deg2rad(positions(lst_position.SelectIdx)));
      StartPendulum(true);
      lastposindex=lst_position.SelectIdx;
   }
   if chk_corotate.WasModified then {
      if chk_corotate.Checked then StartCorotate();
      else StopCorotate();
   }
   if chk_arrows.WasModified then {
      root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.GravArrow.visible=chk_arrows.Checked;
      root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendSusp.RotIndic.visible=chk_arrows.Checked;
      root.SC.Universe.SolarSystem.Earth.Inclin.Globe.LocFrame.PendObject.PendPlane.visible=chk_arrows.Checked;
   }
}


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

CreateEarthFrame();


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



#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;
dialogframe.DepthTest=DepthTestDisable;
dialogframe.DepthMask=DepthMaskDisable;
dialogframe.EnabeMouseArrow(point(0.1,0.95),point(0,0.8),point(0.3,1),0.03);
root.SC.Universe.DialogFrame.addignoreviewport("Sky");
#root.SC.Universe.ClockFrame.addignoreviewport("Sky");

lst_position=dialogframe.add("ListControl","Size":0.03,"Position":point(0.02,yps),"CountY":1,"SizeX":0.25);
positions=list;
lst_position.List.add('North Pole');          positions.add(89.0);
lst_position.List.add('Nortern hemisphere');  positions.add(51.0);
lst_position.List.add('Equator');             positions.add(0.0);
lst_position.List.add('Southern hemisphere'); positions.add(-45.0);
lst_position.List.add('South Pole');          positions.add(-89.0);
lst_position.SelectIdx=1;
yps=yps-ysep;

chk_corotate=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_corotate.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Corotate');
yps=yps-ysep;

chk_arrows=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_arrows.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Show arrows');
yps=yps-ysep;

ActivateControl(lst_position);


#=================================================================================
# SETUP
#=================================================================================

root.TimeSpeedFactor=0;
root.Viewports.main.cameradir=vecnorm(vector(-0.2,0.2,-0.6));
root.Viewports.Sky.EnableUserNavigation=false;

#This sets the pendulum position to a particular lattitude position on Earth
setposition(deg2rad(51.0));

root.time=time(2011,3,21,6,0,0);
root.TimeSpeedFactor=1500;
while true do
   AnimateStep();
