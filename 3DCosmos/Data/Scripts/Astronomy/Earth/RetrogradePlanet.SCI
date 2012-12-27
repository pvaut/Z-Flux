codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_CamMoveTools.sci");
codeblock readtextfile(ScriptFilePath+"\_SUB\_AnimateTools.sci");


JID=0;

ssysframe=ssys_create;

showconstellations=true;
ssys_showconstellations(showconstellations);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
root.framerate=30;
ssys_createmilkyway;
ssys_createsun;

planetpointlist=list("Mercurius","Venus","Mars","Jupiter","Saturnus","Uranus","Neptunus");

planetnamelist=list(
   list("Mercurius",1600),
   list("Venus",1400),
   list("Earth",1400),
   list("Mars",1600),
   list("Jupiter",800),
   list("Saturnus",900),
   list("Uranus",1000),
   list("Neptunus",1000)
   );
foreach item in planetnamelist do {
   planetname=item(0);
   zoomf=item(1);
   pinfo=ssys_getplanetinfo(planetname);
   ssys_createplanet(planetname,zoomf);
   GetPlanetOrbit(planetname).color=color(0,0.3,0.6,0.3);
   GetPlanetOrbit(planetname).BlendType=BlendTransparent;
   pfr=GetPlanetFrame(planetname);
   arfr=pfr.addviewdirframe(point(0,0,0),"arrowframe");
   arfr.visible=false;
   ar=arfr.add("Arrow","EnableLight":false,"color":color(1,0.5,0));
   ar.position=point(0,zoomf*pinfo.radius+autokm(0.25),0);
   ar.Axis=vector(0,autokm(-0.2),0);ar.NormAxis=vector(0,0,1);
   ar.Width=autokm(0.1);
}

#darker shadows
root.SC.AmbientLightColor=color(0.02,0.02,0.02);

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.13);
myviewport.FarClipPlane=autokm(200);
myviewport.FocalDistance=autokm(7);
myviewport.enableusernavigation=true;

myviewport.camerapos=point(autokm(10),autokm(5),autokm(5));
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);

clockframe=ssysframe.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.065,0.065,0),"Size":0.06,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);
   
   
   
#create other viewport
myviewport.YMaxFrac=1.0;

mapscene=addscene;
mapscene.Name="mapscene";
rootf=mapscene.addsubframe("rootf");
sf=rootf.addviewportframe("sc");

try { DelObject(root.Viewports.map); }
displayname=ReadSetting("DisplayName","");
if displayname=="" then displayname="\\.\DISPLAY1";
vp2=addviewport(0,1,1,1.3,displayname,displayname);
vp2.name="Map";
vp2.Usestereo=root.Viewports.main.Usestereo;
vp2.Framesize=0.003;
vp2.start;
vp2.setscene(root.mapscene);

OffsetAngle=Pi;

fac=1/360*vp2.aspectratio;

tx=sf.createtexture("star",DataDir+"\textures\star3.bmp");
mp=sf.add("StarMap");
mp.SizeX=vp2.aspectratio;
mp.SizeY=vp2.aspectratio/2;
mp.Position=point(0,0.5-mp.SizeY/2,0);
mp.StarSize=0.007;
mp.LineSize=0.01;
mp.LineColor=color(0.3,0.3,0.6,0.5);
mp.RenderBack=true;
mp.Texture="star";
mp.OffsetAngleX=OffsetAngle;

#single constellation highlight framework
SingleConstFrame=rootf.addviewportframe("sc");
SingleConstFrame.BlinkPeriod=0.25;
singleconst=SingleConstFrame.add("StarMap");
singleconst.SizeX=vp2.aspectratio;
singleconst.SizeY=vp2.aspectratio/2;
singleconst.Position=point(0,0.5-mp.SizeY/2,0);
singleconst.StarSize=0;
singleconst.LineSize=0.015;
redcol1=GetColor("Red");redcol1.a=0.5;
singleconst.LineColor=redcol1;
singleconst.RenderBack=true;
singleconst.OffsetAngleX=OffsetAngle;
singleconst.SingleConstellationName="XXX";

#create ecliptica line
sf.add("Curve","Color":color(1,0.5,0,0.5),"BlendType":BlendTransparent,"CurveRenderType":CurveRenderDot,"Size":2).makeline(point(0,0.5,0),point(vp2.aspectratio,0.5,0));

trackframe=sf.addviewportframe("TrackFrame");

#Planet track point
plantrackframe=sf.addviewportframe("PlanTrackFrame");
plantrackframe.blinkperiod=0.3;
plantrackpoint=plantrackframe.add("SolidPoint","name":"pl","Color":color(0.3,1,0),"BlendType":BlendTranslucent,"Size":20,"position":point(0.2,0.2),"texture":"plantx");
plantrackpoint.visible=false;


