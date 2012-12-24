codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");


JID=0;

ssysframe=ssys_create;
root.SC.AmbientLightColor=color(0.15,0.15,0.15,1);
root.SC.VolumeShadowAdd(0,color(0,0,0,0.5),5000,9700000);


root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
ssys_createmilkyway;
ssys_createsun;

planetnamelist=ssys_getplanetlist;
foreach planetname in planetnamelist do {
   planframe=ssys_createplanet(planetname,1500);
   if planetname!="Earth" then {
      planframe.Inclin.Globe.GlobeRendering.clearobjects;
      planframe.add("LightPoint","Size":autokm(0.03),"Color":Color(0.5,0.7,1,0.5));
      porbit=GetPlanetOrbit(planetname);
      porbit.Color=color(0,0.5,1,0.25);
      porbit.BlendType=BlendTranslucent;
   }
}

earthpositframe=GetPlanetFrame("Earth");

#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;
#create grid on Earth
radius=9500000*1.01;
eglobe.add("cylinder","Position":point(0,0,-1.3*radius),"Axis":vector(0,0,2.6*radius),"Radius":(0.05*radius),"IsClosed":true,
    "color":color(0.5,0.5,0.5));
gridframe=eglobe.addsubframe("grid");
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

#create indications
sz=9500000;
indicationsframe=earthpositframe.addsubframe("Indications");
indicationsframe.add("Rectangle","RenderBack":true,"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,
               "color":color(0.15,0.15,0.15,0.7),"enablelight":false,
               "Position":point(-1.2*sz,-1.2*sz,0),"Axis1":vector(2.4*sz,0,0),"Axis2":vector(0,2.4*sz,0));
indicationsframe.add("Curve","color":color(1,0.5,0),"enablelight":false,
      "Size":3,"Arrow2Size":(0.15*sz)).makeline(point(0,0,-2*sz),point(0,0,-1*sz));


#create map
mapsize=0.27;
mapposx=1*T_getviewport.aspectratio-2*mapsize-0.01;
mapposy=0.01;
mapframe=root.SC.Universe.addscreenframe("MapFrame");
maptexture=mapframe.CreateTexture("MapTexture",datadir+"\textures\earthmap.bmp");
mp=mapframe.add("Rectangle","EnableLight":false,
   "Position":point(mapposx,mapposy),
   "Axis1":vector(2*mapsize,0,0),"Axis2":vector(0,mapsize,0));
mp.Texture=maptexture.name;

#create grid on map
radius=9500000*1.01;
for i=-8 to 8 do {
   crv=mapframe.add("Curve","enablelight":false,"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   crv.makeline(point(mapposx,mapposy+(0.5+i/16)*mapsize,0.001),point(mapposx+2*mapsize,mapposy+(0.5+i/16)*mapsize,0.001));
   crv.color=color(0.75,0,0,0.25);
   if i==0 then { crv.Size=2; crv.color=color(1,0,0,0.5); }
}
for i=0 to 36 do {
   crv=mapframe.add("Curve","enablelight":false,"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   crv.makeline(point(mapposx+i/36*2*mapsize,mapposy,0.001),point(mapposx+i/36*2*mapsize,mapposy+mapsize,0.001));
   crv.color=color(0.75,0,0,0.25);
   if i/9==floor(i/9) then { crv.Size=2; crv.color=color(1,0,0,0.5); }
}


lightmapframe=root.SC.Universe.addscreenframe("LightMapFrame");
mapframe.visible=false;
lightmapframe.visible=false;


menu=T_createmenu;
menu.Add("",'Zoom on Earth',"zoom",true);
menu.Add("",'Jump to next point',"jump");
menu.Add("",'Show points',"showpoints",true);
menu.Add("",'Show map',"showmap",true);


myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.1);
myviewport.FarClipPlane=autokm(30);
myviewport.FocalDistance=autokm(3);
myviewport.enableusernavigation=true;


dist_unzoomed=autokm(3);
dist_zoomed=autokm(0.3);

myviewport.cameradir=vecnorm(vector(1,0.5,-0.5));
myviewport.camerapos=point(0,0,0)-dist_unzoomed*myviewport.cameradir;

#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);
   
#equinoxes & solstices frame
poledir=root.SC.Universe.SolarSystem.Earth.Inclin.transf.Zaxis;
dr1=vecnorm(vector(0,0,1)*poledir);
dr2=vecnorm(vector(0,0,1)*dr1);
dd0=0.8;dd=1.2;
txts=autokm(0.1);
ptsframe=ssysframe.addsubframe("PTS");
ptsframe.visible=false;

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr1),@point(autokm(dd)*dr1));
txt=ptsframe.add("Text3D","Content":'Spring equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr1);
txt.UnitY=-1*txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr1),@point(autokm(-1*dd)*dr1));
txt=ptsframe.add("Text3D","Content":'Fall equinox',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr1);
txt.UnitY=txts*dr1;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(dd0)*dr2),@point(autokm(dd)*dr2));
txt=ptsframe.add("Text3D","Content":'Summer solstice',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(dd*1.1)*dr2);
txt.UnitY=-1*txts*dr2;
txt.UnitX=txts*vector(0,0,1);

