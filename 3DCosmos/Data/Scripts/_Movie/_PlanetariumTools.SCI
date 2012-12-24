codeblock readtextfile(ScriptFilePath+"\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_AnimateTools.sci");
codeblock readtextfile(ScriptFilePath+"\_ConstellHighlight.sci");
codeblock readtextfile(ScriptFilePath+"\_CamMoveTools.sci");

###########################################################################################################
###########################################################################################################
###########################################################################################################


#createvar(rdd);
function InitPlanetarium(mydata)
{
   mydata.clipbelowhorizon=false;
   mydata.animateatmosphere=true;
   
   mydata.comoving=false;
   mydata.comovefrac=0;

   ei=+23.4457889/180*Pi;
   
   ssysframe=ssys_create;
   ssys_createearth(500);
   ssys_createluna(0.000005*mydata.scfac);
   root.framerate=30;
#   ssys_createluna(0.00001*mydata.scfac);
   
   #Create star globe
   ssys_showstarback(false);
   globeframe=root.SC.Universe.SolarSystem.Earth.addsubframe("StarGlobeFrame");
   tx=globeframe.createtexture("star",DataDir+"\textures\star4.bmp");
   st1=globeframe.add("StarGlobe","Name":"StarglobeFront");
   st1.radius=mydata.scfac;
   st1.texture="star";
   st1.StarSize=mydata.starsizefrac*st1.radius;
   st1.LineSize=mydata.starsizefrac*st1.radius;
   st1.linecolor=color(0,0.5,1,0.4);
   st1.renderfront=false;
   
   if true then
   {#Create milky way
      #galactic pole
      glong=179.32095/180*Pi;
      glatt=29.811954/180*Pi;
      ez=-1*vector(cos(glong)*cos(glatt),sin(glong)*cos(glatt),sin(glatt));
      #galactic center
      glong=266.14097/180*Pi;
      glatt=-5.52967943/180*Pi; 
      ex=vector(cos(glong)*cos(glatt),sin(glong)*cos(glatt),sin(glatt));
      ey=vecnorm(ez*ex);
      mwf=globeframe.addsubframe("MilkyWay");
      mwf.transf.Xaxis=-1*ex;
      mwf.transf.Yaxis=-1*ey;
      mwf.transf.Zaxis=ez;
      tx=mwf.createtexture("MilkyWay",DataDir+"\textures\milkyway.png");
      mw=mwf.add("sphere","EnableLight":false);
      mw.color=color(0.3,0.5,1,0.35);
      mw.texture=tx.name;
      mw.BlendType=BlendTransparent;mw.DepthMask=DepthMaskDisable;
      mw.renderback=true;mw.renderfront=false;
      mw.radius=mydata.scfac;
   }

   
   #Create moon halo object
   mydata.frame_moonhalo=root.SC.Universe.SolarSystem.Earth.addsubframe("MoonHalo");
   root.SC.Universe.SolarSystem.Earth.moveobject(mydata.frame_moonhalo.name,-1000);
   tx=mydata.frame_moonhalo.createtexture("halo",DataDir+"\textures\star1.bmp",DataDir+"\textures\star1.bmp");
   mydata.moonhalo=mydata.frame_moonhalo.add("Rectangle","EnableLight":false,
      "Color":color(0.7,0.7,0.7,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
   fc=mydata.scfac*0.8;
   mydata.moonhalo.Texture=tx.name;
   mydata.moonhalo.position=point(-1*fc,-1*fc,0);
   mydata.moonhalo.Axis1=vector(2*fc,0,0);
   mydata.moonhalo.Axis2=vector(0,2*fc,0);
   
   #Create sun halo object
   mydata.frame_sunhalo=root.SC.Universe.SolarSystem.Earth.addsubframe("SunHalo");
   root.SC.Universe.SolarSystem.Earth.moveobject(mydata.frame_sunhalo.name,-1000);
   tx=mydata.frame_sunhalo.createtexture("halo",DataDir+"\textures\star1.bmp");
   #large halo
   mydata.sunhalo=mydata.frame_sunhalo.add("Rectangle","Name":"Large","EnableLight":false,
      "Color":color(1,1,0.7,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
   fc=mydata.scfac*1;
   mydata.sunhalo.Texture=tx.name;
   mydata.sunhalo.position=point(-1*fc,-1*fc,0);
   mydata.sunhalo.Axis1=vector(2*fc,0,0);
   mydata.sunhalo.Axis2=vector(0,2*fc,0);
   #small halo
   rc=mydata.frame_sunhalo.add("Rectangle","Name":"Small","EnableLight":false,
      "Color":color(1,1,0.5,1.0),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
   fc=mydata.scfac*0.05;
   rc.Texture=tx.name;
   rc.position=point(-1*fc,-1*fc,0);
   rc.Axis1=vector(2*fc,0,0);
   rc.Axis2=vector(0,2*fc,0);
   #sunset/sunrise halo
   mydata.sunsethalo=mydata.frame_sunhalo.add("Rectangle","Name":"RiseSet","EnableLight":false,
      "Color":color(1,0.5,0.0,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
   fc=mydata.scfac*1;
   mydata.sunsethalo.Texture=tx.name;
   mydata.sunsethalo.position=point(-3*fc,-1*fc,0);
   mydata.sunsethalo.Axis1=vector(6*fc,0,0);
   mydata.sunsethalo.Axis2=vector(0,2*fc,0);
   
   
   #hide Earth orbit
   root.SC.Universe.SolarSystem.EarthOrbit.visible=false;
   
   
   #create planet objects
   mydata.frame_planets=root.SC.Universe.SolarSystem.Earth.addsubframe("PlanetsIndicators");
   root.SC.Universe.SolarSystem.Earth.moveobject(mydata.frame_planets.name,-1000);
   mydata.planlist=list("Mercurius","Venus","Mars","Jupiter","Saturnus");
   plsize=5*mydata.starsizefrac*mydata.scfac;
   for i=0 to mydata.planlist.size-1 do {
      ssys_createplanet(mydata.planlist.get(i),-1);
      GetPlanetOrbit(mydata.planlist.get(i)).visible=false;
      fr=GetPlanetFrame(mydata.planlist.get(i));
      fr.clearobjects;
      fr=mydata.frame_planets.addsubframe(mydata.planlist.get(i));
      fr.add("SolidPoint","Name":"Point","Size":plsize,"Color":color(1,1,0));
      fr.add("TextFlat","Name":"Name","Size":(1.2*plsize),"Color":color(1,1,0,0.5),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable,
         "Content":(" "+mydata.planlist.get(i)));
   }
   
   
   
   earthframe=GetPlanetBodyFrame("Earth");
   earthframe.GlobeRendering.visible=false;
   myviewport=T_getviewport;
   myviewport.EnableUserNavigation=false;
   
   #create viewer frame, but at earth center
   mydata.vframe=earthframe.addsubframe("ViewerFrame");
   mydata.vframe.transf.rotate(vector(0,1,0),Pi/2-mydata.lattit);
   mydata.vframe.transf.rotate(vecrotate(vector(0,0,1),vector(0,1,0),-1*(Pi/2-mydata.lattit)),mydata.longit);
   
   mydata.vframe.addsubframe("FogStart");
   mydata.fog1=mydata.vframe.get("FogStart").add("Fog");mydata.fog1.Color=color(0,0,0);
   mydata.fog1.Type=FogExponentialSq;mydata.fog1.Density=2.5/mydata.scfac;
   mydata.vframe.addsubframe("BackDropFrame");#this subframe should contain the backdrop
   mydata.vframe.addsubframe("FogStop");
   fog1=mydata.vframe.get("FogStop").add("Fog","Type":FogNone);

   
   #atmosphere above horizon
   if false then {
      mydata.atm1=mydata.vframe.add("Cylinder","Name":"AtmopshereGlow");
      mydata.vframe.createtexture("Atm1",DataDir+"\textures\gradient1.bmp",DataDir+"\textures\gradient1.bmp");
      mydata.atm1.renderback=true;mydata.atm1.renderfront=true;
      mydata.atm1.enablelight=false;
      mydata.atm1.Texture="Atm1";
      mydata.atm1.BlendType=BlendTransparent;mydata.atm1.DepthMask=DepthMaskDisable;
      mydata.atm1.radius=1.0*mydata.scfac;
      mydata.atm1.Axis=vector(0,0,1.5*mydata.scfac);
      mydata.atm1.IsClosed=false;
      mydata.atm1.Resolution=60;
      mydata.atm1.position=point(0,0,-0.1*mydata.scfac);
      mydata.atm1.canbuffer=true;
   }
      mydata.atm1=mydata.vframe.add("Surface","Name":"AtmopshereGlow");
      mydata.vframe.createtexture("Atm1",DataDir+"\textures\gradient1.bmp",DataDir+"\textures\gradient1.bmp");
      mydata.atm1.renderback=true;mydata.atm1.renderfront=true;
      mydata.atm1.enablelight=false;
      mydata.atm1.Texture="Atm1";
      mydata.atm1.BlendType=BlendTransparent;mydata.atm1.DepthMask=DepthMaskDisable;
      rdd=1.0*mydata.scfac;
      fnc=functor("radial2point("+str(rdd)+",u,v)","u","v");
      mydata.atm1.Generate(fnc,0,2*pi,60,-0.1*pi/2,pi/2,30);
      mydata.atm1.canbuffer=true;
   
   #full atmosphere (equal light)
   mydata.atm2=mydata.vframe.add("Sphere","Name":"AtmosphereEqual");
   mydata.atm2.renderback=true;mydata.atm2.renderfront=false;
   mydata.atm2.enablelight=false;
   mydata.atm2.BlendType=BlendTransparent;mydata.atm2.DepthMask=DepthMaskDisable;
   mydata.atm2.radius=mydata.scfac;
   mydata.atm2.resolution=60;
   mydata.atm2.canbuffer=true;
   
   gridthick=mydata.starsizefrac*mydata.scfac;
   #azimuth grid
   gridrd=mydata.scfac;
   if true then {
      gridframe=mydata.vframe.AddSubFrame("Grid");
      for i=0 to 15 do {
         ang=i/16*2*Pi;
         cl=color(0,0.6,0.5,0.15);
         gridframe.add("Curve","Color":cl,"BlendType":BlendTransparent,
            "Size":gridthick).makecircle(point(0,0,0),vector(cos(ang),sin(ang),0),gridrd,200);
      }
      for i=0 to 6 do {
         ang=i/6*Pi/2;
         cl=color(0,0.6,0.5,0.15);
#         if i==0 then cl=color(1,0,0);
         gridframe.add("Curve","Color":cl,"BlendType":BlendTransparent,
            "Size":gridthick).makecircle(point(0,0,gridrd*sin(ang)),vector(0,0,1),gridrd*cos(ang),200);
      }
   }
   
   signcol=color(0.4,0.4,0.4);
   
   direcframe=mydata.vframe.AddSubFrame("Directions");
   direcframe.add("TextFlat","name":"n","Position":point(-0.9*gridrd,0.05*gridrd,0),"Content":"N",
      "Size":(0.15*gridrd),"EnableLight":false,"Color":signcol);
   direcframe.add("TextFlat","name":"z","Position":point(0.99*gridrd,0.05*gridrd,0),"Content":"Z",
      "Size":(0.15*gridrd),"EnableLight":false,"Color":signcol);
   direcframe.add("TextFlat","name":"o","Position":point(0.05*gridrd,0.99*gridrd,0),"Content":"O",
      "Size":(0.15*gridrd),"EnableLight":false,"Color":signcol);
   direcframe.add("TextFlat","name":"w","Position":point(0.05*gridrd,-0.87*gridrd,0),"Content":"W",
      "Size":(0.15*gridrd),"EnableLight":false,"Color":signcol);
   
   myviewport=T_getviewport;
   myviewport.CameraPos=point(0,0,mydata.camh*mydata.scfac);
   myviewport.Cameradir=vecnorm(vector(1,0,0.5));
   myviewport.FocalDistance=0.06*mydata.scfac;
   myviewport.NearClipPlane=0.05*mydata.scfac;
   myviewportFarClipPlane=2*mydata.scfac;
   myviewport.Aperture=60/180*Pi;

   Cam_Init(myviewport);
   ConstellHighlight_Init(root.SC.Universe.SolarSystem.Earth.StarGlobeFrame,root.SC.Universe.SolarSystem.Earth.StarGlobeFrame.StarglobeFront);

}


###########################################################################################################
###########################################################################################################

function Planetarium_CreateViewPort_Earth(mydata)
{
   try { DelObject(root.Viewports.Earth); }
   displayname=ReadSetting("DisplayName","");
   if displayname=="" then displayname="\\.\DISPLAY1";
   vp2=CreateNewViewPort(0.55,0.3,1,1);
   vp2.name="Earth";
   vp2.Framesize=0.003;
   vp2.start;
   vp2.setscene(root.SC);
   vp2.FocalDistance=80000000;
   vp2.cameradir=vecnorm(vector(0,-1,-0.5));
   vp2.camerapos=point(0,0,0)-vp2.FocalDistance*vp2.cameradir;
   vp2.cameraupdir=vector(0,0,1);
   vp2.enableusernavigation=root.Viewports.main.enableusernavigation;
   vp2.EnableUserTimeControl=root.Viewports.main.EnableUserTimeControl;
   vp2.NearClipPlane=0.1*vp2.FocalDistance;
   vp2.FarClipPlane=40*vp2.FocalDistance;
   vp2.Aperture=40/180*Pi;
   
   ssys_showstarback(true);
   root.SC.Universe.StarBackFrame.addignoreviewport("main");
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.visible=true;
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.addignoreviewport("main");

   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.addignoreviewport("Earth");
   root.SC.Universe.ClockFrame.addignoreviewport("Earth");
   root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.addignoreviewport("Earth");
   root.SC.Universe.SolarSystem.Earth.SunHalo.addignoreviewport("Earth");
   root.SC.Universe.SolarSystem.Earth.MoonHalo.addignoreviewport("Earth");
   root.SC.Universe.SolarSystem.Earth.Luna.addignoreviewport("Earth");
   root.SC.Universe.SolarSystem.Earth.StarGlobeFrame.addignoreviewport("Earth");

   mydata.viewport_earth=vp2;
}


function Planetarium_CreateViewPort_Sun(mydata)
{
   try { DelObject(root.Viewports.Sun); }
   au2km=149598000;

   displayname=ReadSetting("DisplayName","");
   if displayname=="" then displayname="\\.\DISPLAY1";
   vp3=CreateNewViewPort(0.55,0,1,0.3,);
   vp3.name="Sun";
   vp3.Framesize=0.003;
   vp3.start;
   vp3.setscene(root.SC);
   vp3.FocalDistance=7.0*au2km;
   vp3.cameradir=vecnorm(vector(0,-1,-0.5));
   vp3.camerapos=point(0,0,0)-vp3.FocalDistance*vp3.cameradir;
   vp3.cameraupdir=vector(0,0,1);
   vp3.enableusernavigation=root.Viewports.main.enableusernavigation;
   vp3.EnableUserTimeControl=root.Viewports.main.EnableUserTimeControl;
   vp3.NearClipPlane=0.1*vp3.FocalDistance;
   vp3.FarClipPlane=20*vp3.FocalDistance;
   vp3.Aperture=30/180*Pi;
   
   ssys_createsun;
   root.SC.Universe.SolarSystem.Sun.addignoreviewport("main");
   root.SC.Universe.SolarSystem.EarthOrbit.visible=true;
   root.SC.Universe.SolarSystem.addsubframe("EarthOrbitFrame");
   root.SC.Universe.SolarSystem.objectchangeparent(root.SC.Universe.SolarSystem.EarthOrbitFrame,"EarthOrbit");
   root.SC.Universe.SolarSystem.EarthOrbitFrame.addignoreviewport("main");
   root.SC.Universe.SolarSystem.EarthOrbitFrame.addignoreviewport("Earth");

   ssys_showstarback(true);
   root.SC.Universe.StarBackFrame.addignoreviewport("main");
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.visible=true;
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.addignoreviewport("main");
   
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.addignoreviewport("Sun");
   root.SC.Universe.ClockFrame.addignoreviewport("Sun");
   root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.addignoreviewport("Sun");
   root.SC.Universe.SolarSystem.Earth.SunHalo.addignoreviewport("Sun");
   root.SC.Universe.SolarSystem.Earth.MoonHalo.addignoreviewport("Sun");
   root.SC.Universe.SolarSystem.Earth.Luna.addignoreviewport("Sun");
   root.SC.Universe.SolarSystem.Earth.StarGlobeFrame.addignoreviewport("Sun");

   mydata.viewport_sun=vp3;
   Cam_Init(mydata.viewport_sun);
}

###########################################################################################################
###########################################################################################################

function CreatePlanetariumClock()
{
   #clock overlay
   clockframe=root.SC.Universe.addscreenframe("ClockFrame");
   #time
   clock=clockframe.add("Clock","name":"Clock1");
   clock.enablelight=false;
   clock.Position=point(0.085,0.085,0);
   clock.Size=0.075;
   clock.DepthTest=DepthTestDisable;
   clock.DepthMask=DepthMaskDisable;
   clock.BlendType=BlendTranslucent;
   clock.color=GetColor("Clock");
   #date
   clock=clockframe.add("Clock","name":"Clock2");
   clock.enablelight=false;
   clock.Position=point(0.15,0.01,0);
   clock.Type=ClockTypeCalendar;
   clock.Size=0.03;
   clock.DepthTest=DepthTestDisable;
   clock.DepthMask=DepthMaskDisable;
   clock.BlendType=BlendTranslucent;
   clock.color=GetColor("Clock");
}



###########################################################################################################
###########################################################################################################
###########################################################################################################


function CreateBackDropGent(mydata)
{
   bdframe=mydata.vframe.get("BackDropFrame");
   bdframe.transf.rotate(vector(0,0,1),-42/180*Pi-Pi+Pi/2);
   mydata.vframe.createtexture("sf1",DataDir+"\textures\gent_big.jpg");
   surf1=bdframe.add("Surface","name":"surface","enablelight":false,"color":color(0.4,0.4,0.4));
   surf1.Texture="sf1";
   dm=1*mydata.scfac;
   rd=9.5*mydata.scfac;
   oo=0.05*mydata.scfac;
   fnc1=functor("point(y,x,sqrt("+str(rd*rd)+"-sqr(x)-sqr(y))-"+str(rd)+"+"+str(oo)+")","x","y");
   surf1.Generate(fnc1,-1*dm,dm,30,-1*dm,dm,30);
}

function CreateBackDrop(mydata)
{
   #black obscuring object with hole for the sky dome
   bdframe=root.SC.addsubframe("MaskFrame");
   surf1=bdframe.add("Surface","name":"surface","enablelight":false,"color":color(0,0,0));
   rd=1.003*mydata.scfac;
   fnc1=functor("radial2point(r,a,0)-vector(0,0,0.01*"+str(rd)+")","r","a");
   dm=rd;
   surf1.Generate(fnc1,rd,3*rd,2,0,2*pi,60);
   surf1.canbuffer=true;
   
   maskfile=ScriptFilePath+"\panomask_gent.jpg";
   bdframe.CreateTexture("gent",maskfile,maskfile);
   maskfile=ScriptFilePath+"\panomask_exotic.jpg";
   bdframe.CreateTexture("exotic",maskfile,maskfile);

   bdframemask=bdframe.addsubframe("MaskHorizonFrame");
   bdframemask.transf.rotate(vector(0,0,1),-0.2*Pi);
#   bdframemask.Color=color(1,1,1,0.7);
   angmin=-0.03;
   angmax=0.375*pi;
   gr=0.0;
   surf1=bdframemask.add("Surface","name":"surface","enablelight":false,"color":color(gr,gr,gr));
   rd=0.995*mydata.scfac;
   fnc1=functor("radial2point("+str(rd)+",-1*a,b)","a","b");
   dm=rd;
   surf1.Generate(fnc1,0,2*pi,60,angmin,angmax,20);
   surf1.renderback=true;
   surf1.canbuffer=true;
   surf1.Texture="gent";
   surf1.BlendType=BlendTranslucent;
   
   if true then {
      zmin=rd*sin(angmin);
      zmax=rd*sin(angmax);
      stri="0.999*(pt.z-("+str(zmin)+"))/"+str(zmax-zmin);
      output(stri);
      fnc1=functor(stri,"pt");
      surf1.GenerateVertexProperty(fnc1,VertexPropertyTC2);
   }
}

function SetHorizonMask(name)
{
   root.SC.MaskFrame.MaskHorizonFrame.surface.Texture=name;
}


###########################################################################################################
###########################################################################################################
###########################################################################################################

function AdjustPlanetarium(mydata)
{
   #mechanism to co-move camera with earth
   createvar(tftot);createvar(tftotinv);
   if false then {
      root.SC.Universe.transf.reset;
      tftot=mydata.vframe.totaltransformation;
      tftotinv=tftot;tftotinv.invert;
      root.viewports.main.transf=tftotinv;
   } else {#(enhanced precision mechanism)
      root.sc.universe.transf.reset;
      tftot=mydata.vframe.totaltransformation;
      tftotinv=tftot;tftotinv.invert;
      root.sc.universe.transf=tftotinv;
      root.sc.light0pos=tftotinv*point(0,0,0);
   }
   

   viewtf=root.SC.Universe.SolarSystem.Earth.Inclin.transf
      *root.SC.Universe.SolarSystem.Earth.Inclin.Globe.transf
      *root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.transf;
 
    if mydata.IsPresent("viewport_earth") then
      mydata.viewport_earth.transf=viewtf;
   if mydata.IsPresent("viewport_sun") then
      mydata.viewport_sun.transf=root.SC.Universe.SolarSystem.Earth.transf*viewtf;
     
   if mydata.clipbelowhorizon then {
      root.SC.Universe.SolarSystem.Earth.resetclipplanes;
      root.SC.Universe.SolarSystem.Earth.addclipplane(
         CreatePlane1(viewtf*point(0,0,-0.01*mydata.scfac),viewtf*vector(0,0,1)),
         "main");
   }


   vertdir=viewtf*vector(0,0,1);

   earthpos=GetPlanetPosition("Earth");
   moonpos=GetLunaPosition;
   sunpos=point(0,0,0);
   
   #update positions of planet indicator
   foreach planname in mydata.planlist do {
      mydata.frame_planets.get(planname).transf.origin=point(0,0,0)+mydata.scfac*vecnorm(GetPlanetPosition(planname)-earthpos);
   }
   
   #update position of moon halo & calculate intensity
   ez=vecnorm(moonpos-point(0,0,0));
   ex=vecnorm(ez*vertdir);
   ey=ex*ez;
   mydata.frame_moonhalo.transf.origin=point(0,0,0)+1.0*mydata.scfac*ez;
   mydata.frame_moonhalo.transf.Xaxis=ex;
   mydata.frame_moonhalo.transf.Yaxis=ey;
   mydata.frame_moonhalo.transf.Zaxis=ez;
   
   fr=vecnorm(@vector(moonpos))^vecnorm(@vector(earthpos));
   fr=1+fr;if (fr>1) then fr=1;
   fr=fr*fr;
   mydata.moonhalo.color=color(0.7,0.7,0.7,0.2*fr);
   
   
   
   #update position of sun halo
   ez=vecnorm(sunpos-earthpos);
   ex=vecnorm(ez*vertdir);
   ey=ex*ez;
   mydata.frame_sunhalo.transf.origin=point(0,0,0)+1.0*mydata.scfac*ez;
   mydata.frame_sunhalo.transf.Xaxis=ex;
   mydata.frame_sunhalo.transf.Yaxis=ey;
   mydata.frame_sunhalo.transf.Zaxis=ez;
   
   #calculate sun presence
   sunang=vecnorm(sunpos-earthpos)^vertdir;
   sunfr=(sunang+0.01)/0.2;
   if sunfr<0 then sunfr=0;
   if sunfr>1 then sunfr=1;
   
   if not(mydata.animateatmosphere) then
      sunfr=0;
   
   #adjust atmosphere color to sun presence
   mydata.atm1.color=color(0.3,0.5,0.7,0.2+0.58*sunfr);
   mydata.atm2.color=color(0.3,0.5,0.7,0.1+0.78*sunfr);
   
   fcl1=color(22/255,37/255,53/255);
   fcl2=color(126/255,211/255,255/255);
   mydata.fog1.color=(1-sunfr)*fcl1+sunfr*fcl2;
   
   #adjust sunset/sunrise halo brightness
   fr=6*(sunang+0.1);fr=1-fr*fr;
   if fr<0 then fr=0;
   mydata.sunsethalo.color=color(1,0.5,0.0,0.6*fr);

   #adjust sunhalo brightness
   fr=3*(sunang+0.1);
   if fr>1 then fr=1;
   if fr<0 then fr=0;
   mydata.sunhalo.color=color(1,1,0.7,0.3*fr);
   
   #backdrop illumunation
   bdframe=mydata.vframe.get("BackDropFrame");
   bdlist=bdframe.getmembers;
   fr2=0.15+0.8*sunfr;
   foreach backdropnm in bdlist do {
      backdrop=bdframe.get(backdropnm.name);
      backdrop.color=color(fr2,fr2,fr2,1);
   }
   
   #move luna to globe
   root.SC.Universe.SolarSystem.Earth.Luna.transf.origin=point(0,0,0)+1.03*mydata.scfac*vecnorm(moonpos-point(0,0,0));
}



function AnimateStep()
{
   incrtime;
   AdjustPlanetarium(ref(mydata));
   ConstellHighlight_Update;
   Cam_Adjust(myviewport);
   if mydata.IsPresent("viewport_sun") then
      Cam_Adjust(mydata.viewport_sun);

   
   if mydata.comoving then {
      mydata.comovefrac=(mydata.comovefrac+0.0003)*1.05;
      if mydata.comovefrac>1 then mydata.comovefrac=1;
   } else {
      mydata.comovefrac=mydata.comovefrac/1.05-0.0003;
      if mydata.comovefrac<0 then mydata.comovefrac=0;
   }
   if mydata.comovefrac>0 then {
      earthpos=GetPlanetPosition("earth");
      earthdirflat=@vector(earthpos);earthdirflat.z=0;earthdirflat=vecnorm(earthdirflat);
      camdist=sqrt(sqr(mydata.viewport_sun.camerapos.x)+sqr(mydata.viewport_sun.camerapos.y));
      campos=point(
         camdist*earthdirflat.x,
         camdist*earthdirflat.y,
         mydata.viewport_sun.camerapos.z
         );
      mydata.viewport_sun.camerapos=(1-mydata.comovefrac)*mydata.viewport_sun.camerapos+mydata.comovefrac*campos;
      mydata.viewport_sun.cameradir=vecnorm(point(0,0,0)-mydata.viewport_sun.camerapos);
   }
   if isfunctiondefined("CustomAnimateStep") then CustomAnimateStep;
   
   render;
   if UIGetAxisActive(UIAxisX,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisX,UIAxisLevel0);
      myviewport.CameraDir=vecrotate(myviewport.CameraDir,vector(0,0,1),0.03*vl);
   }
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then {
      ok=false;
      ang0=vecangle(myviewport.CameraDir,vector(0,0,1));
      vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
      if vl>2 then vl=2;
      if vl<-2 then vl=-2;
      if (vl>0) and (ang0>5/180*Pi) then ok=true;
      if (vl<0) and (ang0<75/180*Pi) then ok=true;
      ok=true;
      if ok then {
         dr1=vecnorm(myviewport.CameraDir*vector(0,0,1));
         myviewport.CameraDir=vecrotate(myviewport.CameraDir,dr1,0.03*vl);
      }
   }
   if UIGetAxisActive(UIAxisZ,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisZ,UIAxisLevel0);
      myviewport.Aperture=myviewport.Aperture*(1-vl/20);
      if myviewport.Aperture<30/180*Pi then myviewport.Aperture=30/180*Pi;
      if myviewport.Aperture>170/180*Pi then myviewport.Aperture=170/180*Pi;
   }
}
