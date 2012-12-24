
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
codeblock readtextfile(ScriptDir+"\SSYS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");

JID=0;



function formatlargenumber(vl)
{
   rs=str(round(vl,0));
   while rs.length<9 do rs=" "+rs;
#   rs="123456789";
   rs=rs.substring(0,5)+"."+rs.substring(6,99);
   rs=rs.substring(0,2)+"."+rs.substring(3,99);
   while rs.substring(0,0)==" " do rs=rs.substring(1,99);
   while rs.substring(0,0)=="." do rs=rs.substring(1,99);
   return(rs);
}


ssysframe=ssys_create;
SUB_init;
PanelLeft=SUB_createleftpanel;
SUB_showloading(true);


ssys_showconstellations(false);
ssys_createearth(1000);
ssys_createmilkyway;


myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.5);
myviewport.FarClipPlane=autokm(80);
myviewport.FocalDistance=myviewport.NearClipPlane*1.05;
myviewport.EyeSeparation=0.0001;
myviewport.enableusernavigation=false;
myviewport.cameraupdir=vecnorm(myviewport.cameradir*vector(1,0,0));


pttx=ssysframe.createtexture("point",DataDir+"\textures\star1.bmp");
function addlightpoint(frame,sz)
{
   sz=sz/2;
   rc=frame.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0));
   rc.Texture=pttx.name;
   rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   return(rc);
}


#tabulate earth position
tab_earthpos=list;
tab_earthang=list;
tabtm=time(2008,1,1,0,0,0);
eanglast=-100;
root.time=tabtm;
earthpos0=GetPlanetPosition("Earth");
earthang0=angle(earthpos0.x,earthpos0.y);
for i=0 to 400 do {
   root.time=tabtm;
   epos=GetPlanetPosition("Earth");
   eang=angle(epos.x,epos.y)-earthang0;
   while eang<0 do eang=eang+2*Pi;
   while eang<eanglast do eang=eang+2*Pi;
   tab_earthpos.add(epos);
   tab_earthang.add(eang);
   tabtm.adddays(1);
   eanglast=eang;
}

function find_earthpos(ang)
{
   ang=ang-earthang0;
   while ang<0 do ang=ang+2*Pi;
   while ang>2*Pi do ang=ang-2*Pi;
   found=false;
   idx=0;
   createvar(rspos);
   while not(found) do {
      if (tab_earthang(idx)<=ang) and (tab_earthang(idx+1)>ang) then {
         found=true;
         angdiff=tab_earthang(idx+1)-tab_earthang(idx);
         fr1=(tab_earthang(idx+1)-ang)/angdiff;
         fr2=(ang-tab_earthang(idx))/angdiff;
         rspos=fr1*tab_earthpos(idx)+fr2*tab_earthpos(idx+1);
      }
      idx=idx+1;
   }
   return(rspos);
}


#rs=find_earthpos(5);
#ai=angle(rs.x,rs.y);
#break;


#sun reference
#ssysframe.add("Sphere","Radius":autokm(0.02),"EnableLight":false);
ssysframe.add("LightPoint","Size":autokm(0.2),"Color":color(1,0.5,0));
ssysframe.add("LightPoint","Size":autokm(0.09),"Color":color(1,1,0.5));
ssysframe.add("LightPoint","Size":autokm(0.04),"Color":color(1,1,1));

#create Mars object
marsframe=ssys_createplanet("Mars",300);
marsframe.inclin.dispose;
textframe=marsframe.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.05)).color=color(0.3,0.3,1);
addlightpoint(textframe,autokm(0.03)).color=color(0,1,1);
addlightpoint(textframe,autokm(0.01)).color=color(1,1,1);
txt=textframe.add("TextFlat","Position":point(autokm(-0.03),autokm(0.03),0),"Content":"Mars","Size":autokm(0.06));


