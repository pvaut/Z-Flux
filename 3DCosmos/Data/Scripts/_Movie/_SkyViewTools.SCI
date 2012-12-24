codeblock readtextfile(ScriptFilePath+"\_AnimateTools.sci");
codeblock readtextfile(ScriptFilePath+"\_CamMoveTools.sci");
codeblock readtextfile(ScriptFilePath+"\_Colors.sci");




dist_unzoomed=autokm(3);
dist_zoomed=autokm(0.28);
skysize=1000000;
skyviewportdistance=3.8*skysize;


function InitSkyView(mydata)
{
   ei=+23.4457889/180*Pi;
   coseps=0.9174369381;
   sineps=0.3978812030;
   
   ssysframe=ssys_create;
   ssys_createmilkyway;
   ssys_createsun;
   ssys_createearth(1300);
   ssys_showstarback(true);
   root.SC.VolumeShadowAdd(0,color(0,0,0,0.25),0.01*skysize,16*skysize);
   CreateClock(true);
   root.framerate=30;

   #enhanced texture on Earth
   eglobe=GetPlanetBodyFrame("Earth");
   etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
   eglobe.GlobeRendering.Earth.Texture=etx2.name;

   
   mydata.earthpositframe=root.SC.Universe.SolarSystem.Earth;
      
   earthframe=GetPlanetBodyFrame("Earth");
   myviewport=T_getviewport;
   myviewport.XmaxFrac=1.0;
   #myviewport.EnableUserNavigation=false;
   mydata.viewport_sun=myviewport;
   
   myviewport=T_getviewport;
   myviewport.FocalDistance=autokm(0.4);
   myviewport.NearClipPlane=0.05*myviewport.FocalDistance;
   myviewportFarClipPlane=4*myviewport.FocalDistance;
   myviewport.Cameradir=vecnorm(vector(1,0.1,-1));
   myviewport.CameraPos=point(0,0,0)-dist_unzoomed*myviewport.Cameradir;
   Cam_Init(myviewport);
   
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
   myviewport.FocalDistance=3*skysize;
   myviewport.cameradir=vecnorm(vecnorm(vector(-0.4,-1.5,0))-vector(0,0,0.4));
   myviewport.cameraupdir=vector(0,0,1);
   myviewport.enableusernavigation=root.Viewports.main.enableusernavigation;
   myviewport.EnableUserTimeControl=root.Viewports.main.EnableUserTimeControl;
   myviewport.NearClipPlane=0.1*myviewport.FocalDistance;
   myviewport.FarClipPlane=20*myviewport.FocalDistance;
   Cam_Init(myviewport);
#   myviewport.Aperture=30/180*Pi;

   #hide unwanted stuff for sky frame
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.addignoreviewport("Sky");
   root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.CastVolumeShadow=false;
   orbitframe=root.SC.Universe.SolarSystem.AddSubFrame("OrbitFrame");
   root.SC.Universe.SolarSystem.objectchangeparent(orbitframe,"EarthOrbit");
   root.SC.Universe.SolarSystem.orbitframe.addignoreviewport("Sky");
   root.SC.Universe.ClockFrame.addignoreviewport("Sky");
  
   mydata.earthradius=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.Radius;
   
   #create earth globe overlay
   mydata.earthoverlayframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.AddSubFrame("OverlayFrame");
   mydata.earthoverlayframe.CastVolumeShadow=false;
   mydata.earthoverlayframe.addignoreviewport("Sky");
#   edir=mydata.earthoverlayframe.add("Curve","Size":600000,"Color":color(0,0.5,1),"CurveRenderType":CurveRenderSmooth);
#   edir.makeline(point(0,0,-1.5*mydata.earthradius),point(0,0,1.5*mydata.earthradius));
   edir=mydata.earthoverlayframe.add("Cylinder",
      "Position":point(0,0,-1.7*mydata.earthradius),"Axis":vector(0,0,3.4*mydata.earthradius),
      "Radius":600000,"Color":color(0.6,0.6,0.6));
   redcol1=GetColor("Red");redcol1.a=0.5;
   
   ecirc=mydata.earthoverlayframe.add("Curve","Size":2,"Color":redcol1,"BlendType":BlendTranslucent,"Name":"equator");
   ecirc.makecircle(point(0,0,0),vector(0,0,1),1.005*mydata.earthradius,100);
   
   csf=mydata.earthoverlayframe.AddSubFrame("TropicCancer");csf.visible=false;
   ecirc=csf.add("Curve","Size":2,"Color":color(1,0.5,0,0.5),"BlendType":BlendTranslucent);
   ecirc.makecircle(point(0,0,mydata.earthradius*sineps),vector(0,0,1),1.005*mydata.earthradius*coseps,100);
   
   csf=mydata.earthoverlayframe.AddSubFrame("TropicCapriCorn");csf.visible=false;
   ecirc=csf.add("Curve","Size":2,"Color":color(1,0.5,0,0.5),"BlendType":BlendTranslucent);
   ecirc.makecircle(point(0,0,-1*mydata.earthradius*sineps),vector(0,0,1),1.005*mydata.earthradius*coseps,100);
   
   csf=mydata.earthoverlayframe.AddSubFrame("ArcticCircleN");csf.visible=false;
   ecirc=csf.add("Curve","Size":2,"Color":color(0.5,1.0,0,0.3),"BlendType":BlendTranslucent);
   ecirc.makecircle(point(0,0,mydata.earthradius*sin(Pi/2-ei)),vector(0,0,1),1.005*mydata.earthradius*cos(Pi/2-ei),100);
   
   csf=mydata.earthoverlayframe.AddSubFrame("ArcticCircleS");csf.visible=false;
   ecirc=csf.add("Curve","Size":2,"Color":color(0.5,1.0,0,0.3),"BlendType":BlendTranslucent);
   ecirc.makecircle(point(0,0,-1*mydata.earthradius*sin(Pi/2-ei)),vector(0,0,1),1.005*mydata.earthradius*cos(Pi/2-ei),100);

   mydata.earthoverlayframe.visible=false;
 
   #create shadow plane & sun direction
   mydata.shadowframe=root.SC.Universe.SolarSystem.Earth.AddSubFrame("ShadowFrame");
   mydata.shadowframe.CastVolumeShadow=false;
   mydata.shadowframe.addignoreviewport("Sky");
   fc=1.3;
   mydata.shadowframe.add("Rectangle","position":point(0-fc*mydata.earthradius,0-fc*mydata.earthradius,0),
      "Axis1":vector(2*fc*mydata.earthradius,0,0),"Axis2":vector(0,2*fc*mydata.earthradius,0),
      "RenderBack":true,
      "EnableLight":false,"BlendType":BlendTranslucent,"Color":color(0.3,0.3,0.3,0.3));
   sundir=mydata.shadowframe.add("Curve","Size":3,"Color":color(1,0.5,0),"Arrow2Size":1000000);
   sundir.makeline(point(0,0,1.5*mydata.earthradius),point(0,0,1.02*mydata.earthradius));
   mydata.shadowframe.visible=false;

   #create object that indicate viewer position
   mydata.refposframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.AddSubFrame("ResPosFrame");
#   mydata.refposobject=mydata.refposframe.Add("Sphere",
#      "Color":GetColor("Red"),"Radius":(mydata.earthradius/30));
#   mydata.refposobject.position=radial2point(mydata.earthradius,(mydata.longit+Pi),mydata.lattit);
   CreateFigure(mydata.refposframe,mydata.earthradius/5);
   mydata.refposframe.visible=false;
   #mydata.refposframe.CastVolumeShadow=true;

   #create viewer frame, but at earth center
   earthframe=GetPlanetBodyFrame("Earth");
   mydata.vframe=earthframe.addsubframe("ViewerFrame");
   mydata.vframe.addignoreviewport("main");
   mydata.vframe.transf.rotate(vector(0,1,0),Pi/2-mydata.lattit);
   mydata.vframe.transf.rotate(vecrotate(vector(0,0,1),vector(0,1,0),-1*(Pi/2-mydata.lattit)),mydata.longit);

   #Create Sky dome
   vframebk=mydata.vframe.AddSubFrame("ViewFrameBK");
   vframebk.CastVolumeShadow=false;
   vframebk.addclipplane(CreatePlane1(point(0,0,-0.1*skysize),vector(0,0,1)));
   vframebk.Add("Cylinder","radius":(1.91*skysize),"Axis":vector(0,0,-0.2*skysize),"Resolution":60,"Color":color(0.12,0.23,0.12));
   #Create wind directions
   signcolor=color(0.25,0.25,0.75);
   rd=1.075*skysize;
   scenescale=0.1*skysize;
   dirnames=list('Z','O','N','W');
   for i=0 to 3 do {
      frame=vframebk.addviewdirframe(radial2point(rd,i*Pi/2,0),"DF_"+dirnames(i));
      frame.add("Text3D","Position":point(-0.48*scenescale,0.6*scenescale,0),
         "UnitX":vector(1.4*scenescale,0,0),"UnitY":vector(0,1.4*scenescale),"EnableLight":false,"Color":signcolor,"Content":dirnames(i));
      frame.add("Arrow","Position":point(0,0.4*scenescale,0),
         "Axis":vector(0,-0.4*scenescale,0),"NormAxis":vector(0,0,1),"Width":(0.2*scenescale),"EnableLight":false,"Color":signcolor);
   }
   vframebk.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,0,0),radial2point(rd,Pi,0));
   vframebk.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,-0.5*Pi,0),radial2point(rd,0.5*Pi,0));
      
   vframebk.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(1,0,0),1.001*skysize,80);
   vframebk.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(0,1,0),1.001*skysize,80);
   
   if true then {#create halo
      st_globeradius=str(skysize);
      halfcircle=FlatContourSet;
      haloframe=vframebk.addviewdirframe(point(0,0,0),"haloframe");
      halfcircle.generate(functor("point("+st_globeradius+"*sin(a),"+st_globeradius+"*cos(a),0)","a"),Pi,0,40);
      halo=haloframe.add("SolidObject","Name":"Halo");
      halo.Revolve(halfcircle,40);
      halo.BlendType=BlendTransparent;
      halo.RenderBack=true;
      halo.DepthMask=DepthMaskDisable;
      halo.EnableLight=false;
      halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/"+st_globeradius+")),0.25)","p"),VertexPropertyColor);
      halo.canbuffer=true;
   }

 #create viewer figure
   CreateFigure(mydata.vframe,0.2*skysize);
   
   #create small sun
   mydata.smallsunframe=vframebk.AddSubFrame("SmallSunFrame");
   mydata.smallsunframe.add("sphere","Name":"SmallSun","radius":(0.02*skysize),"color":color(1,1,0.5),"EnableLight":false);
   mydata.crv_sundir=vframebk.add("Curve","Name":"crv_sundir","EnableLight":false,"Color":color(1,0.7,0,0.3),"Size":2,"BlendType":BlendTransparent);   
   
   root.Viewports.main.XMaxFrac=1.0;
   root.Viewports.Sky.XMinFrac=1.0;root.Viewports.Sky.Active=false;
   mydata.zoomfrac=0;
}

