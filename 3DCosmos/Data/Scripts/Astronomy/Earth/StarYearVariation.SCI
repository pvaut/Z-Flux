codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_PlanetariumTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Figure.sci");


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
CreatePlanetariumClock(true);
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.visible=false;

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
myviewport.XMaxFrac=0.6;
Planetarium_CreateViewPort_Earth(ref(mydata));
Planetarium_CreateViewPort_Sun(ref(mydata));

mydata.viewport_earth.xminfrac=0.6;
mydata.viewport_sun.xminfrac=0.6;

mydata.viewport_earth.yminfrac=0.5;
mydata.viewport_earth.ymaxfrac=1.0;
mydata.viewport_sun.yminfrac=0.0;
mydata.viewport_sun.ymaxfrac=0.502;

mydata.viewport_earth.EnableUserNavigation=true;
mydata.viewport_sun.EnableUserNavigation=true;

root.SC.MaskFrame.addignoreviewport("Earth");
root.SC.MaskFrame.addignoreviewport("Sun");


#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;

#hide the moon
root.SC.Universe.SolarSystem.Earth.MoonHalo.visible=false;
root.SC.Universe.SolarSystem.Earth.Luna.visible=false;
root.SC.Universe.SolarSystem.Earth.LunaOrbit.visible=false;

#enlarge earth a bit
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.radius=3*3189000;

#create local neighborhood with zenith & horizon
earthframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering;
earthradius=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.radius;
localframe1=earthframe.addsubframe("Viewer");
localframe1.addignoreviewport("Earth");
localframe1.transf.zaxis=radial2point(1,(mydata.longit+Pi),mydata.lattit)-point(0,0,0);
localframe1.transf.yaxis=-1*vecnorm(localframe1.transf.zaxis*vector(0,0,1));
localframe1.transf.xaxis=-1*vecnorm(localframe1.transf.zaxis*localframe1.transf.yaxis);
ln=localframe1.add("Curve","enablelight":false,"Size":2,"Arrow2Size":(1*earthradius),
   "color":color(0,1,0));
ln.makeline(point(0,0,0),point(0,0,6*earthradius));
localframe1.add("Cylinder","Axis":vector(0,0,0.001*earthradius),
     "Radius":(3.8*earthradius),"Resolution":50,
     "enablelight":false,"color":color(0.3,0.7,0.3,0.4),"BlendType":BlendTranslucent);

localframe1.add("Sphere",
     "Radius":(3.77*earthradius),"Resolution":50,"Angle2Max":(Pi/2),
     "enablelight":false,
     "color":color(0.1,0.5,1,0.15),"BlendType":BlendTransparent);
localframe1.add("Sphere",
     "Radius":(3.8*earthradius),"Resolution":50,"Angle2Max":(Pi/2),
     "color":color(0.2,0.5,1,0.5),"BlendType":BlendTranslucent);
  
 
#create viewer information on earth overview
localframe2=earthframe.addsubframe("Viewer2");
localframe2.addignoreviewport("Sun");
localframe2.transf.zaxis=radial2point(1,(mydata.longit+Pi),mydata.lattit)-point(0,0,0);
localframe2.transf.yaxis=-1*vecnorm(localframe2.transf.zaxis*vector(0,0,1));
localframe2.transf.xaxis=-1*vecnorm(localframe2.transf.zaxis*localframe2.transf.yaxis);
ln=localframe2.add("Curve","enablelight":false,"Size":2,"Arrow2Size":(0.1*earthradius),
   "color":color(0,1,0));
ln.makeline(point(0,0,0),point(0,0,1.5*earthradius));
figobj=CreateFigure(localframe2,0.3*earthradius);
figobj.position.z=0.95*earthradius;
figobj.position.x=-0.1*earthradius;



ctr=0;
function CustomAnimateStepPre()
{
   ctr=ctr+1;
   if chk_varyseason.WasModified then {
      if chk_varyseason.Checked then {
         root.TimeSpeedFactor=0;
         root.time=time(2011,3,21,0,0,0);
      }
      else {
         root.TimeSpeedFactor=3000;
      }
   }
   if chk_varyseason.Checked then {
      if not(root.pauzed) then {
         if ctr>5 then { root.time=root.time+24*60*60; ctr=0; }
      }
   }
   
   if lst_position.WasModified then {
      SetLattit(positions(lst_position.SelectIdx));
   }
}

