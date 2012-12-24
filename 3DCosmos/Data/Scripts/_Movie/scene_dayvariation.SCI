
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_Colors.sci");
codeblock readtextfile(ScriptFilePath+"\_CamMoveTools.sci");
codeblock readtextfile(ScriptFilePath+"\_ConstellHighlight.sci");



###############################################################################################
###############################################################################################
###############################################################################################

createvar(globeradius);




function AddPlane(mydata,fra,name,col)
{
   weakcol=color(col.r,col.g,col.b,0.1);
   sz=1.3*mydata.globeradius;
   fra.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0),
           "color":weakcol,"BlendType":BlendTransParent,"EnableLight":false,"DepthMask":DepthMaskDisable,"renderback":true);
   crv=fra.add("Curve","color":col,"EnableLight":false);
   crv.addpoint(point(-1*sz,-1*sz,0));
   crv.addpoint(point(-1*sz,1*sz,0));
   crv.addpoint(point(1*sz,1*sz,0));
   crv.addpoint(point(1*sz,-1*sz,0));
   crv.IsClosed=true;
   fra.add("Curve",
      "color":col,"BlendType":BlendTransParent,"EnableLight":false
      ).makeline(point(-1*sz,0,0),point(1*sz,0,0));
   fra.add("Text3D","Content":name,
      "Position":point(0.95*sz,0.95*sz,0),
      "UnitX":vector(-0.15*sz,0,0),
      "UnitY":vector(0,-0.15*sz,0),
      "EnableLight":false,"Color":col,"BlendType":BlendTransParent
      );
}


function setviewerposition(ilongit,ilattit)
{
   vframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame;
   vframe.transf.reset;
   vframe.transf.rotate(vector(0,0,1),ilongit);
   vframe.transf.rotate(vector(0,1,0),Pi/2-ilattit);
}


