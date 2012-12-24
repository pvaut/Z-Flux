

codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_waypoints.sci");

#mars pole direction
#inclin=deg2rad(25.19);
#ls=equat2eclipt(deg2rad(317.68143),deg2rad(52.88650));
#poledir=@vector(radial2point(1,ls(0),ls(1)));


JID=0;

ssysframe=ssys_create;
SUB_init;
PanelLeft=SUB_createleftpanel;
SUB_showloading(true);

showconstellations=false;
ssys_showconstellations(showconstellations);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
#ssys_createearth(1000);
ssys_createmilkyway;

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.13);
myviewport.FarClipPlane=autokm(80);
myviewport.FocalDistance=myviewport.NearClipPlane*1.05;
myviewport.EyeSeparation=0.0001;
myviewport.enableusernavigation=false;

myviewport.cameraupdir=vector(0,0,1);


planetlist=ssys_getplanetlist;
namelist=list;
#break;

clockframe=PanelLeft.addscreenframe("ClockFrame");
PanelLines_init(clockframe,0.96);
info_time=PanelLines_add("");
info_time.unitx=1.25*info_time.unitx;
info_time.unity=1.6*info_time.unity;
info_time.color=color(0.6,0.8,1);

info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.03,0.85,0),"Size":0.08,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);

pttx=ssysframe.createtexture("point",DataDir+"\textures\star1.bmp");
function addlightpoint(frame,sz)
{
   sz=sz/2;
   rc=frame.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0));
   rc.Texture=pttx.name;
   rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   return(rc);
}

createvar(marspositframe);createvar(marspointframe);
for i=0 to planetlist.size-1 do {
   planname=planetlist(i);
   plannamedutch=planname;if planname=="Earth" then plannamedutch="Aarde";
   planframe=ssys_createplanet(planname,1000);
   textframe=planframe.addviewdirframe(point(0,0,0));
   if i!=3 then {
      planframe.Inclin.dispose;
   } else {
      marspositframe=planframe;
      marspointframe=textframe;
   }
   addlightpoint(textframe,autokm(0.07)).color=color(0.3,0.3,1);
   addlightpoint(textframe,autokm(0.04)).color=color(0,1,1);
   addlightpoint(textframe,autokm(0.02)).color=color(1,1,1);
   txt=textframe.add("Text3D","Content":plannamedutch,"EnableLight":false,"Color":color(0.7,0.7,0.7),
      "UnitX":vector(autokm(0.08),0,0),"UnitY":vector(0,autokm(0.08),0));
   txt.position=point(-0.5*txt.GetSizeX*autokm(0.08),autokm(0.04),0);
   namelist.add(txt);
      
   planorbit=GetPlanetOrbit(planname);
   planorbit.color=color(0,0.5,1,0.4);
   planorbit.BlendType=BlendTransparent;
   planorbit.DepthMask=DepthMaskDisable;
   
}

#create Mars sphere
marsglobeframe=root.SC.Universe.SolarSystem.Mars.Inclin.Globe;
marsglobeframe.GlobeRendering.dispose;
marstx=marsglobeframe.createtexture("mars2",DataDir+"\textures\mars3.jpg");
marsglobeframe.add("sphere","radius":autokm(0.09),"texture":(marstx.name),"Resolution":50,"Canbuffer":true);
marsglobeframe.add("Cylinder","Radius":autokm(0.004),"Color":color(0.6,0.6,0.6),
   "Position":point(0,0,autokm(-0.15)),"Axis":vector(0,0,autokm(0.3)));

#equator   
marsglobeframe.add("Curve","Size":2,"Color":color(0.5,0,0)).makecircle(point(0,0,0),vector(0,0,1),autokm(0.091),50);