#Normal planet points
planframe=sf.addviewportframe("PlanetPointFrame");
pname="saturnus";
psize=autokm(0.000000001);
sf.createtexture("plantx",DataDir+"\textures\star1.bmp",DataDir+"\textures\star1.bmp");
planframe.DepthMask=DepthMaskDisable;
planframe.DepthTest=DepthTestDisable;
foreach planetname in planetpointlist do {
   planpoint=planframe.add("SolidPoint","name":planetname,"Color":color(0,1,0),
      "BlendType":BlendTranslucent,
      "DepthTest":false,"DepthMask":false,
      "Size":15,"position":point(0.2,0.2),"texture":"plantx");
}
planframe.visible=false;


#Sun point
sunframe=sf.addviewportframe("sunframe");
sunframe.DepthMask=DepthMaskDisable;
sunframe.DepthTest=DepthTestDisable;

suntx=sf.createtexture("suntx",DataDir+"\textures\star1.bmp",DataDir+"\textures\star1.bmp");
sunpoint=sunframe.add("SolidPoint","name":"sun1","Color":color(1.0,0.25,0),"Size":65,
      "BlendType":BlendTransparent,
      "DepthTest":false,"DepthMask":false,
      "position":point(0.2,0.2),"texture":"suntx");
sunpoint=sunframe.add("SolidPoint","name":"sun2","Color":color(1.0,1.0,0),"Size":28,
      "BlendType":BlendTransparent,
      "DepthTest":false,"DepthMask":false,
      "position":point(0.2,0.2),"texture":"suntx");

lasttrackpoint=point(-1000,-1000,-1000);
createvar(trackcurve);


#Create Earth-Planet direction
root.SC.Universe.SolarSystem.add("Fog","Type":FogExponential,"Density":(1/autokm(50)),"Color":color(0,0,0));
plandirline=root.SC.Universe.SolarSystem.add("Curve","color":color(0,0.5,0),"BlendType":BlendTransparent);
plandirline.Size=2;
root.SC.Universe.SolarSystem.add("Fog","Type":FogNone);


trackplanetname="";
showplanetdir=false;

function HighlighPlanet(pname,how)
{
   arfr=root.SC.Universe.SolarSystem.get(pname).arrowframe;
   if how then arfr.blinkperiod=0.4;
   else {
      arfr.blinkperiod=0;
      arfr.visible=false;
   }
}

function TrackPlanet(pname,showdir)
{
   if pname!=trackplanetname then {
      trackplanetname=pname;
      plantrackpoint.visible=pname.length>0;
      trackframe.clearobjects;
      lasttrackpoint=point(-1000,-1000,-1000);
   }
   showplanetdir=showdir;
   plandirline.reset;
}

comovefrac=0;
comoving=false;





lastplanetidx=-1;
currenttrack='';

############### DEFINITION OF ANIMATION STEP ################################
function AnimateStep()
{

   incrtime;
   Cam_Adjust(myviewport);
   earthpos=GetPlanetPosition("earth");
   earthdirflat=@vector(earthpos);earthdirflat.z=0;earthdirflat=vecnorm(earthdirflat);
   
   if comoving then {
      comovefrac=(comovefrac+0.0002)*1.03;
      if comovefrac>1 then comovefrac=1;
   } else {
      comovefrac=comovefrac/1.03-0.0002;
      if comovefrac<0 then comovefrac=0;
   }
   
   camdist=sqrt(sqr(myviewport.camerapos.x)+sqr(myviewport.camerapos.y));
   campos=point(
      camdist*earthdirflat.x,
      camdist*earthdirflat.y,
      myviewport.camerapos.z
      );
   myviewport.focaldistance=0.8*camdist;
   myviewport.camerapos=(1-comovefrac)*myviewport.camerapos+comovefrac*campos;
   myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
   
   if true then {#show sun on map
      dir=vecnorm(point(0,0,0)-earthpos);
      ang1=rad2deg(angle(dir.x,dir.y)+OffsetAngle);
      while ang1<0 do ang1=ang1+360;
      while ang1>360 do ang1=ang1-360;
      angz=rad2deg(asin(dir.z/dir.size));
      pt=point((360-ang1)*fac,0.5+angz*fac);
      root.mapscene.rootf.sc.sunframe.sun1.position=pt;
      root.mapscene.rootf.sc.sunframe.sun2.position=pt;
   }
   
   if trackplanetname.Length>0 then {
      dir=vecnorm(GetPlanetPosition(trackplanetname)-earthpos);
            
      ang1=rad2deg(angle(dir.x,dir.y)+OffsetAngle);
      while ang1<0 do ang1=ang1+360;
      while ang1>360 do ang1=ang1-360;
      angz=rad2deg(asin(dir.z/dir.size));
      pt=point((360-ang1)*fac,0.5+angz*fac);
      root.mapscene.rootf.sc.plantrackframe.pl.position=pt;
      
      if distance(pt,lasttrackpoint)>0.2 then {
         trackcurve=trackframe.add("Curve","enablelight":false,#"BlendType":BlendTransparent,
            "Size":2,"color":color(0,0.5,0));
      }
      if distance(pt,lasttrackpoint)>0.005 then {
         trackcurve.addpoint(pt);
         lasttrackpoint=pt;
      }
      
      if showplanetdir then
         plandirline.makeline(earthpos,earthpos+autokm(200)*dir);
   }
   
   if planframe.visible then {
      foreach planetname in planetpointlist do {
         dir=vecnorm(GetPlanetPosition(planetname)-earthpos);
         ang1=rad2deg(angle(dir.x,dir.y)+OffsetAngle);
         while ang1<0 do ang1=ang1+360;
         while ang1>360 do ang1=ang1-360;
         angz=rad2deg(asin(dir.z/dir.size));
         pt=point((360-ang1)*fac,0.5+angz*fac);
         root.mapscene.rootf.sc.planetpointframe.get(planetname).position=pt;
      }
   }
   
   render;
   
   if lst_trackplanets.SelectIdx!=lastplanetidx then {
        if currenttrack.length>0 then {
           HighlighPlanet(currenttrack,false);
           TrackPlanet('',false);
           currenttrack='';
        }
      if lst_trackplanets.SelectIdx>0 then {
         currenttrack=planetpointlist(lst_trackplanets.SelectIdx-1);
         HighlighPlanet(currenttrack,true);
         TrackPlanet(currenttrack,true);
      }
      lastplanetidx=lst_trackplanets.SelectIdx;
   }

}


