codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

sats=list;
projsats=list;
objctr=0;

cols=list;
cols.add(color(1,0,0));
cols.add(color(0,0.75,0));
cols.add(color(0,0.25,1));
cols.add(color(0.8,0.5,0));
cols.add(color(0,0.6,0.6));
cols.add(color(0.7,0,0.7));

function addsat(inclin)
{
   col=color(1,1,1);
   if sats.size<cols.size then col=cols(sats.size);
   sat=inclinframe.addsubframe("sframe");
   sats.add(sat);
   sat.custom.active=true;
   sat.custom.col=col;
   
   mt=MotionKepler.Create(sat,"Motion");
   mt.Eccentricity=0;
   mt.SemiMajorAxis=startdistance/(1+mt.Eccentricity);
   mt.period=mt.SemiMajorAxis^1.5/8684551.229;
   mt.Inclination=inclin;
   mt.AscendingNode=deg2rad(180);
   mt.ArgPerigee=deg2rad(60);
   mt.PeriTime=Root.time+(mt.period/2)*24*60*60;
   
   objctr=objctr+1;objname=str(objctr);
   pl=planeframe.add("Rectangle","name":objname,"Position":point(-1*ess,0,0),
            "Axis1":vector(2*ess,0,0),
            "Axis2":vector(0,ess*cos(inclin),-1*ess*sin(inclin)),
            "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"RenderBack":true,"EnableLight":false,
            "Color":color(col.r,col.g,col.b,0.22));
   pl.Center=point(0,0.1*ess*cos(inclin),-0.1*ess*sin(inclin));
   objctr=objctr+1;objname=str(objctr);
   pl=planeframe.add("Rectangle","name":objname,"Position":point(-1*ess,0,0),
            "Axis1":vector(2*ess,0,0),
            "Axis2":vector(0,-1*ess*cos(inclin),ess*sin(inclin)),
            "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"RenderBack":true,"EnableLight":false,
            "Color":color(col.r,col.g,col.b,0.22));
   pl.Center=point(0,-0.1*ess*cos(inclin),0.1*ess*sin(inclin));

   
   sat.motion=mt;
   sat.add("Sphere","Radius":400,"Color":col,"SpecularColor":color(0.51,0.51,0.51),"SpecularValue":30);
   
   orbit=inclinframe.add("Curve","Color":col);
   mt.CreateCurve(orbit,200);

   sat.custom.projsat=mapframe.add("Sphere","Radius":0.005,"Color":col,"EnableLight":false);
   sat.custom.lastprojptx=1000;

}

ssysframe=ssys_create;
ssys_createearth(1);
ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

root.time=time(2000,6,1,0,0,0);
root.TimeSpeedFactor=1200;


#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.55,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.42,0),"Size":0.03);

#create map
mapsize=0.3;
mapframe=root.SC.Universe.addscreenframe("MapFrame");
maptexture=mapframe.CreateTexture("MapTexture",datadir+"\textures\earthmap.bmp");
mp=mapframe.add("Rectangle","EnableLight":false,"Axis1":vector(2*mapsize,0,0),"Axis2":vector(0,mapsize,0));
mp.Texture=maptexture.name;


earthframe=GetPlanetFrame("Earth");
inclinframe=earthframe.inclin;
globeframe=GetPlanetBodyFrame("Earth");

#create planes object
ess=15000;
col=color(1,0,0);
planeframe=inclinframe.addsubframe("PlaneFrame");
planeframe.autosort=true;


#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=7000;
vp.FarClipPlane=1000000;
vp.FocalDistance=60000;
vp.cameradir=vector(-0.656717856, -0.77984201, -0.347225086);
vp.camerapos=point(38277.091873653, 32215.518621378, 15532.135915139);



startdistance=12000;
earthradius=6356;



######################################################################################
# SETTINGS DIALOG BOX
######################################################################################

cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);

rc1=cframe.add("FrameControl");
rc1.position=point(0,0.8);
rc1.SizeX=0.4;
rc1.SizeY=0.3;
rc1.color=color(0.2,0.2,0.2);

txt1=cframe.add("TextControl");
txt1.position=point(0.025,0.95);
txt1.Content='Inclination:';
txt1.size=0.04;


vlinclin=cframe.add("ScalarControl");
vlinclin.position=point(0.025,0.9);
vlinclin.size=0.04;
vlinclin.SizeX=0.13;vlinclin.RangeSizeX=0.2;
vlinclin.min=0;vlinclin.max=90;vlinclin.step=5;vlinclin.DecimalCount=1;
vlinclin.value=0;


btadd=cframe.add("ButtonControl");
btadd.content='Add';
btadd.position=point(0.025,0.83);
btadd.size=0.04;
btadd.sizex=0.2;


addsat(0);


while true do {
   if btadd.wasmodified then addsat(deg2rad(vlinclin.Value));

   incrtime;
   
   #co-move camera with earth (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   foreach sat in sats do {
      posit=sat.transf.origin;
      earthdirx=globeframe.transf.xaxis;
      earthdiry=globeframe.transf.yaxis;
      ppx=@vector(posit)^earthdirx;
      ppy=@vector(posit)^earthdiry;
      a1=angle(ppx,ppy)+Pi;
      if a1>2*Pi then a1=a1-2*Pi;
      a2=angle(sqrt(sqr(posit.x)+sqr(posit.y)),posit.z);
      if a2>Pi/2 then a2=a2-2*Pi;
      projposit=point(a1/Pi*mapsize,(1+a2/(Pi/2))/2*mapsize,0.01);
      sat.custom.projsat.Position=projposit;
      if projposit.x<sat.custom.lastprojptx-0.1 then 
         sat.custom.projtrack=mapframe.add("Curve","Color":sat.custom.col,"Size":3,"CurveRenderType":CurveRenderDot);
      sat.custom.projtrack.addpoint(projposit);
      sat.custom.lastprojptx=projposit.x;
   }
      
   render;
}