#lattitude circles
for i=-8 to 8 do {
   a=deg2rad(10*i);
   crv=marsglobeframe.add("Curve","Size":1,"Color":color(0.5,0,0,0.5));
   crv.CurveRenderType=CurveRenderSmooth;
   crv.size=autokm(0.001);
   crv.makecircle(point(0,0,autokm(0.091)*sin(a)),vector(0,0,1),autokm(0.091)*cos(a),50);
   txt=marsglobeframe.add("Text3D","Content":str(i*10),"EnableLight":false,"Color":color(0.5,0,0,0.5));
   txt.Position=point(autokm(0.091)*cos(a),0,autokm(0.091)*sin(a));
   txt.UnitX=autokm(0.01)*vector(0,1,0);
   txt.UnitY=autokm(0.01)*vector(-1*sin(a),0,cos(a));
}


#equinoxes & solstices frame
poledir=root.SC.Universe.SolarSystem.Mars.Inclin.transf.Zaxis;
dr1=vecnorm(vector(0,0,1)*poledir);
dr2=vecnorm(vector(0,0,1)*dr1);
dd0=1.3;dd=1.8;
txts=autokm(0.2);
ptsframe=ssysframe.addsubframe("PTS");
ptsframe.visible=false;

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr1),@point(autokm(dd)*dr1));
txt=ptsframe.add("Text3D","Content":"Lentepunt","EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr1);
txt.UnitY=-1*txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr1),@point(autokm(-1*dd)*dr1));
txt=ptsframe.add("Text3D","Content":"Herfstpunt","EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr1);
txt.UnitY=txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr2),@point(autokm(dd)*dr2));
txt=ptsframe.add("Text3D","Content":"Zomerpunt","EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr2);
txt.UnitY=-1*txts*dr2;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr2),@point(autokm(-1*dd)*dr2));
txt=ptsframe.add("Text3D","Content":"Winterpunt","EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr2);
txt.UnitY=txts*dr2;
txt.UnitX=txts*vector(0,0,1);



marsglobeframe.visible=false;

loadwaypoints;
waypointframe=marsglobeframe.addsubframe("waypointframe");
dispwaypoints(waypointframe,autokm(0.09));

loadflags;
flagsframe=marsglobeframe.addsubframe("Flags");
flagsframe.CreateTexture("Flag",ScriptDir+"\MarsProject\Bitmaps\flag.bmp");
dispflags(flagsframe,autokm(0.09));

#sun dir frame
sundirframe=root.SC.Universe.SolarSystem.Mars.addsubframe("SunDirFrame");
sundirframe.add("Curve","Size":3,"Color":color(1,1,0),"Arrow1Size":autokm(0.06)).makeline(point(autokm(0.092),0,0),point(autokm(0.3),0,0));
us=autokm(0.2);
sundirframe.add("Rectangle","Position":point(0,-1*us,-1*us),"Axis1":vector(0,2*us,0),"Axis2":vector(0,0,2*us),
   "RenderBack":true,"Color":color(0.2,0.2,0.2,0.5),"EnableLight":false,"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
sundirframe.add("Rectangle","Position":point(-1*us,-1*us,0),"Axis1":vector(2*us,0,0),"Axis2":vector(0,2*us,0),
   "RenderBack":true,"Color":color(0.5,0.5,0,0.2),"EnableLight":false,"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
sundirframe.visible=false;



#sun reference
textframe=ssysframe.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.2)).color=color(1,0.5,0);
addlightpoint(textframe,autokm(0.09)).color=color(1,1,0.5);
addlightpoint(textframe,autokm(0.04)).color=color(1,1,1);

distaxislast=-10;
doalign=false;
showmarsglobe=false;trackmarspos=false;

showmarsglobe=false;
marsglobeframe.visible=showmarsglobe;
marspointframe.visible=not(showmarsglobe);

ssysframe.visible=false;
SUB_showloading(false);
ssysframe.visible=true;

zoomfact=1;
zoomingin=false;zoomingout=false;
zoomfrac=0;

dist=autokm(5);


root.time=CurrentTimeUT;
root.TimeSpeed=24*60*60/2000;
incrspeedsign=1;
while true do {
   incrspeed=JoystickGetAxisCorrected(JID,0);
#   root.TimeSpeed=incrspeedsign*sqr(incrspeed)*sqr(incrspeed)*0.8712*24*60*60;
   root.TimeSpeed=sign(incrspeed)*(exp(1+10*abs(incrspeed))-exp(1))/(exp(11)-exp(1))   *0.8712*24*60*60;
   info_clock.visible=(abs(root.timespeed)<30*60);

   incrtime;
   
   #distance throttle control
   distaxis=JoystickGetAxisCorrected(JID,1);
   if abs(distaxis)>0.3 then {
      distaxis=sign(distaxis)*(abs(distaxis)-0.3)*(abs(distaxis)-0.3);
      dist=dist*(1-distaxis/30);
      if dist<autokm(1.5) then dist=autokm(1.5);
      if dist>autokm(20) then dist=autokm(20);
   }
   myviewport.camerapos=point(0,0,0)+dist/zoomfact*vecnorm(myviewport.camerapos-point(0,0,0));
   
   JoyAxis1=JoystickGetAxis(JID,3);if abs(JoyAxis1)<0.02 then JoyAxis1=0;
   JoyAxis1=JoyAxis1/60;
   JoyAxis2=JoystickGetAxis(JID,2);if abs(JoyAxis2)<0.02 then JoyAxis2=0;
   JoyAxis2=-1*JoyAxis2/60;
   JoyAxis3=0;
#   JoyAxis3=JoystickGetAxis(JID,1);if abs(JoyAxis3)<0.02 then JoyAxis3=0;
#   JoyAxis3=-1*JoyAxis3/60;
   
   
   
   
   if (not(JoyStickButtonDown(0,25))) then {
      rtdir=myviewport.cameraupdir;
      myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,JoyAxis1);
      myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,JoyAxis1);
      
      rtdir=vecnorm(myviewport.cameradir*myviewport.cameraupdir);
      ok=true;
      aa=myviewport.cameradir^myviewport.cameraupdir;
      if (JoyAxis2>0) and (aa>0.975) then ok=false;
      if (JoyAxis2<0) and (aa<-0.975) then ok=false;
      if ok then {
         myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,JoyAxis2);
         myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,JoyAxis2);
      }
 
   
   }
   
  
   info_time.content=datestring(root.time);

   if JoyStickButtonClicked(0,2) then root.time=CurrentTimeUT;

   if JoyStickButtonClicked(0,1) then {
      showmarsglobe=not(showmarsglobe);
      marsglobeframe.visible=showmarsglobe;
      marspointframe.visible=not(showmarsglobe);
      sundirframe.visible=showmarsglobe;
   }
   
   if JoyStickButtonClicked(0,0) then {
      trackmarspos=not(trackmarspos);
      if trackmarspos then {
         zoomingin=true;
         zoomfrac=0;
      } else {
         zoomingout=true;
         zoomfrac=1;
      }
   }
   
   if JoyStickButtonClicked(0,3) then {
      ptsframe.visible=not(ptsframe.visible);
   }
   
   
   
   if zoomingin then {
      zoomfrac=zoomfrac+0.01;
      if zoomfrac>1 then { zoomfrac=1; zoomingin=false; }
      zoomfact=1+4*zoomfrac;
   }
   if zoomingout then {
      zoomfrac=zoomfrac-0.01;
      if zoomfrac<0 then { zoomfrac=0; zoomingout=false; }
      zoomfact=1+4*zoomfrac;
   }
   
   if JoyStickButtonClicked(0,20) then incrspeedsign=+1;
   if JoyStickButtonClicked(0,22) then incrspeedsign=-1;
   
   
#   if JoyStickButtonClicked(0,8) then {
#      showconstellations=not(showconstellations);
#      ssys_showconstellations(showconstellations);
#   }

   sundirframe.transf.Xaxis=vecnorm(point(0,0,0)-marspointframe.totaltransformation.origin);
   sundirframe.transf.Yaxis=sundirframe.transf.Xaxis*sundirframe.transf.Zaxis;

#mechanism to co-move with position on Mars
   root.SC.Universe.transf.reset;
   tf1=Transformation;
   tf1.translate(zoomfrac*@vector(marspositframe.totaltransformation.origin));
   tf2=tf1;tf2.invert;
   root.viewports.main.transf=tf2;
   
   SUB_testcmd();
   render;
}