function SetViewPosition(longit,lattit)
{
   mydata.longit=longit;
   mydata.lattit=lattit;
   mydata.vframe.transf.reset;
   mydata.vframe.transf.rotate(vector(0,1,0),Pi/2-mydata.lattit);
   mydata.vframe.transf.rotate(vecrotate(vector(0,0,1),vector(0,1,0),-1*(Pi/2-mydata.lattit)),mydata.longit);
   #mydata.refposobject.position=radial2point(mydata.earthradius,(mydata.longit+Pi),mydata.lattit);
   mydata.refposframe.transf.origin=radial2point(mydata.earthradius,(mydata.longit+Pi),mydata.lattit);
   mydata.refposframe.transf.Zaxis=vecnorm(mydata.refposframe.transf.origin-point(0,0,0));
   mydata.refposframe.transf.Yaxis=vector(0,-1,0);
   mydata.refposframe.transf.Xaxis=mydata.refposframe.transf.Yaxis*mydata.refposframe.transf.Zaxis;
}

function CreateSunTrace(name,col)
{
   vframebk=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.ViewFrameBK;
   crv_suntrace=vframebk.add("Curve","Name":name,"EnableLight":false,"Color":col,
         "Size":3,"BlendType":BlendTransparent,"CurveRenderType":CurveRenderDot);
   crv_suntrace.Track(mydata.smallsunframe,0.01*skysize);
   return(crv_suntrace);
}