line=ptsframe.add("Curve","color":color(0,0.5,0),"Size":2);
line.makeline(@point(autokm(-1*dd0)*dr2),@point(autokm(-1*dd)*dr2));
txt=ptsframe.add("Text3D","Content":'Winter solstice',"EnableLight":false,"color":color(0,0.5,0));
txt.position=@point(autokm(-1*dd*1.1)*dr2);
txt.UnitY=txts*dr2;
txt.UnitX=txts*vector(0,0,1);

   
zoomfact=1;zoomfrac=0;
zoomingin=false;zoomingout=false;


createvar(zoom_pos0);createvar(zoom_pos1);

#some initialisations for map creation
maprs=50;
mlongit=Pi/2;
mlongitincr=2*Pi/maprs;
mappointlist=list;
mapposxlist=list;
for i=0 to maprs do {
   mappointlist.add(radial2point(1,mlongit,0));
   mapposxlist.add(mapposx+2*mapsize*(i/maprs));
   mlongit=mlongit+mlongitincr;
}

root.time=CurrentTimeUT;
#root.time=time(2001,9,1,0,0,0);
root.TimeSpeedFactor=200;
while true do {
   incrtime;
   
#position indicationsframe
   earthdir=vecnorm(earthpositframe.totaltransformation.origin-point(0,0,0));
   zax=earthdir;
   yax=vector(0,1,0);
   xax=vecnorm(yax*zax);
   yax=vecnorm(zax*xax);
   indicationsframe.transf.zaxis=zax;
   indicationsframe.transf.yaxis=yax;
   indicationsframe.transf.xaxis=xax;

#create light map
   if mapframe.visible then {
      ttf=eglobe.totaltransformation;
      poledir=ttf*vector(0,0,1);
      flip=poledir^earthdir>0;
      lightmapframe.clearobjects;
      cnt=FlatContourSet;cnt.newcontour;
      if flip then {
         cnt.addpoint(point(mapposx+2*mapsize,mapposy+mapsize,0.002));
         cnt.addpoint(point(mapposx,mapposy+mapsize,0.002));
      } else {
         cnt.addpoint(point(mapposx+2*mapsize,mapposy,0.002));
         cnt.addpoint(point(mapposx,mapposy,0.002));
      }
      ldir1=vector(0,0,0);i=0;
      mlongit=Pi/2;
      mlongitincr=2*Pi/maprs;
      zeropt=point(0,0,0);
      flipsign=1;
      if flip then flipsign=-1;
      for i=0 to maprs do {
         ldir1=flipsign*(ttf*(mappointlist(i)-zeropt));
         ldir1=vecnorm(ldir1*earthdir);
         cnt.addpoint(point(mapposxlist(i),mapposy+mapsize*(1-acos(poledir^ldir1)/Pi),0.002));
      }
      cnt.close;
      lightmap=lightmapframe.add("SolidObject","enablelight":false,"renderback":false,
         "color":color(0,0,0,0.75),"BlendType":BlendTranslucent,"DepthMask":DepthMaskEnable);
      lightmap.CreateFlatShape(cnt);
   }


   if menu.WasModified and (menu.SelectID=="showpoints") then
      ptsframe.visible=menu.Checked("showpoints");
   
   if menu.WasModified and (menu.SelectID=="zoom") then {
      trackmarspos=menu.Checked("zoom");
      zoom_pos0=myviewport.camerapos;
      if trackmarspos then {
         if not(zoomingin) and not(zoomingout) then dist_unzoomed=distance(point(0,0,0),myviewport.camerapos);
         zoomingin=true;zoomingout=false;
         zoomfrac=0;
         zoom_pos1=point(0,0,0)-dist_zoomed*myviewport.cameradir;
      } else {
         if not(zoomingin) and not(zoomingout) then dist_zoomed=distance(point(0,0,0),myviewport.camerapos);
         zoomingout=true;zoomingin=false;
         zoomfrac=1;
         zoom_pos1=point(0,0,0)-dist_unzoomed*myviewport.cameradir;
      }
   }
   
   if menu.WasModified and (menu.SelectID=="showmap") then {
      mapframe.visible=menu.Checked("showmap");
      lightmapframe.visible=mapframe.visible;
   }
   
   if zoomingin then {
      zoomfrac=zoomfrac+0.01;
      if zoomfrac>1 then { zoomfrac=1; zoomingin=false; }
      myviewport.camerapos=(1-zoomfrac)*zoom_pos0+zoomfrac*zoom_pos1;
   }
   if zoomingout then {
      zoomfrac=zoomfrac-0.01;
      if zoomfrac<0 then { zoomfrac=0; zoomingout=false; }
      myviewport.camerapos=zoomfrac*zoom_pos0+(1-zoomfrac)*zoom_pos1;
   }
   
   if menu.WasModified and (menu.SelectID=="jump") then {
      tm=root.time;tm1=time;
      mindiff=10000000;
      tm0=time(tm.year,3,21,0,0,0);
      if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
      tm0=time(tm.year,6,21,0,0,0);
      if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
      tm0=time(tm.year,9,21,0,0,0);
      if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
      tm0=time(tm.year,12,21,0,0,0);
      if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
      tm0=time(tm.year+1,3,21,0,0,0);
      if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
      root.time=tm1;
   }
   
   
#mechanism to co-move with Earth
   root.SC.Universe.transf.reset;
   tf1=Transformation;
   tf1.translate(zoomfrac*@vector(earthpositframe.totaltransformation.origin));
   tf2=tf1;tf2.invert;
   root.viewports.main.transf=tf2;
   
   myviewport.focaldistance=distance(point(0,0,0),myviewport.camerapos);


   render;
}