marsorbit=GetPlanetOrbit("Mars");
marsorbit.color=color(0,0.5,1,0.6);
marsorbit.BlendType=BlendTransparent;
marsorbit.DepthMask=DepthMaskDisable;

#create Earth object
earthframe=GetPlanetFrame("Earth");
earthframe.inclin.dispose;
textframe=earthframe.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.05)).color=color(0.3,0.3,1);
addlightpoint(textframe,autokm(0.03)).color=color(0,1,1);
addlightpoint(textframe,autokm(0.01)).color=color(1,1,1);
txt=textframe.add("TextFlat","Position":point(autokm(-0.033),autokm(0.03),0),"Content":"Aarde","Size":autokm(0.06));

earthorbit=GetPlanetOrbit("Earth");
earthorbit.color=color(0,0.5,1,0.6);
earthorbit.BlendType=BlendTransparent;
earthorbit.DepthMask=DepthMaskDisable;


infolinecolor=color(0.8,0.8,0.8);

clockframe=PanelLeft.addscreenframe("ClockFrame");
PanelLines_init(clockframe,0.96);
info_time=PanelLines_add("");
info_time.unitx=1.25*info_time.unitx;
info_time.unity=1.6*info_time.unity;
info_time.color=color(0.6,0.8,1);

travelframe=PanelLeft.addscreenframe("TravelInfo");
PanelLines_init(travelframe,0.90);
simulsign=PanelLines_add("Simulatie reistraject");
simulsign.Color=color(1,0.5,0.5);
simulsign.unitX=1.1*simulsign.UnitX;
simulsign.unitY=1.3*simulsign.UnitY;

PanelLines_skip;
PanelLines_add("Start van de reis:");
info_travelstart=PanelLines_add("");
info_travelstart.color=infolinecolor;
PanelLines_skip;
PanelLines_add("Reisduur:");
info_traveltime=PanelLines_add("");
info_traveltime.color=infolinecolor;

PanelLines_init(travelframe,0.58);
PanelLines_add("Snelheid:");
info_speed=PanelLines_add("");
info_speed.color=infolinecolor;
PanelLines_add("T.o.v. Mars:");
info_diffspeedmars=PanelLines_add("");
info_diffspeedmars.color=infolinecolor;
PanelLines_add("T.o.v. Aarde:");
info_diffspeedearth=PanelLines_add("");
info_diffspeedearth.color=infolinecolor;

PanelLines_init(travelframe,0.20);
succesline=PanelLines_add("Reis voltooid!");
succesline.unitX=1.5*succesline.UnitX;succesline.unitY=1.6*succesline.UnitY;
succesline.color=color(0,1,0);

PanelLines_init(travelframe,0.15);
PanelLines_add("Afstand tot Aarde:");
info_distearth=PanelLines_add("");
info_distearth.color=infolinecolor;
info_distearth.unitX=1.2*info_distearth.UnitX;info_distearth.unitY=1.3*info_distearth.UnitY;
PanelLines_skip;
PanelLines_add("Afstand tot Mars:");
info_distmars=PanelLines_add("");
info_distmars.unitX=1.2*info_distmars.UnitX;info_distmars.unitY=1.3*info_distmars.UnitY;
info_distmars.color=infolinecolor;

travelframe.visible=false;


tmstart=CurrentTimeUT;
tmstart=time(2009,9,4,0,0,0);

root.time=tmstart;
root.TimeSpeed=0;


#create spacecraft
startpos=point(0,0,0);
obj=ssysframe.addsubframe("Obj");
textframe=obj.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.05)).color=color(1,0.3,0.3);
addlightpoint(textframe,autokm(0.03)).color=color(1,0.7,0);
addlightpoint(textframe,autokm(0.01)).color=color(1,1,1);
orb=MotionKepler.create(obj);
Tcraft=0;