function FreezeSunTrace(name)
{
   vframebk=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.ViewFrameBK;
   crv_suntrace=vframebk.get(name);
   crv_suntrace.StopTrack;
}

function DelSunTrace(name)
{
   vframebk=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.ViewFrameBK;
   crv_suntrace=vframebk.drop(name);
}


function AdjustSkyView(mydata)
{

   #mechanism to co-move sky camera with earth
   createvar(tftot);createvar(tftotinv);
   tftot=mydata.vframe.totaltransformation;
   tftotinv=tftot;tftotinv.invert;
   root.viewports.Sky.transf=tftotinv;
   
   #mechanism to co-move solar system camera with Earth
   earthpos=mydata.earthpositframe.totaltransformation.origin;
   tf1=Transformation;
   tf1.translate(mydata.zoomfrac*@vector(earthpos));
   tf2=tf1;tf2.invert;
   myviewport=root.viewports.main;
   myviewport.transf=tf2;
   dst=dist_unzoomed+(dist_zoomed-dist_unzoomed)*mydata.zoomfrac;
   myviewport.FocalDistance=1.3*dst;
   myviewport.CameraPos=point(0,0,0)-dst*myviewport.Cameradir;

   #adjust sky view frame
   skyviewport=root.Viewports.Sky;
   skyviewport.CameraPos=point(0,0,(0.35*skysize))-skyviewportdistance*skyviewport.Cameradir;


  #adjust position of shadow frame
  za=vecnorm(point(0,0,0)-earthpos);
  ya=vector(0,0,1);
  mydata.shadowframe.transf.Zaxis=za;
  mydata.shadowframe.transf.Yaxis=ya;
  mydata.shadowframe.transf.Xaxis=vecnorm(ya*za);
  
   #calculate position of sun in the local reference system
      sunpoint=tftotinv*point(0,0,0);
      sundir=vecnorm(sunpoint-point(0,0,0));
      mydata.smallsunframe.transf.origin=point(0,0,0)+1.003*skysize*sundir;
      mydata.crv_sundir.makeline(point(0,0,0)+0.2*skysize*sundir,sunpoint);
      sunaz=-0.5*Pi-angle(sundir.x,sundir.y);
      if sunaz<-1*Pi then sunaz=sunaz+2*Pi;
      if sunaz>Pi then sunaz=sunaz-2*Pi;
      sunang=Pi/2-vecangle(sundir,vector(0,0,1));
      sunc=sunang;
      if sunc>1 then sunc=1;
      if sunc<0 then sunc=0;
      colfac=sqrt(2*sunc);if colfac>1 then colfac=1;
      root.viewports.Sky.Light0ColorFactor=colfac;
      root.SC.VolumeShadowReset;
      root.SC.VolumeShadowAdd(0,color(0,0,0,0.5*sunc),0.01*skysize,16*skysize);

}



