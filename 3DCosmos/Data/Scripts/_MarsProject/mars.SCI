codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_CreateMarsGeoGlobe.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_CreateMarsSurface.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_CreateMarsSolarMovement.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_waypoints.sci");

mountains=list;
st=readtextfile(ScriptDir+"\MarsProject\Data\mountains.txt");
st.split("~n");
while st!="" do {
   st1=st.split("~n");
   mp=map;
   mp.name=st1.split("~t");
   mp.lattit=ToScalar(st1.split("~t"));
   mp.longit=ToScalar(st1.split("~t"));
   mountains.add(mp);
}

landers=list;
st=readtextfile(ScriptDir+"\MarsProject\Data\landingsites.txt");
st.split("~n");
while st!="" do {
   st1=st.split("~n");
   mp=map;
   mp.name=st1.split("~t");
   mp.lattit=ToScalar(st1.split("~t"));
   mp.longit=ToScalar(st1.split("~t"));
   landers.add(mp);
}


function calclongitlattit(pos,longit,lattit)
{
   pos=vecnorm(pos);
   lattit=asin(pos.z);
   longit=Pi+angle(pos.x,pos.y);
   while longit>Pi do longit=longit-2*Pi;
   while longit<-1*Pi do longit=longit+2*Pi;
}

winddirs=list("N","NO","O","ZO","Z","ZW","W","NW");

function G_dirinfo(pt1,pt2)
{
   info=map;
   vc1=vecnorm(@vector(pt1));
   vc2=vecnorm(@vector(pt2));
   info.dist=vecangle(vc1,vc2)*G_MarsRadius;
   dd_z=vc1;
   dd_x=vecnorm(vector(0,0,1)*dd_z);
   dd_y=vecnorm(dd_z*dd_x);
   cmpx=(vc2-vc1)^dd_x;
   cmpy=(vc2-vc1)^dd_y;
   info.dir=angle(cmpy,cmpx);
   diridx=floor(info.dir/(2*Pi)*8+0.5);
   if diridx>=8 then diridx=diridx-8;
   info.winddir=winddirs(diridx);
   return(info);
}



#Create solar system
ssysframe=ssys_create;
sss=T_getscene;
myviewport=T_getviewport;
SUB_init;
PanelLeft=SUB_createleftpanel;
SUB_showloading(true);


ssys_showconstellations(false);
ssys_createearth(1000);
ssys_createmilkyway;
ssys_createconstellationnames;
ssys_createstarnames;
root.SC.Universe.StarBackFrame.ConstellationNames.Visible=false;
root.SC.Universe.StarBackFrame.StarNames.Visible=false;
marsframe=ssys_createplanet("Mars",1000);
GetPlanetOrbit("Mars").visible=false;
#sun reference
ssysframe.add("Sphere","Radius":autokm(0.02),"EnableLight":false);



sss.ambientlightcolor=color(0.05,0.05,0.05);
myviewport.NearClipPlane=0.01;
myviewport.FarClipPlane=10;
myviewport.focaldistance=0.0101;
myviewport.EyeSeparation=0.0001;
myviewport.enableusernavigation=false;
JID=0;#joystick ID

rootframe=sss.Universe;

resol=60;

#Create Mars globe
globeframe=rootframe.addsubframe("MarsGlobe");
waypointsframe=globeframe.addsubframe("WayPoints");
flagsframe=globeframe.addsubframe("Flags");
flagsframe.CreateTexture("Flag",ScriptDir+"\MarsProject\Bitmaps\flag.bmp");

surf=globeframe.add("sphere","radius":1,"position":point(0,0,0),"resolution":resol);
surf.canbuffer=true;
tx1=globeframe.createtexture("mars",DataDir+"\textures\mars3.jpg");
tx3=globeframe.createtexture("MarsTopo",DataDir+"\textures\MarsTopo.jpg");
#tx1=globeframe.createtexture("mars",DataDir+"\textures\mars.bmp");
surf.texture=tx1.name;
curtexture=1;


#Create Mars geological globe
SUB_CreateMarsGeoGlobe(globeframe);
globeframe.MarsGlobeGeological.visible=false;