function createorbit()
{
   ang=angle(marsframe.transf.origin.x,marsframe.transf.origin.y);
   
   startpos=marsframe.transf.origin;
   endpos=find_earthpos(ang+Pi);
   
   startdist=distance(point(0,0,0),startpos);
   enddist=distance(point(0,0,0),endpos);
      
   #Aearth=1.000001018;
   Aearth=kmtoau(enddist);
   Tearth=365.2524;
   #Amars=1.523679;
   Amars=kmtoau(startdist);
   Tmars=360.0/(0.9856076686/(Amars^1.5));
   
   Acraft=(Aearth+Amars)/2;
   Tcraft=360.0/(0.9856076686/(Acraft^1.5));
   ecraft=(Amars-Aearth)/(Amars+Aearth);
   
   
   orb.SemiMajorAxis=autokm(Acraft);
   orb.Eccentricity=ecraft;
   orb.Inclination=0;
   orb.AscendingNode=ang;
   orb.ArgPerigee=Pi;
   orb.Period=Tcraft;
   peritm=root.time;peritm.AddDays(Tcraft/2);
   orb.PeriTime=peritm;
}

objorb=ssysframe.add("Curve","Name":"CraftOrbit","Color":color(1,0.5,0,0.6),"Size":2);
objorb.BlendType=BlendTransparent;
objorb.DepthMask=DepthMaskDisable;


#create pointers
marspointer=ssysframe.add("Curve","Size":2,"Color":color(0,0.5,1),"Arrow2Size":autokm(0.07));
marspointer.makeline(point(0,0,autokm(0.15)),point(0,0,0));
earthpointer=ssysframe.add("Curve","Size":2,"Color":color(0,0.5,1),"Arrow2Size":autokm(0.07));
earthpointer.makeline(point(0,0,autokm(0.15)),point(0,0,0));
marspointer.visible=false;
earthpointer.visible=false;