function SetLattit(lattit)
{
   mydata.lattit=(lattit+3/60.0)/180*Pi;
   mydata.vframe.transf.reset;
   mydata.vframe.transf.rotate(vector(0,1,0),Pi/2-mydata.lattit);
   mydata.vframe.transf.rotate(vecrotate(vector(0,0,1),vector(0,1,0),-1*(Pi/2-mydata.lattit)),mydata.longit);
   
   localframe1.transf.zaxis=radial2point(1,(mydata.longit+Pi),mydata.lattit)-point(0,0,0);
   localframe1.transf.yaxis=-1*vecnorm(localframe1.transf.zaxis*vector(0,0,1));
   localframe1.transf.xaxis=-1*vecnorm(localframe1.transf.zaxis*localframe1.transf.yaxis);
   
   localframe2.transf.zaxis=radial2point(1,(mydata.longit+Pi),mydata.lattit)-point(0,0,0);
   localframe2.transf.yaxis=-1*vecnorm(localframe2.transf.zaxis*vector(0,0,1));
   localframe2.transf.xaxis=-1*vecnorm(localframe2.transf.zaxis*localframe2.transf.yaxis);
   
   if abs(lattit)>50 then
      SetHorizonMask("");

   if (lattit<55) and (lattit>=45) then
      SetHorizonMask("Gent");
   
   if (lattit<20) and (lattit>=-50) then
      SetHorizonMask("Exotic");

}

function C_RA(hours,minutes)
{
   return((hours+minutes/60)/24*360);
}

function C_DE(degs,minutes)
{
   return(degs+minutes/60);
}

mydata.viewport_earth.camerapos=point(0,0,0)-45000000*mydata.viewport_earth.cameradir;
root.Viewports.Sun.camerapos=point(0,0,0)-autokm(4.5)*root.Viewports.Sun.cameradir;




#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;
dialogframe.DepthTest=DepthTestDisable;
dialogframe.DepthMask=DepthMaskDisable;
root.SC.Universe.DialogFrame.addignoreviewport("Earth");
root.SC.Universe.DialogFrame.addignoreviewport("Sun");

lst_position=dialogframe.add("ListControl","Size":0.03,"Position":point(0.02,yps),"CountY":1,"SizeX":0.25);
positions=list;
lst_position.List.add('North Pole');          positions.add(89.0);
lst_position.List.add('Nortern hemisphere');  positions.add(51.0);
lst_position.List.add('Equator');             positions.add(0.0);
lst_position.List.add('Southern hemisphere'); positions.add(-45.0);
lst_position.List.add('South Pole');          positions.add(-89.0);
lst_position.SelectIdx=1;
yps=yps-ysep;

chk_varyseason=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_varyseason.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Vary season');
yps=yps-ysep;

ActivateControl(lst_position);


root.TimeSpeedFactor=0;

#=================================================================================
# ANIMATION
#=================================================================================


root.time=time(2011,3,21,19,0,0);
root.TimeSpeedFactor=5000;
while true do
   AnimateStep();



stop;
####################################################################################################
#   THE END   
####################################################################################################


if true then {
#show daily variation in southern hemisphere
root.TimeSpeedFactor=0;
animate(1);
SetLattit(-51);
root.time=time(2011,3,21,19,0,0);
SetHorizonMask("Exotic");
animate(1);
ConstellHighlight_Start("CRU",GetColor("Red"));
animate(3);

root.TimeSpeedFactor=3000;
animate(17);
ConstellHighlight_Stop("CRU");
}



#Show yearly variation in northern hemisphere
root.TimeSpeedFactor=0;
animate(2);
SetLattit(51);
root.TimeSpeedFactor=0;
root.time=time(2011,3,21,0,0,0);
SetHorizonMask("Gent");
animate(2);
for i=1 to 365 do {
   if i==90 then {
      ConstellHighlight_Start("CYG",GetColor("Red"));
   }
   if i==115 then {
      ConstellHighlight_Stop("CYG");
   }
   if i==120 then {
      PointHighlight_Start("sa1",color(1,0.5,0),  C_RA(18,36),C_DE(38,47));
      PointHighlight_Start("sa2",color(1,0.5,0),  C_RA(20,41),C_DE(45,16));
      PointHighlight_Start("sa3",color(1,0.5,0),  C_RA(19,50),C_DE(8,52));
      ArcHighlight_Start("sarc1",GetColor("Red"),  C_RA(18,36),C_DE(38,47),C_RA(20,41),C_DE(45,16),  false);
      ArcHighlight_Start("sarc2",GetColor("Red"),  C_RA(20,41),C_DE(45,16),C_RA(19,50),C_DE(8,52),  false);
      ArcHighlight_Start("sarc3",GetColor("Red"),  C_RA(19,50),C_DE(8,52),C_RA(18,36),C_DE(38,47),  false);
   }
   if i==150 then {
      ConstellHighlight_Stop("sa1");
      ConstellHighlight_Stop("sa2");
      ConstellHighlight_Stop("sa3");
      ConstellHighlight_Stop("sarc1");
      ConstellHighlight_Stop("sarc2");
      ConstellHighlight_Stop("sarc3");
   }
   
   if i==170 then 
      ConstellHighlight_Start("ORI",GetColor("Red"));
   if i==320 then 
      ConstellHighlight_Stop("ORI");
      
   Animate(0.15);
   root.time=root.time+24*60*60;
}

#root.TimeSpeedFactor=2000;
animate(2);

FadeViewPortsOut;
stop;