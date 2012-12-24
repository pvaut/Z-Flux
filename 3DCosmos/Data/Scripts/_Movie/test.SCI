
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_PlanetariumTools.sci");
codeblock readtextfile(ScriptFilePath+"\_Colors.sci");




#=================================================================================
# SETUP
#=================================================================================


mydata=map;
mydata.scfac=100;
mydata.starsizefrac=0.004;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
mydata.camh=0.1;

InitPlanetarium(ref(mydata));
CreatePlanetariumClock;
root.SC.Universe.ClockFrame.Clock1.size=0.1;
root.SC.Universe.ClockFrame.Clock1.position=point(0.11,0.11);
root.SC.Universe.ClockFrame.Clock2.visible=false;
CreateBackDropGent(ref(mydata));

root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Grid.visible=false;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.Directions.visible=false;

Planetarium_CreateViewPort_Earth(ref(mydata));
vpearth=mydata.viewport_earth;
vpearth.EraseBackground=true;
ratio=root.Viewports.main.aspectratio;
sz=0.4;offset=0.03;
vpearth.XMinFrac=1-offset-sz;
vpearth.YMaxFrac=1.0-offset*ratio;
vpearth.XMaxFrac=1-offset;
vpearth.YMinFrac=vpearth.YMaxFrac-sz*ratio;
vpearth.active=false;

dist=14000000;
dist=10000000;
vpearth.FocalDistance=2.0*dist;
vpearth.cameradir=vecnorm(vector(-1,0,-0.5));
vpearth.camerapos=point(0,0,0)-dist*vpearth.cameradir;
vpearth.Aperture=60/180*Pi;
vpearth.nearclipplane=0.4*dist;
vpearth.FrameSize=0.015;
vpearth.FrameColor=color(0.2,0.2,0.2);


root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.visible=false;
root.SC.Universe.SolarSystem.Earth.MoonHalo.visible=false;
root.SC.Universe.SolarSystem.Earth.StarGlobeFrame.StarglobeFront.linesize=0;

   #enhanced texture on Earth
   eglobe=GetPlanetBodyFrame("Earth");
   etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
   eglobe.GlobeRendering.Earth.Texture=etx2.name;