ssysframe.visible=false;
SUB_showloading(false);
ssysframe.visible=true;
distaxislast=-10;
ctr=0;simulctr=0;
craftstartpos=point(0,0,0);
simulating=false;
doalign=false;
createvar(simulstarttime);createvar(simulcompleted);
createvar(orbtracing);createvar(orbtracingbusy);
while true do {



   #distance throttle control
   distaxis=JoystickGetAxis(JID,3);
   if abs(distaxislast-distaxis)>0.015 then {
      distaxislast=distaxis;
      dist=autokm(2+2*sqr(1+distaxislast));
      myviewport.camerapos=point(0,0,0)+dist*vecnorm(myviewport.camerapos-point(0,0,0));
   }
   
   JoyAxis1=JoystickGetAxis(JID,0);if abs(JoyAxis1)<0.02 then JoyAxis1=0;
   JoyAxis1=JoyAxis1/60;
   JoyAxis2=JoystickGetAxis(JID,1);if abs(JoyAxis2)<0.02 then JoyAxis2=0;
   JoyAxis2=-1*JoyAxis2/60;
   JoyAxis3=JoystickGetAxis(JID,2);if abs(JoyAxis3)<0.02 then JoyAxis3=0;
   JoyAxis3=-1*JoyAxis3/60;

   if JoyStickButtonDown(0,5) then doalign=true;#Align to North
   if doalign then {
      cdir=vecnorm(vector(0,0,1)-(vector(0,0,1)^myviewport.cameradir)*myviewport.cameradir);
      ang=vecangle(cdir,myviewport.cameraupdir);
      if abs(ang)>0.01 then {
         if (vector(0,0,1)*myviewport.cameraupdir)^myviewport.cameradir<0 then ang=-1*ang;
         JoyAxis3=-0.02*(ang+ang/abs(ang));
      } else doalign=false; 
   }
   
   
   if (not(JoyStickButtonDown(JID,4))) then {#Move scene by joystick
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
   }

   #time control
   if not(simulating) then {
      if JoystickButtonDown(JID,4) then
         root.TimeSpeed=-1*sign(JoyAxis2)*sqr(40*JoyAxis2)*1.5*24*60*60;
      else root.TimeSpeed=0;
   } else {
      if not(simulcompleted) then {
         incrspeed=(JoystickGetAxis(JID,4)+1)/2;
         root.TimeSpeed=sqr(incrspeed)*1.5*24*60*60;
      } else root.timespeed=0;
   }
   
   if JoystickButtonClicked(JID,7) and not(simulating) then {#Start simulation
      orbtracing=true;orbtracingbusy=false;
      simulating=true;simulcompleted=false;
      simulctr=0;
      simulstarttime=root.time;
      createorbit;
      obj.visible=true;
      marspointer.position=marsframe.totaltransformation.origin;
      earthpointer.position=earthframe.totaltransformation.origin;
      marspointer.visible=true;earthpointer.visible=true;succesline.visible=false;
      travelframe.visible=true;
      info_travelstart.content=datestring(simulstarttime);
   }
   
   incrtime;
   
   earthpos=GetPlanetPosition("Earth");
   marspos=GetPlanetPosition("Mars");
   earthspeed=root.SC.Universe.SolarSystem.Earth.Motions.EarthOrbit.speed;
   marsspeed=root.SC.Universe.SolarSystem.Mars.Motions.MarsOrbit.speed;
   
   if simulating then {
      simulctr=simulctr+1;
      simulsign.visible=(simulctr/50-floor(simulctr/50)<0.7);
      durat=root.time.DiffDays(tmstart);
      orbpos=orb.position;
      orbspeed=orb.speed;
      spd=orb.speed.size/(24*60*60);
      zcorr=startpos.z*sqr(cos(Pi*durat/Tcraft));
      craftpos=point(orbpos.x,orbpos.y,zcorr);
      if simulctr<=1 then craftstartpos=craftpos;
      obj.transf.origin=craftpos;
      if orbtracing then {
         if orbtracingbusy and (distance(craftpos,craftstartpos)<autokm(0.02)) then {
            orbtracing=false;
            objorb.IsClosed=true;
         } else objorb.addpoint(craftpos);
         if distance(craftpos,craftstartpos)>autokm(0.06) then orbtracingbusy=true;
      }
      dst=distance(craftpos,earthpos);
      info_distearth.content=formatlargenumber(dst)+" km";
      info_distearth.position.x=0.11-info_distearth.GetSizeX*info_distearth.unitX.size;
      if dst<1000000 then {
         simulcompleted=true;
         succesline.visible=true;
      }
   
      dst=distance(craftpos,marspos);
      info_distmars.content=formatlargenumber(dst)+" km";
      info_distmars.position.x=0.11-info_distmars.GetSizeX*info_distmars.unitX.size;
      
      info_traveltime.content=str(round(durat,0))+" dagen";
      
      info_speed.content=ScalarToString(spd,0,1)+" km/s";
      info_speed.position.x=0.07-info_speed.GetSizeX*info_speed.unitX.size;
      diffspeed=(marsspeed-orbspeed).size/(24*60*60);
      info_diffspeedmars.content=ScalarToString(diffspeed,0,1)+" km/s";
      info_diffspeedmars.position.x=0.07-info_diffspeedmars.GetSizeX*info_diffspeedmars.unitX.size;
      
      diffspeed=(earthspeed-orbspeed).size/(24*60*60);
      info_diffspeedearth.content=ScalarToString(diffspeed,0,1)+" km/s";
      info_diffspeedearth.position.x=0.07-info_diffspeedearth.GetSizeX*info_diffspeedearth.unitX.size;
      
      if (simulctr>10) and (JoystickButtonClicked(JID,7)) then {#stop simulation
         simulating=false;
         root.time=simulstarttime;
         obj.visible=false;
         objorb.reset;
         marspointer.visible=false;earthpointer.visible=false;
         travelframe.visible=false;
      }
   }
   
   info_time.content=datestring(root.time);

   
   SUB_testcmd();
   render;
   ctr=ctr+1;
}