function InitStarGlobe(mydata)
{
   ssysframe=ssys_create;
   ssys_createearth(200);
   ssys_showstarback(true);
   #root.SC.Universe.SolarSystem.EarthOrbit.visible=false;
   
   #create clock
   CreateClock(true);   
   
   globeframe=GetPlanetFrame("Earth");
   earthframe=GetPlanetBodyFrame("Earth");
   
   globeradius=mydata.globeradius;
   
   #shadow
   shad=globeframe.add("sphere","Name":"shadow");
   shad.radius=0.99*globeradius;
   shad.EnableLight=false;
   shad.BlendType=BlendTranslucent;
   shad.DepthMask=DepthMaskDisable;
   shad.renderfront=false;
   shad.renderback=true;
   shad.color=color(0,0,0,0.5);
   shad.resolution=50;
   shad.canbuffer=true;
   
   
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
   halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/"+str(globeradius)+")),0.15)","p"),VertexPropertyColor);
   halo.canbuffer=true;
   
   sp=globeframe.add("sphere");
   sp.color=color(1,1,1);
   
   #Star globe
   tx=globeframe.createtexture("star",DataDir+"\textures\star4.bmp");
   st1=globeframe.add("StarGlobe","Name":"StarglobeFront");
   st1.radius=globeradius;
   st1.texture="star";
   st1.StarSize=0.007*globeradius;
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
   
   #Sun path
   fr=globeframe.addsubframe("EclipticaCircle");
   c1=fr.add("curve");
   c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
   c1.color=color(1,0.75,0);
   c1.size=2;

   
   #create earth pole axis & equator
   fr=earthframe.addsubframe("EquatorFrame");
   eaxis=fr.add("Curve","Name":"Axis");
   eaxis.makeline(point(0,0,-1*globeradius),point(0,0,+1*globeradius));
   eaxis.size=2;
   eaxis.color=GetColor("Red");
   eaxis.Arrow1Size=0.05*globeradius;
   eaxis.Arrow2Size=eaxis.Arrow1Size;
   c1=fr.add("curve");
   c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
   c1.color=GetColor("Red");
   c1.size=2;
   
   #create equatorial grid
   eqgridcol=getcolor("Red")+color(0,0,0);
   eqgridframe=globeframe.inclin.addsubframe("eqgrid");
   for i=-8 to 8 do {
      a=deg2rad(10*i);
      crv=eqgridframe.add("Curve","Size":1,"Color":eqgridcol);
      crv.makecircle(point(0,0,globeradius*sin(a)),vector(0,0,1),globeradius*cos(a),80);
   }
   for i=0 to 17 do {
      a=deg2rad(10*i);
      crv=eqgridframe.add("Curve","Size":1,"Color":eqgridcol);
      crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),globeradius,80);
      if (i==0) or (i==9) then { crv.Size=3; }
   }
   eqgridframe.visible=false;
   
   #create circumpolar area
   splframe=globeframe.inclin.addsubframe("CircumPolarFrame");splframe.blinkperiod=0.5;
   sph=splframe.add("sphere","Name":"CircumPolar","Radius":(0.995*globeradius),"Resolution":80,"RenderBack":true,
      "Color":color(0,0.5,0.8,0.15),"BlendType":BlendTranslucent,"EnableLight":false);
   sph.Angle2Max=(51.0)/180*Pi;
   sph.visible=false;
   
   #create invisible area
   splframe=globeframe.inclin.addsubframe("InvisibleFrame");splframe.blinkperiod=0.5;
   sph=splframe.add("sphere","Name":"Invisible","Radius":(0.995*globeradius),"Resolution":80,"RenderBack":true,
      "Color":color(0.6,0.5,0.0,0.15),"BlendType":BlendTranslucent,"EnableLight":false);
   sph.Angle2Min=Pi-(51.0)/180*Pi;
   sph.visible=false;
   
   
   #create viewer frame, but at earth center
   vframe=earthframe.addsubframe("ViewerFrame");
   
   setviewerposition(mydata.ViewerLongitude,mydata.ViewerLattitude);
   
   
   #create zenith axis & horizon
   horframe=vframe.addsubframe("horizon");
   horframe.visible=false;
   zaxis=horframe.add("Curve","Name":"ZenithAxis");
   zaxis.makeline(point(0,0,0),point(0,0,globeradius));
   zaxis.size=2;
   zaxis.color=color(0.2,0.5,0.2);
   zaxis.Arrow2Size=0.05*globeradius;
   horline=horframe.add("Curve","Name":"HorizLine");
   horline.makecircle(point(0,0,0),vector(0,0,1),1.01*globeradius,80);
   horline.size=2;
   horline.color=color(0.2,0.5,0.2);
   horplane=horframe.add("Cylinder","Name":"HorPlane");
   horplane.Axis=vector(0,0,0.01*globeradius);
   horplane.radius=globeradius;
   horplane.resolution=60;
   horplane.EnableLight=false;
   horplane.color=color(0.1,0.3,0.1,0.5);
   horplane.BlendType=BlendTranslucent;
   horplane.DepthMask=DepthMaskDisable;
   
   figobj=CreateFigure(horframe,0.15*globeradius);
   figobj.position.z=0.27*globeradius;
   figobj.position.x=-0.03*globeradius;
   
   horlabelframe=horframe.addsubframe("HorizonLabels");
   lne=horlabelframe.add("Curve","Name":"horline1","size":1,"color":color(0.2,0.5,0.2));
   lne.makeline(point(-1*globeradius,0,0),point(globeradius,0,0));
   lne.Arrow1Size=0.03*globeradius;
   lne.Arrow2Size=0.03*globeradius;
   lne=horlabelframe.add("Curve","Name":"horline1","size":1,"color":color(0.2,0.5,0.2));
   lne.makeline(point(0,-1*globeradius,0),point(0,globeradius,0));
   lne.Arrow1Size=0.03*globeradius;
   lne.Arrow2Size=0.03*globeradius;
   horlabelframe.visible=false;
   
   txtcol=color(0.2,0.6,0.2);txtsize=0.15*globeradius;
   fc=1.1;
   fr=horlabelframe.addviewdirframe(point(fc*globeradius,0,0));
   fr.add("Text3D","Content":"Z","color":txtcol,"enablelight":false,"position":point(-0.25*txtsize,-0.25*txtsize,0),"UnitX":vector(txtsize,0,0),"UnitY":vector(0,txtsize,0));
   fr=horlabelframe.addviewdirframe(point(-1*fc*globeradius,0,0));
   fr.add("Text3D","Content":"N","color":txtcol,"enablelight":false,"position":point(-0.25*txtsize,-0.25*txtsize,0),"UnitX":vector(txtsize,0,0),"UnitY":vector(0,txtsize,0));
   fr=horlabelframe.addviewdirframe(point(0,fc*globeradius,0));
   fr.add("Text3D","Content":"O","color":txtcol,"enablelight":false,"position":point(-0.25*txtsize,-0.25*txtsize,0),"UnitX":vector(txtsize,0,0),"UnitY":vector(0,txtsize,0));
   fr=horlabelframe.addviewdirframe(point(0,-1*fc*globeradius,0));
   fr.add("Text3D","Content":"W","color":txtcol,"enablelight":false,"position":point(-0.25*txtsize,-0.25*txtsize,0),"UnitX":vector(txtsize,0,0),"UnitY":vector(0,txtsize,0));
   
   horcap=horframe.add("Surface","Name":"HorCap");
   horcap.generate(functor("point(1.02*"+str(globeradius)+"*cos(u)*cos(v),1.02*"+str(globeradius)+"*cos(u)*sin(v),-1.02*"+str(globeradius)+"*sin(u))","u","v"),Pi/2,0,30,0,2*Pi,60);
   horcap.enablelight=false;
   horcap.color=color(0,0,0,0.8);
   horcap.BlendType=BlendTranslucent;
   horcap.DepthMask=DepthMaskDisable;
   
   #create azimutal grid
   azgridframe=vframe.addsubframe("azgrid");
   for i=-8 to 8 do {
      a=deg2rad(10*i);
      crv=azgridframe.add("Curve","Size":1,"Color":color(0.2,0.4,0.2));
      crv.makecircle(point(0,0,globeradius*sin(a)),vector(0,0,1),globeradius*cos(a),80);
   }
   for i=0 to 17 do {
      a=deg2rad(10*i);
      crv=azgridframe.add("Curve","Size":1,"Color":color(0.2,0.4,0.2));
      crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),globeradius,80);
      if (i==0) or (i==9) then { crv.Size=2; crv.color=color(0.4,0.8,0.4); }
   }
   azgridframe.visible=false;
   
   
   
   
   #makes sure the translucent object have correct order or rendering
   globeframe.moveobject("Inclin",+10000);
   
   
   #create sun object
   sunpoint=globeframe.add("Sphere","Name":"Sun");
   sunpoint.radius=globeradius/60;
   sunpoint.EnableLight=false;
   sunpoint.color=color(1,0.8,0);

   #Ecliptica plane
   root.SC.Universe.SolarSystem.Earth.AddSubFrame("EclipticaPlane");
   AddPlane(ref(mydata),root.SC.Universe.SolarSystem.Earth.EclipticaPlane,"Ecliptica",color(1,1,0));
   root.SC.Universe.SolarSystem.Earth.EclipticaPlane.visible=false;
   
   #equatorial plane
   root.SC.Universe.SolarSystem.Earth.Inclin.AddSubFrame("EquatorPlane");
   AddPlane(ref(mydata),root.SC.Universe.SolarSystem.Earth.Inclin.EquatorPlane,"Equator",GetColor("Red"));
   root.SC.Universe.SolarSystem.Earth.Inclin.EquatorPlane.visible=false;
   
   #misc
   mydata.viewpostransitfr=1;
   mydata.curviewframe=0;#0=ecliptica pole up + standstill 1=north pole up + co-moving; 2=zenith up
   mydata.pastviewframe=-1;
   mydata.viewtransitfr=1;
}