############################### Create Ptolemaeus viewport
   try { DelObject(root.Viewports.Ptol); }
   displayname=ReadSetting("DisplayName","");
   if displayname=="" then displayname="\\.\DISPLAY1";
   vp2=CreateNewViewPort(0.55,0.3,1,1);
   vp2.name="Ptol";
   vp2.Framesize=0.003;
   vp2.start;
   vp2.setscene(root.SC);
   vp2.FocalDistance=90000000;
   vp2.cameradir=vecnorm(vector(1,0,0));
   vp2.camerapos=point(0,0,0)-0.5*vp2.FocalDistance*vp2.cameradir;
   vp2.cameraupdir=vector(0,0,1);
   vp2.enableusernavigation=root.Viewports.main.enableusernavigation;
   vp2.EnableUserTimeControl=root.Viewports.main.EnableUserTimeControl;
   vp2.NearClipPlane=0.1*vp2.FocalDistance;
   vp2.FarClipPlane=40*vp2.FocalDistance;
   vp2.Aperture=40/180*Pi;
   
   vp2.XMinFrac=offset;
   vp2.YMaxFrac=1.0-offset*ratio;
   vp2.XMaxFrac=offset+sz;
   vp2.YMinFrac=vp2.YMaxFrac-sz*ratio;
   #vp2.EraseBackground=true;
   vp2.FrameSize=0.015;
   vp2.FrameColor=color(0.2,0.2,0.2);
   vp2.FrameSize=0;
   vp2.active=false;
   

   root.SC.Universe.StarBackFrame.addignoreviewport("Ptol");
   root.SC.Universe.StarBackFrame.addignoreviewport("Earth");
   
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.addignoreviewport("Ptol");
   root.SC.Universe.ClockFrame.addignoreviewport("Ptol");
   root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.addignoreviewport("Ptol");
   root.SC.Universe.SolarSystem.Earth.SunHalo.addignoreviewport("Ptol");
   root.SC.Universe.SolarSystem.Earth.MoonHalo.addignoreviewport("Ptol");
   root.SC.Universe.SolarSystem.Earth.Luna.addignoreviewport("Ptol");
   root.SC.Universe.SolarSystem.Earth.StarGlobeFrame.addignoreviewport("Ptol");
   
   globeframe=root.SC.Universe.SolarSystem.Earth.addsubframe("StarGlobe");
   globeframe.addignoreviewport("main");
   globeframe.addignoreviewport("Earth");
   
   globeradius=4*root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.radius;
   #create halo
   halfcircle=FlatContourSet;
   haloframe=globeframe.addviewdirframe(point(0,0,0),"haloframe");
   halfcircle.generate(functor("point("+str(globeradius)+"*sin(a),"+str(globeradius)+"*cos(a),0)","a"),Pi,0,40);
   halo=haloframe.add("SolidObject","Name":"Halo");
   halo.Revolve(halfcircle,40);
   halo.BlendType=BlendTransparent;
   halo.RenderBack=true;
   halo.DepthMask=DepthMaskDisable;
   halo.EnableLight=false;
   halo.GenerateVertexProperty(functor("color(0,0,1/(1+5*sqr(p.z/"+str(globeradius)+")),0.2)","p"),VertexPropertyColor);
   halo.canbuffer=true;
   
   #Star globe
   tx=globeframe.createtexture("star",DataDir+"\textures\star4.bmp");
   st1=globeframe.add("StarGlobe","Name":"StarglobeFront");
   st1.radius=globeradius;
   st1.texture="star";
   st1.StarSize=0.013*globeradius;
   st1.LineSize=0;#0.01*globeradius;
   st1.linecolor=color(0,0.5,1,0.4);
   st1.renderback=false;
   st2=globeframe.add("StarGlobe","Name":"StarGlobeBack");
   st2.radius=globeradius;
   st2.texture="star";
   st2.StarSize=0.007*globeradius;
   st2.LineSize=0;#0.01*globeradius;
   st2.renderfront=false;
   st2.linecolor=color(0,0.5,1,0.4);
   st2.color=color(1,1,1,0.15);
   
   #Create milky way
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
   mw.color=color(0.3,0.5,1,0.45);
   mw.texture=tx.name;
   mw.BlendType=BlendTransparent;mw.DepthMask=DepthMaskDisable;
   mw.renderback=false;mw.renderfront=true;
   mw.radius=globeradius;
   
   linew=25;
   crossframe=root.SC.Universe.addscreenframe("CrossFrame");
   crossframe.addignoreviewport("main");
   crossframe.addignoreviewport("earth");
   ln1=crossframe.add("Curve","Color":GetColor("Red"),"Size":linew);
   csz=0.12;coffs=0.03;
   ln1.makeline(point(coffs,1-coffs,0),point(coffs+csz,1-coffs-csz,0));
   ln1=crossframe.add("Curve","Color":GetColor("Red"),"Size":20);
   ln1.makeline(point(coffs,1-coffs-csz,0),point(coffs+csz,1-coffs,0));
   crossframe.visible=false;

   ext=linew/5000;
   checkframe=root.SC.Universe.addscreenframe("CheckFrame");
   checkframe.addignoreviewport("main");
   checkframe.addignoreviewport("Ptol");
   ln1=checkframe.add("Curve","Color":color(0,0.75,0),"Size":linew);
   csz=0.14;sz1=0.06;sz2=0.12;
   ln1.makeline(point(coffs,1-coffs-sz2+sz1,0),point(coffs+sz1+ext,1-coffs-sz2-ext,0));
   ln1=checkframe.add("Curve","Color":color(0,0.75,0),"Size":20);
   ln1.makeline(point(coffs+sz1,1-coffs-sz2,0),point(coffs+sz1+sz2,1-coffs,0));
   checkframe.visible=false;



############################### End Create Ptolemaeus

root.time=time(2012,1,1,17,40,0);
myviewport=T_getviewport;
myviewport.FocalDistance=0.06*mydata.scfac;
myviewport.Aperture=80/180*Pi;

myviewport.cameradir=vecnorm(vector(0.92526291, 0.08590846, 0.369470278-0.05));




#=================================================================================
# ANIMATION
#=================================================================================

root.TimeSpeedFactor=0;
FadeViewportsIn;

if false then {
   animate(4);
   Cam_RotateHor(myviewport,-0.6*Pi,14);
   animate(18);
   
   root.TimeSpeedFactor=700;
   animate(6);
   Cam_RotateHor(myviewport,-0.6*Pi,10);
   animate(12);
   PointHighlight_Start("Pole",color(1,0.5,0),  40.5,89.3);
   animate(6);
   ConstellHighlight_Stop("Pole");
   animate(4);
}

root.Viewports.Ptol.active=true;

#animate(10);

#root.Viewports.Ptol.EraseBackground=true;
root.Viewports.Ptol.FrameSize=0;
vpearth.active=true;

#animate(3);
root.SC.Universe.CrossFrame.visible=true;
root.Viewports.Earth.EraseBackGround=false;
root.Viewports.Earth.FrameSize=0;
root.SC.Universe.CrossFrame.blinkperiod=0.4;root.SC.Universe.CrossFrame.maxblinkcount=4;
root.SC.Universe.CheckFrame.visible=true;
root.SC.Universe.CheckFrame.blinkperiod=0.4;root.SC.Universe.CheckFrame.maxblinkcount=4;

animate(10);

FadeViewportsOut;
stop;