#create grid
gridframe=globeframe.addsubframe("grid");
gridframe.visible=false;
for i=-8 to 8 do {
   a=deg2rad(10*i);
   crv=gridframe.add("Curve");
   crv.makecircle(point(0,0,sin(a)),vector(0,0,1),1.002*cos(a),200);
   crv.CurveRenderType=CurveRenderSmooth;
   crv.Size=0.005;
   crv.color=color(1,0,0,0.5);
   if i==0 then { crv.Size=0.02; crv.color=color(1,0,0,1); }
}
for i=0 to 17 do {
   a=deg2rad(10*i);
   crv=gridframe.add("Curve");
   crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),1.002,200);
   crv.CurveRenderType=CurveRenderSmooth;
   crv.Size=0.005;
   crv.color=color(1,0,0,0.5);
   if (i==0) or (i==9) then { crv.Size=0.02; crv.color=color(1,0,0,1); }
}
#add mountains
foreach mp in mountains do {
   mpt=radial2point(1,deg2rad(180-mp.longit),deg2rad(mp.lattit));
   mu1=mpt-point(0,0,0);fc=0.025;
   mu1=-1*fc*vecnorm(mu1*vector(0,0,1));
   mu2=-1*fc*vecnorm(mu1*(mpt-point(0,0,0)));
   mu3=fc*(mpt-point(0,0,0));
   gridframe.add("sphere","radius":0.005,"position":mpt,"Color":color(0.3,0.3,0.3));
   gridframe.add("Text3D","position":(mpt+0.3*mu1-0.3*mu2),"Color":color(0,0,0),
      "Content":(mp.name),"UnitX":mu1,"UnitY":mu2,"EnableLight":false);
}
#add landingsites
foreach mp in landers do {
   mpt=radial2point(1,deg2rad(180-mp.longit),deg2rad(mp.lattit));
   mu1=mpt-point(0,0,0);fc=0.03;
   mu1=-1*fc*vecnorm(mu1*vector(0,0,1));
   mu2=-1*fc*vecnorm(mu1*(mpt-point(0,0,0)));
   mu3=fc*(mpt-point(0,0,0));
   gridframe.add("sphere","radius":0.005,"position":mpt,"Color":color(0,0.75,0.2));
   gridframe.add("Text3D","position":(mpt+0.3*mu1-0.3*mu2),"Color":color(0,0.75,0.2),
      "Content":(mp.name),"UnitX":mu1,"UnitY":mu2,"EnableLight":false);
}


#create info layer frame
PanelContentMain=PanelLeft.addscreenframe("PanelContentMain");
PanelLines_init(PanelContentMain,0.95);
PanelLines_add("Plaats op Mars:");
info_longit=PanelLines_add("");
info_lattit=PanelLines_add("");
PanelLines_skip;
PanelLines_add("Laatste route punt:");
info_dist=PanelLines_add("");
info_dir=PanelLines_add("");

myviewport.camerapos=radial2point(-10,0,0);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,0,1);

#create cursor point
cursorpos=point(-1,0,0);
cursor=globeframe.add("sphere","color":color(0,0.75,0),"radius":0.01);
cursor.position=cursorpos;
distaxislast=-100;

#create distance scale object
distscaleframe=globeframe.addsubframe("DistScale");
for i=1 to 10 do {
   ang=i*200/G_MarsRadius;
   crv=distscaleframe.add("Curve","Color":color(0,0.15,0,0.5),"Size":2);
   crv.BlendType=BlendTranslucent;
   crv.makecircle(point(0,0,cos(ang)-1),vector(0,0,1),1.002*sin(ang),100);
   if i/5==floor(i/5) then {
      crv.size=6;
      st=str(i*200)+"km";
      distscaleframe.add("Text3D","Position":point(1.01*sin(ang),0,1.01*cos(ang)-1),
         "UnitX":vector(0.03,0,0),"UnitY":Vector(0,0.03,0),
         "Content":st,"EnableLight":false,"Color":color(0,0.25,0));
   }
}
distscaleframe.visible=false;

#create windrose object
windrose=globeframe.addsubframe("WindRose");
windrosetx=windrose.CreateTexture("windrosetx",DataDir+"\textures\windrose1.jpg",DataDir+"\textures\windrose2.jpg");
windroserect=windrose.add("Rectangle","Position":point(-0.5,-0.5,0),"Axis1":vector(1,0,0),"Axis2":vector(0,1,0));
windroserect.texture=windrosetx.name;
windroserect.BlendType=BlendTranslucent;
windroserect.color=color(1,1,0.7,0.75);
windroserect.enablelight=false;
windroserect.DepthMask=DepthMaskDisable;
windrose.visible=false;