###############################################################################################

ttf=transformation;
unittf=transformation;
tf2=transformation;
tf3=transformation;

function AdjustStarGlobe(mydata)
{
   if mydata.viewpostransitfr<0.9999 then {
      mydata.viewpostransitfr=min(mydata.viewpostransitfr+0.005,1);
      tfr=0.5+0.5*sin((mydata.viewpostransitfr-0.5)*Pi);
      lo1=mydata.Viewerlongitude;
      lo2=mydata.PastViewerLongitude;
      la1=mydata.Viewerlattitude;
      la2=mydata.PastViewerLattitude;
      setviewerposition(
         tfr*lo1+(1-tfr)*lo2,
         tfr*la1+(1-tfr)*la2);
   }


   mydata.viewtransitfr=min(mydata.viewtransitfr+0.005,1);
   tfr=0.5+0.5*sin((mydata.viewtransitfr-0.5)*Pi);

   lasttimestep=incrtime;

   globeradius=mydata.globeradius;
   earthframe=root.SC.Universe.SolarSystem.Earth;
   viewerframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame;
   sunpoint=root.SC.Universe.SolarSystem.Earth.Sun;

   #track sun position
   vc=vecnorm(point(0,0,0)-earthframe.totaltransformation.origin);
   sunpoint.position=@point(globeradius*1.0*vc);
   
   #mechanism to co-move camera with earth
   if true then {
      ttf=earthframe.transf;

      corotfr=0;
      if mydata.curviewframe==1 then { corotfr=tfr; }
      if mydata.pastviewframe==1 then { corotfr=1-tfr; }
      if corotfr>0.001 then {
         tf2=root.SC.Universe.SolarSystem.Earth.Inclin.transf
            *root.SC.Universe.SolarSystem.Earth.Inclin.Globe.transf;
         ttf=ttf*weight(unittf,tf2,corotfr);
      }
      
      horizonfr=0;
      if mydata.curviewframe==2 then { horizonfr=tfr; }
      if mydata.pastviewframe==2 then { horizonfr=1-tfr; }
      if horizonfr>0.001 then {
         tf2=root.SC.Universe.SolarSystem.Earth.Inclin.transf
            *root.SC.Universe.SolarSystem.Earth.Inclin.Globe.transf
            *root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.transf;
         ttf=ttf*weight(unittf,tf2,horizonfr);
      }
      ttf.invert;
      
      root.viewports.main.transf=ttf;

      #handle corotation
      if false then {
      rotatfr=0;
      if mydata.curviewframe==1 then { rotatfr=mydata.viewtransitfr; }
      if mydata.pastviewframe==1 then { rotatfr=1-mydata.viewtransitfr; }
      if rotatfr>0 then {#enable corotation
         vp=GetViewPort;
         inclin=deg2rad(-23.4457889);
         poledir=vector(0,-1*sin(inclin),cos(inclin));
        if not(root.Pauzed) then {
           rotang=lasttimestep*2*Pi*365/364/(24*60*60)*rotatfr;
           root.SC.Universe.SolarSystem.Earth.transf=root.SC.Universe.SolarSystem.Earth.transf.rotate(poledir,-1*rotang);
         }
      }
      
      
      
      
      }
   }
}

