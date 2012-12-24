codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

ssysframe=ssys_create;
ssys_createearth(1);
ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

#prompt for Eclipse date
menu=T_createmenu;
menu.add("","2008-08-01 10:22");
menu.add("","2009-07-22 02:35");
menu.add("","2010-07-11 19:34");
menu.add("","2012-11-13 22:12");
menu.add("","2015-03-20 09:46");
while not(menu.WasModified) do render;
datestr=menu.SelectID;
root.SC.Universe.MenuFrame.Dispose;
tm_year=ToScalar(datestr.split("-"));
tm_month=ToScalar(datestr.split("-"));
tm_day=ToScalar(datestr.split(" "));
tm_hour=ToScalar(datestr.split(":"));
tm_min=ToScalar(datestr);
root.time=time(tm_year,tm_month,tm_day,tm_hour,tm_min,0);
root.time=root.time-10000;
root.TimeSpeedFactor=200;

#Earth radius with offset for overlays
radius=6378.0*1.001;


#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(1,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);

#initialise shadow
root.SC.VolumeShadowAdd(0,color(0,0,0,0.5),5000,400000);
root.SC.VolumeShadowAdd(-695000,color(0,0,0,0.6),5000,400000);

#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;

#create grid on Earth
gridframe=eglobe.addsubframe("grid");
#gridframe.visible=false;
for i=-8 to 8 do {
   a=deg2rad(10*i);
   crv=gridframe.add("Curve");
   crv.makecircle(point(0,0,radius*sin(a)),vector(0,0,1),radius*cos(a),200);
   crv.CurveRenderType=CurveRenderSmooth;
   crv.Size=0.01*radius;
   crv.color=color(1,0,0,0.5);
   if i==0 then { crv.Size=0.03*radius; crv.color=color(1,0,0,1); }
}
for i=0 to 17 do {
   a=deg2rad(10*i);
   crv=gridframe.add("Curve");
   crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),radius,200);
   crv.CurveRenderType=CurveRenderSmooth;
   crv.Size=0.01*radius;
   crv.color=color(1,0,0,0.5);
   if (i==0) or (i==9) then { crv.Size=0.03*radius; crv.color=color(1,0,0,1); }
}



#create backdrop frame
backdropframe=ssysframe.addsubframe("BackDropFrame");
br=backdropframe.add("Bar","SizeX":30000,"SizeY":20000,"SizeZ":1000,"BlendType":BlendTranslucent);
br.EnableLight=false;
br.Color=Color(0.6,0.6,0.6,0.5);
backdropframe.CastVolumeShadow=false;



#Create Eclipse path
earthframe=GetPlanetFrame("Earth");
globeframe=GetPlanetBodyFrame("Earth");
path=globeframe.add("Curve","Color":color(0,1,0),"Size":3);

#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=7000;
vp.FarClipPlane=600000;
vp.FocalDistance=40000;
vp.cameradir=vecnorm(earthframe.Transf.origin-point(0,0,0))-vector(0,0,0.4);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;

while true do {
   incrtime;
   
   #position backdrop frame
   earthpos=earthframe.Transf.origin;
   dirz=vecnorm(earthpos-point(0,0,0));
   dirx=vecnorm(vector(0,0,1)*dirz);
   diry=vector(0,0,1);
   backdropframe.transf.origin=earthpos-15000*dirx-10000*diry+30000*dirz;
   backdropframe.transf.xaxis=dirx;
   backdropframe.transf.yaxis=diry;
   backdropframe.transf.zaxis=dirz;
  
   #co-move camera with earth (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #track path
   ttf=earthframe.transf*earthframe.inclin.transf*globeframe.transf;ttf.invert;
   pline=CreateLine1(ttf*point(0,0,0),ttf*earthframe.transf*GetLunaPosition);
   cpoint=pline.ClosestPoint(point(0,0,0));
   dst=distance(cpoint,point(0,0,0));
   if dst<radius then {
      epoint=cpoint-sqrt(radius*radius-dst*dst)*pline.Direction;
      path.addpoint(epoint);
   }
   
   render;
}