#create topo scale
topocols=list;topovals=list;
topocols.add(color(0/255,20/255,80/255));     topovals.add(-8);
topocols.add(color(1/255,110/255,255/255));   topovals.add(-4);
topocols.add(color(1/255,200/255,200/255));   topovals.add(0);
topocols.add(color(50/255,255/255,50/255));   topovals.add(4);
topocols.add(color(201/255,78/255,1/255));    topovals.add(8);
topocols.add(color(254/255,0/255,75/255));    topovals.add(12);
topocols.add(color(255/255,214/255,230/255)); topovals.add(16);
toposcale=PanelContentMain.addscreenframe("TopoScale");
toposcale.enablelight=false;
yp=0.25;xp=SUB_G_leftpaneloffsett+0.02;
for i=0 to topocols.size-1 do {
   toposcale.add("Rectangle","position":point(xp,yp,0),"Axis1":vector(0.02,0,0),"Axis2":vector(0,0.02,0),"Color":topocols(i));
   toposcale.add("Text3D","position":point(xp+0.03,yp+0.003,0),"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Color":SUB_G_sidepanelcolor,"Content":(str(1000*topovals(i))+"m"));   yp=yp-0.03;
}
toposcale.visible=false;



#load waypoints
loadwaypoints;
dispwaypoints(waypointsframe,1.0);
loadflags;
dispflags(flagsframe,1.0);


SUB_showloading(false);