function StarGlobe_Corotate(mydata)
{
   mydata.pastviewframe=mydata.curviewframe;
   mydata.curviewframe=1;
   mydata.viewtransitfr=0;
}


function StarGlobe_NonCorotate(mydata)
{
   mydata.pastviewframe=mydata.curviewframe;
   mydata.curviewframe=0;
   mydata.viewtransitfr=0;
}

function StarGlobe_Horizon(mydata)
{
   mydata.pastviewframe=mydata.curviewframe;
   mydata.curviewframe=2;
   mydata.viewtransitfr=0;
}

function StarGlobe_ChangeViewerPosition(mydata,newlongit,newlattit)
{
   mydata.PastViewerLongitude=mydata.ViewerLongitude;
   mydata.PastViewerLattitude=mydata.ViewerLattitude;
   mydata.ViewerLongitude=newlongit;
   mydata.ViewerLattitude=newlattit;
   mydata.viewpostransitfr=0;
}

function GotoDate(mydata,targetdate)
{
   oldfactor=root.TimeSpeedFactor;root.TimeSpeedFactor=0;
   while targetdate-root.time>24*60*60 do {
      root.time.AddDays(1);
      AdjustStarGlobe(ref(mydata));
      render;
      render;
   }
   root.TimeSpeedFactor=oldfactor;
}