function AnimateStep()
{
   incrtime;
   Cam_Adjust(mydata.viewport_sun);
   Cam_Adjust(root.Viewports.Sky);
   AdjustSkyView(ref(mydata));
   render;
}

function ZoomIn()
{
   chr=chrono;chr.Start;
   dur=3;
   while chr.elapsed<=dur do {
      ps=chr.elapsed/dur;
      ps=SmoothRamp1(ps,0.5);
      mydata.zoomfrac=ps;
      AnimateStep;
   }
}

function ZoomOut()
{
   chr=chrono;chr.Start;
   dur=3;
   while chr.elapsed<=dur do {
      ps=chr.elapsed/dur;
      ps=SmoothRamp1(ps,0.5);
      mydata.zoomfrac=1-ps;
      AnimateStep;
   }
}


function SkyFrameVisible(status)
{
   if status then {
      root.Viewports.Sky.Active=true;
      root.Viewports.main.XMaxFrac=0.5;
      root.Viewports.Sky.XMinFrac=0.5;
      root.Viewports.Sky.XMaxFrac=1.0;
      #mydata.refposframe.blinkperiod=0.3;
      mydata.refposframe.visible=true;
   } else {
      root.Viewports.Sky.Active=false;
      root.Viewports.main.XMaxFrac=1.0;
      root.Viewports.Sky.XMinFrac=1.0;
      root.Viewports.Sky.XMaxFrac=2.0;
      mydata.refposframe.visible=false;
   }
}


function SkyFrameAppear()
{
   chr=chrono;chr.Start;
   dur=2;
   root.Viewports.Sky.Active=true;
   while chr.elapsed<=dur do {
      ps=chr.elapsed/dur;
      ps=ps*ps;ps=1-(1-ps)*(1-ps);
      ps=1-ps*0.5;
      if ps<0.5 then ps=0.5;
      root.Viewports.main.XMaxFrac=ps;
      root.Viewports.Sky.XMinFrac=ps;
      root.Viewports.Sky.XMaxFrac=1.0001;
      AnimateStep;
   }
   #mydata.refposframe.blinkperiod=0.3;
   mydata.refposframe.visible=true;
}

#function SkyFrameVisible()
#{
#   root.Viewports.Sky.Active=true;
#   ps=0.5;
#   root.Viewports.main.XMaxFrac=ps;
#   root.Viewports.Sky.XMinFrac=ps;
#   root.Viewports.Sky.XMaxFrac=1.0001;
#   AnimateStep;
#   mydata.refposframe.visible=true;
#}


function SkyFrameDisAppear()
{
   mydata.refposframe.blinkperiod=0;
   mydata.refposframe.visible=false;
   chr=chrono;chr.Start;
   dur=2;
   while chr.elapsed<=dur do {
      ps=chr.elapsed/dur;
      ps=ps*ps;ps=1-(1-ps)*(1-ps);
      ps=0.5+ps*0.5;
      if ps>=1.0 then {
         ps=1.0;
         root.Viewports.Sky.Active=false;
      }
      root.Viewports.main.XMaxFrac=ps;
      root.Viewports.Sky.XMinFrac=ps;
      root.Viewports.Sky.XMaxFrac=1.0001;
      AnimateStep;
   }
   root.Viewports.Sky.Active=false;
}

function JumpToDate(newtime)
{
   chr=chrono;chr.Start;
   tm0=root.time;
   diff=newtime-root.time;
   dur=diff/2000000;
   while chr.elapsed<=dur do {
      ps=chr.elapsed/dur;
      #ps=sin(ps*Pi/2);ps=ps*ps;
      ps=SmoothRamp1(ps,1.0/dur);
      root.time=tm0+ps*diff;
      AnimateStep;
   }
}