doalign=false;
ct=0;
distfac=1;dist=2;
while true do {
   ct=ct+1;
   cursorposchanged=false;distinfochanged=false;
   
   #distance throttle control
   distaxis=JoystickGetAxis(JID,3);
   if abs(distaxislast-distaxis)>0.015 then {
      distaxislast=distaxis;
      dist=1.4+1.1*sqr(1+distaxislast);
      myviewport.camerapos=point(0,0,0)+dist*vecnorm(myviewport.camerapos-point(0,0,0));
      distfac=dist/3;
      cursorposchanged=true;
   }
   
   JoyAxis1=JoystickGetAxis(JID,0);if abs(JoyAxis1)<0.02 then JoyAxis1=0;
   JoyAxis1=distfac*JoyAxis1/60;
   JoyAxis2=JoystickGetAxis(JID,1);if abs(JoyAxis2)<0.02 then JoyAxis2=0;
   JoyAxis2=-1*distfac*JoyAxis2/60;
   JoyAxis3=JoystickGetAxis(JID,2);if abs(JoyAxis3)<0.02 then JoyAxis3=0;
   JoyAxis3=JoyAxis3/60;
   
   
   if JoyStickButtonDown(0,5) then doalign=true;#Align to North
   if doalign then {
      cdir=vector(0,0,1)-(vector(0,0,1)^myviewport.cameradir)*myviewport.cameradir;
      ang=vecangle(cdir,myviewport.cameraupdir);
      if abs(ang)>0.01 then {
         if (vector(0,0,1)*myviewport.cameraupdir)^myviewport.cameradir<0 then ang=-1*ang;
         JoyAxis3=-0.02*(ang+ang/abs(ang));
      } else doalign=false;
   }
   
   
   if (not(JoyStickButtonDown(0,25))) then {#Move planet by joystick
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
   
   
   sss.Light0Pos=point(0,0,0)-100*myviewport.cameradir
      +30*myviewport.cameraupdir
      -40*myviewport.cameradir*myviewport.cameraupdir;
      
      
   if JoystickButtonClicked(JID,9) and (curtexture!=1) then {#Show image texture
      curtexture=1;
      surf.visible=true;
      globeframe.MarsGlobeGeological.visible=false;
      surf.texture=tx1.name;
      toposcale.visible=false;
   }
   if JoystickButtonClicked(JID,11) and (curtexture!=2) then {#Show geological texture
      curtexture=2;
      surf.visible=false;
      globeframe.MarsGlobeGeological.visible=true;
      toposcale.visible=false;
   }
   if JoystickButtonClicked(JID,13) and (curtexture!=3) then {#Show topographic texture
      curtexture=3;
      surf.visible=true;
      globeframe.MarsGlobeGeological.visible=false;
      surf.texture=tx3.name;
      toposcale.visible=true;
   }
   
   
   if JoystickButtonClicked(JID,8) then gridframe.visible=not(gridframe.visible);
   if JoystickButtonClicked(JID,10) then windrose.visible=not(windrose.visible);
   if JoystickButtonClicked(JID,12) then distscaleframe.visible=not(distscaleframe.visible);
   
   #process cursor navigation
   if (JoyStickButtonDown(JID,23)) then {#Auto-center cursor mode
      cursorpos=point(0,0,0)-vecnorm(myviewport.cameradir);
      cursorposchanged=true;
   }
   if (JoyStickButtonDown(JID,24)) then {#Move cursor by rocker mode
      rangle=JoystickRockerPos(0);
      if rangle<=360 then {
         rangle=deg2rad(rangle);
         dr1=myviewport.cameraupdir;
         dr2=vecnorm(dr1*myviewport.cameradir);
         cursorpos=@point(vecnorm(@vector(cursorpos+0.005*(cos(rangle)*dr1-sin(rangle)*dr2))));
         cursorposchanged=true;
      }
   }
   if (JoyStickButtonDown(JID,25)) then {#Move cursor by joystick mode
      rtdir=myviewport.cameraupdir;
      cursorpos=point(0,0,0)+vecrotate(cursorpos-point(0,0,0),rtdir,JoyAxis1);
      rtdir=vecnorm(myviewport.cameradir*myviewport.cameraupdir);
      cursorpos=point(0,0,0)+vecrotate(cursorpos-point(0,0,0),rtdir,JoyAxis2);
      cursorposchanged=true;
   }
   cursor.position=cursorpos;
   loc_z=vecnorm(cursorpos-point(0,0,0));
   loc_x=vecnorm(vector(0,0,1)*loc_z);
   loc_y=vecnorm(loc_z*loc_x);
   
   if cursorposchanged then {#re-position windrose
      cursorsize=0.03+0.3*distfac;
      windrose.transf.origin=@point(1.001*@vector(cursorpos));
      windrose.transf.Xaxis=cursorsize*loc_x;
      windrose.transf.Yaxis=cursorsize*loc_y;
      windrose.transf.Zaxis=cursorsize*loc_z;
   }
   
   if cursorposchanged then {#re-position distance scale
      distscaleframe.transf.origin=@point(1.002*@vector(cursorpos));
      distscaleframe.transf.Xaxis=loc_x;
      distscaleframe.transf.Yaxis=loc_y;
      distscaleframe.transf.Zaxis=loc_z;
   }
   
   if cursorposchanged then distinfochanged=true;
   
   #add new waypoint
   if JoystickButtonClicked(JID,3) and not(JoystickButtonDown(JID,0)) then {
      ok=true;
      if G_waypointcount>0 then
         if (G_waypoint(G_waypointcount-1)-cursorpos).size<0.02 then ok=false;
      if ok then {
         addwaypoint(cursorpos);
         dispwaypoints(waypointsframe,1);
         distinfochanged=true;
      }
   }
   
   #remove waypoint
   if JoystickButtonClicked(JID,3) and JoystickButtonDown(JID,0) then {
      dellastwaypoint;
      dispwaypoints(waypointsframe,1);
      distinfochanged=true;
   }

   #add new flag
   if JoystickButtonClicked(JID,2) and not(JoystickButtonDown(JID,0)) then {
      ok=true;
      if G_flagcount>0 then
         if (G_flagposit(G_flagcount-1)-cursorpos).size<0.02 then ok=false;
      if ok then {
         addflag(cursorpos);
         dispflags(flagsframe,1);
      }
   }

   #remove flag
   if JoystickButtonClicked(JID,2) and JoystickButtonDown(JID,0) then {
      dellastflag;
      dispflags(flagsframe,1);
   }
   
   #remove all flags and waypoints
   if JoystickButtonLongClicked(JID,1) then {
      delallwaypoints;
      delallflags;
      dispwaypoints(waypointsframe,1);
      dispflags(flagsframe,1);
      distinfochanged=true;
   }
   
   
   if distinfochanged then {#update cursor distance info
      if G_waypointcount>0 then {
         info=G_dirinfo(cursorpos,G_waypoint(G_waypointcount-1));
         info_dist.content="Afstand: "+str(round(info.dist,0))+"km";
         info_dir.content="Richting: "+str(round(info.dir/Pi*180,0))+"° "+info.winddir;
      }
      else {
         info_dist.content="";
         info_dir.content="";
      }
   }
   
   
   #blink cursor
   if ct/30-floor(ct/30)<0.3 then cursor.visible=false; else cursor.visible=true;
   
   
  longit=0;lattit=0;
  calclongitlattit(@vector(cursorpos),Ref(longit),Ref(lattit));
  info_longit.content="Lengte="+Longit2string(longit);
  info_lattit.content="Breedte="+lattit2string(lattit);
   
   
   SUB_testcmd();
   render;
   
   
   #show terrain
   if JoystickButtonClicked(JID,7) then {
      oldcamerapos=myviewport.camerapos;
      oldcameradir=myviewport.cameradir;
      oldcameraupdir=myviewport.cameraupdir;
      globeframe.visible=false;PanelContentMain.visible=false;
      rs1=300;rs2=300;
      sss.Universe.StarBackFrame.visible=false;
      SUB_RenderMarsSurface(rootframe,longit,lattit,rs1,rs2);
      sss.Universe.StarBackFrame.visible=true;
      globeframe.visible=true;PanelContentMain.visible=true;
      myviewport.camerapos=oldcamerapos;
      myviewport.cameradir=oldcameradir;
      myviewport.cameraupdir=oldcameraupdir;
   }
   
   #show sun movement
   if JoystickButtonClicked(JID,6) then {
      oldcamerapos=myviewport.camerapos;
      oldcameradir=myviewport.cameradir;
      oldcameraupdir=myviewport.cameraupdir;
      oldnearclipplane=myviewport.nearclipplane;
      oldfarclipplane=myviewport.farclipplane;
      oldfocaldistance=myviewport.focaldistance;
      oldeyeseparation=myviewport.EyeSeparation;
      globeframe.visible=false;PanelContentMain.visible=false;
      root.time=CurrentTimeUT;root.time.hour=0;root.time.min=0;root.time.sec=0;
      SUB_MarsSolarMovementAnimate(longit,lattit);
      globeframe.visible=true;PanelContentMain.visible=true;
      myviewport.camerapos=oldcamerapos;
      myviewport.cameradir=oldcameradir;
      myviewport.cameraupdir=oldcameraupdir;
      myviewport.nearclipplane=oldnearclipplane;
      myviewport.farclipplane=oldfarclipplane;
      myviewport.focaldistance=oldfocaldistance;
      myviewport.EyeSeparation=oldeyeseparation;
   }

   #show route
   if JoystickButtonClicked(JID,29) then {
      globeframe.visible=false;#PanelContentMain.visible=false;
      sss.Universe.StarBackFrame.visible=false;
      render;
      repframe=sss.Universe.addscreenframe("RepFrame");
      repframe.EnableLight=false;repframe.Color=color(0.7,0.6,0.5);
      yp=0.95;xp1=0.4;xp2=0.7;sz=0.02;
      lo=0;la=0;
      for i=0 to G_waypointcount-1 do {
         calclongitlattit(@vector(G_waypoint(i)),Ref(lo),Ref(la));
         st=longit2string(lo)+", ";
         st=st+lattit2string(la);
         line=repframe.add("Text3D","position":point(xp1,yp,0),"UnitX":vector(sz,0,0),"UnitY":vector(0,sz,0),"Content":st);
         yp=yp-sz;
         if i<G_waypointcount-1 then {
            info=G_dirinfo(G_waypoint(i),G_waypoint(i+1));
            st="Afstand: "+str(round(info.dist,0))+"km,";
            st=st+"   Richting: "+str(round(info.dir,1))+"° "+info.winddir;
            line=repframe.add("Text3D","position":point(xp2,yp,0),"UnitX":vector(sz,0,0),"UnitY":vector(0,sz,0),"Content":st);
            yp=yp-sz;
         }
      }
      finished=false;
      while not(finished) do {
         SUB_testcmd();
         if JoystickButtonClicked(JID,29) then finished=true;
         render;
      }
      repframe.dispose;
      globeframe.visible=true;#PanelContentMain.visible=true;
      sss.Universe.StarBackFrame.visible=true;
   }
   
   
}