timespeed=2000;

function StopTime()
{
   for i=0 to 30 do {
      fr=i/30;
      root.timespeedfactor=(1-fr)*timespeed;
      AnimateStep;
   }
}

function StartTime()
{
   for i=0 to 30 do {
      fr=i/30;
      root.timespeedfactor=fr*timespeed;
      AnimateStep;
   }
}

###############################################################################################


function AnimateStep()
{
   Cam_Adjust(vp);
   ConstellHighlight_Update;
   vp.camerapos=point(0,0,0)-cameradistance*vp.cameradir;
   AdjustStarGlobe(ref(mydata));
   render;
}

codeblock readtextfile(ScriptDir+"\_Movie\_AnimateTools.sci");


###############################################################################################
###############################################################################################
###############################################################################################

mydata=map;
mydata.globeradius=autokm(0.03);
mydata.ViewerLongitude=0;
mydata.ViewerLattitude=deg2rad(51);

root.framerate=30;

InitStarGlobe(ref(mydata));

#initialise viewing parameters
vp=GetViewPort;
vp.cameradir=vecnorm(vector(-0.6, -0.6, -0.4));
cameradistance=3*mydata.globeradius;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;
vp.NearClipPlane=0.1*mydata.globeradius;
vp.FarClipPlane=20*mydata.globeradius;
vp.FocalDistance=4*mydata.globeradius;

root.SC.Universe.SolarSystem.Earth.EclipticaCircle.visible=false;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.EquatorFrame.visible=false;

#root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.azgrid.visible=true;

root.time=time(2010,1,1,0,0,0);
createvar(vc);
createvar(tm);
ang=0;
root.TimeSpeedFactor=timespeed;

Cam_Init(vp);
ConstellHighlight_Init(root.SC.Universe.SolarSystem.Earth,root.SC.Universe.SolarSystem.Earth.StarglobeFront);

#-----------------------------------------------------------------------------------------

durat_noglobe=5;
durat_globe_noconstell=3;
durat_globe_constell=7;
durat_globe_introequator=7;

introdurat=durat_noglobe+durat_globe_noconstell+durat_globe_constell+durat_globe_introequator+1;
root.time=time(2010,1,6,18,0,0)-introdurat*root.timespeedfactor;


#initially, do not show the star globe
root.SC.Universe.SolarSystem.Earth.shadow.visible=false;
root.SC.Universe.SolarSystem.Earth.haloframe.visible=false;
root.SC.Universe.SolarSystem.Earth.StarglobeFront.visible=false;
root.SC.Universe.SolarSystem.Earth.StarGlobeBack.visible=false;
root.SC.Universe.SolarSystem.Earth.MilkyWay.visible=false;
root.SC.Universe.SolarSystem.Earth.Sun.visible=false;
root.SC.Universe.SolarSystem.EarthOrbit.visible=false;

FadeViewportsIn;
animate(durat_noglobe);

#activate the star globe and hide the normal background
root.SC.Universe.SolarSystem.Earth.shadow.visible=true;
root.SC.Universe.SolarSystem.Earth.haloframe.visible=true;
root.SC.Universe.SolarSystem.Earth.StarglobeFront.visible=true;
root.SC.Universe.SolarSystem.Earth.StarGlobeBack.visible=true;
root.SC.Universe.SolarSystem.Earth.MilkyWay.visible=true;
root.SC.Universe.SolarSystem.Earth.Sun.visible=true;



for i=0 to 30 do {
   fr=i/30;
   root.SC.Universe.StarBackFrame.StarBack.Color=color(1-fr,1-fr,1-fr,1);
   AnimateStep;
}
ssys_showstarback(false);

animate(durat_globe_noconstell);