function ShowSkyMap()
{
   sz=0.3;
   root.Viewports.main.YMaxFrac=1-sz;
   root.Viewports.Map.YMinfrac=1-sz;
   root.Viewports.Map.YMaxfrac=1-sz+0.3;
}

function ShowZodiacMap()
{
   zlist=list("ARI","TAU","GEM","CNC","LEO","VIR","LIB","SCO","SGR","CAP","AQR","PSC");
   foreach zname in zlist do {
      SingleConstFrame.BlinkPeriod=SingleConstFrame.BlinkPeriod+0.0001;#force syncing of blink cycle
      singleconst.SingleConstellationName=zname;
      animate(1.3);
      singleconst.SingleConstellationName="XXX";
      animate(0.25);
   }
}


cam_init(myviewport);

#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;
dialogframe.DepthTest=DepthTestDisable;
dialogframe.DepthMask=DepthMaskDisable;
root.SC.Universe.DialogFrame.addignoreviewport("Map");

lst_trackplanets=dialogframe.add("ListControl","Size":0.03,"Position":point(0.02,yps),"CountY":1,"SizeX":0.25);
lst_trackplanets.List.add('No tracking');
foreach planet in planetpointlist do
   lst_trackplanets.List.add(planet);
lst_trackplanets.SelectIdx=0;
yps=yps-ysep;


#=================================================================================
# ANIMATION
#=================================================================================

root.time=time(2011,9,1,0,0,0);
root.TimeSpeedFactor=150000*6;
myviewport.camerapos=point(37006323.99467822, 1507727943.873744, 640994723.5070447);

planframe.visible=true;
ShowSkyMap;


while true do {
   AnimateStep;
}

stop;

FadeViewportsIn;
animate(1);
Cam_RotatePos(myviewport,point(10034785207.990473, 5290233002.020952, 4398204409.610373),10);
animate(11);#tell about solar system in general

Cam_RotatePos(myviewport,point(267955201.70870414, 1385998736.0555556, 433468474.83873093),6);
animate(7);#tell about well planets close to earth

Cam_RotatePos(myviewport,point(617529398.4180943, 1341266148.9486222, 18707349.445709705),6);
animate(7);#tell about ecliptica plane and projection on map

planframe.visible=true;
ShowSkyMap;
animate(9);


ShowZodiacMap;
animate(2);

FadeViewportsOut;
planframe.visible=false;

if true then {#track retrograde movement of Mars
   cam_rotatepos(myviewport,point(351131928.2024049, -240120028.55611977, 80758105.23378578),0.0001);
   root.time=time(2009,9,1,0,0,0);
   root.TimeSpeedFactor=150000*4;
   HighlighPlanet("Mars",true);
   FadeViewportsIn;
   animate(3);
   HighlighPlanet("Mars",false);
   TrackPlanet("Mars",true);
   animate(40);
   FadeViewportsOut;
   HighlighPlanet("Mars",false);
   TrackPlanet("",false);
}


if true then {#track retrograde movement of Jupiter
   root.time=time(2009,12,29,0,0,0);
   root.TimeSpeedFactor=150000*10;
   cam_rotatepos(myviewport,point(-590520423.7983273, -420091835.66957295, 247359855.07464457),0.0001);
   FadeViewportsIn;
   TrackPlanet("Jupiter",true);
   animate(40);
   FadeViewportsOut;
}



stop;