#show constellation lines
root.SC.Universe.SolarSystem.Earth.StarglobeFront.LineSize=root.SC.Universe.SolarSystem.Earth.StarglobeFront.StarSize;
root.SC.Universe.SolarSystem.Earth.StarGlobeBack.LineSize=root.SC.Universe.SolarSystem.Earth.StarGlobeBack.StarSize;
animate(durat_globe_constell);

#show equator circle
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.EquatorFrame.blinkperiod=0.4;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.EquatorFrame.MaxBlinkCount=3;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.EquatorFrame.visible=true;
animate(durat_globe_introequator);

#corotate with Earth
StopTime;animate(1);StarGlobe_Corotate(ref(mydata));animate(8);StartTime;
animate(7);

#show equatorial grid
root.SC.Universe.SolarSystem.Earth.Inclin.eqgrid.visible=true;
animate(9);
root.SC.Universe.SolarSystem.Earth.Inclin.eqgrid.visible=false;
animate(3);

#show horizon
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.visible=true;
animate(6);

#show wind directions
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.HorizonLabels.visible=true;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.HorizonLabels.blinkperiod=0.4;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.HorizonLabels.maxblinkcount=5;
animate(7);

#Show Zenith up & show horizon
StopTime;animate(1);StarGlobe_Horizon(ref(mydata));animate(8);StartTime;
animate(9);

#show azimuthal grid
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.azgrid.visible=true;
animate(3);
#Start moving to Focus on North pole
Cam_Rotate(vp,vector(0.791303552, -0.229394974, -0.566759767),18);
animate(13);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.azgrid.visible=false;

animate(4);
PointHighlight_Start("Pole",color(1,0.5,0,0.8),  40.5,89.3);
animate(10);
ConstellHighlight_Stop("Pole");
ConstellHighlight_Start("UMI",GetColor("Red",0.75));
animate(10);
ConstellHighlight_Stop("UMI");

PointHighlight_Start("Pole",color(1,0.5,0,0.8),  40.5,89.3);
ArcHighlight_Start("arc2",GetColor("Red",0.75),  40.5,89.3,  165.75,61.75,  false);
ArcHighlight_Start("arc1",color(0.75,0,0.75,0.75),  165.25,56.38333,  165.75,61.75,false);
animate(10);
ConstellHighlight_Stop("Pole");
ConstellHighlight_Stop("arc1");
ConstellHighlight_Stop("arc2");
animate(2);

#show circumpolar area
root.SC.Universe.SolarSystem.Earth.Inclin.CircumPolarFrame.CircumPolar.visible=true;
animate(9);
root.SC.Universe.SolarSystem.Earth.Inclin.CircumPolarFrame.CircumPolar.visible=false;
animate(2);

#show southern hemisphere
StopTime;animate(1);Cam_Rotate(vp,vector(-0.745931417, -0.329011255, 0.579083685),10);
animate(11);
StartTime;
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.HorCap.visible=false;
animate(5);
#show southern crux
ConstellHighlight_Start("CRU",GetColor("Red",0.85));
animate(12);
#show invisibility cap
root.SC.Universe.SolarSystem.Earth.Inclin.InvisibleFrame.Invisible.visible=true;
animate(9);
root.SC.Universe.SolarSystem.Earth.Inclin.InvisibleFrame.Invisible.visible=false;

animate(3);
ConstellHighlight_Stop("CRU");

#Switch viewer position to southern hemispere
StopTime;animate(1);
Cam_Rotate(vp,vector(-0.79515744, -0.428086645, -0.4294956),6);
animate(7);
root.SC.Universe.SolarSystem.Earth.Inclin.Globe.ViewerFrame.horizon.HorCap.visible=true;
StarGlobe_ChangeViewerPosition(ref(mydata),0,deg2rad(-50));
animate(7);
StartTime;
animate(12);

#Switch viewer position to equator hemispere
StopTime;animate(1);StarGlobe_ChangeViewerPosition(ref(mydata),0,deg2rad(0));animate(8);StartTime;
animate(12);

FadeViewportsOut;
stop;

