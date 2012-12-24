
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");


ssysframe=ssys_create;
ssys_createearth(200);

ssys_showstarback(false);

root.SC.Universe.SolarSystem.EarthOrbit.visible=false;



#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);


#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;
dialogframe.EnabeMouseArrow(point(0.25,0.75),point(0,0.5),point(0.5,1.0),0.03);

chk_showhorizon=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_showhorizon.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Show horizon');
yps=yps-ysep;

chk_showeqgrid=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_showeqgrid.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Show equatorial grid');
yps=yps-ysep;

chk_showazgrid=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_showazgrid.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Show alt-azimuth grid');
yps=yps-ysep;

chk_corotate=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_corotate.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Co-rotate with Earth');
yps=yps-ysep;

lst_position=dialogframe.add("ListControl","Size":0.03,"Position":point(0.02,yps),"CountY":1,"SizeX":0.25);
positionslattit=list;positionslongit=list;
lst_position.List.add('Home');                positionslattit.add(homelattitude);   positionslongit.add(homelongitude);
lst_position.List.add('North Pole');          positionslattit.add(deg2rad(89.9));   positionslongit.add(homelongitude);
lst_position.List.add('Nortern hemisphere');  positionslattit.add(deg2rad(51.0));   positionslongit.add(homelongitude);
lst_position.List.add('Equator');             positionslattit.add(deg2rad(0.0));    positionslongit.add(homelongitude);
lst_position.List.add('Southern hemisphere'); positionslattit.add(deg2rad(-45.0));  positionslongit.add(homelongitude);
lst_position.List.add('South Pole');          positionslattit.add(deg2rad(-89.9));  positionslongit.add(homelongitude);
lst_position.SelectIdx=0;
yps=yps-ysep;

bt_jumptime=dialogframe.add("ButtonControl");
bt_jumptime.content='Jump to next point';
bt_jumptime.position=point(0.025,yps);
bt_jumptime.size=0.03;
bt_jumptime.sizex=0.30;
yps=yps-ysep;




globeframe=GetPlanetFrame("Earth");
earthframe=GetPlanetBodyFrame("Earth");


globeradius=autokm(0.03);

#shadow
if true then {
   shad=globeframe.add("sphere","Name":"shadow");
   shad.radius=0.99*globeradius;
   shad.EnableLight=false;
   shad.BlendType=BlendTranslucent;
   shad.DepthMask=DepthMaskDisable;
   shad.renderfront=false;
   shad.renderback=true;
   shad.color=color(0,0,0,0.8);
   shad.resolution=50;
   shad.canbuffer=true;
}


#create halo
halfcircle=FlatContourSet;
haloframe=globeframe.addviewdirframe(point(0,0,0),"haloframe");
halfcircle.generate(functor("point(globeradius*sin(a),globeradius*cos(a),0)","a"),Pi,0,40);
halo=haloframe.add("SolidObject","Name":"Halo");
halo.Revolve(halfcircle,40);
halo.BlendType=BlendTransparent;
halo.RenderBack=true;
halo.DepthMask=DepthMaskDisable;
halo.EnableLight=false;
halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/globeradius)),0.15)","p"),VertexPropertyColor);
halo.canbuffer=true;

sp=globeframe.add("sphere");
sp.color=color(1,1,1);

if true then {
   tx=globeframe.createtexture("star",DataDir+"\textures\star4.bmp");
   st1=globeframe.add("StarGlobe","Name":"StarglobeFront");
   st1.radius=globeradius;
   st1.texture="star";
   st1.StarSize=0.01*globeradius;
   st1.LineSize=0.01*globeradius;
   st1.linecolor=color(0,0.5,1,0.4);
   st1.renderback=false;
   st2=globeframe.add("StarGlobe","Name":"StarGlobeBack");
   st2.radius=globeradius;
   st2.texture="star";
   st2.StarSize=0.01*globeradius;
   st2.LineSize=0.01*globeradius;
   st2.renderfront=false;
   st2.linecolor=color(0,0.5,1,0.4);
   st2.color=color(1,1,1,0.15);
}

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
   mw.color=color(0.3,0.5,1,0.45);
   mw.texture=tx.name;
   mw.BlendType=BlendTransparent;mw.DepthMask=DepthMaskDisable;
   mw.renderback=false;mw.renderfront=true;
   mw.radius=globeradius;
}

#Sun path
c1=globeframe.add("curve");
c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
c1.color=color(1,0.75,0);
c1.size=2;

arrow=globeframe.add("curve");
arrow.size=3;
arrow.color=color(1,0,0);
arrow.Arrow2Size=globeradius*0.08;


#create earth pole axis & equator
eaxis=earthframe.add("Curve","Name":"Axis");
eaxis.makeline(point(0,0,-1*globeradius),point(0,0,+1*globeradius));
eaxis.size=2;
eaxis.color=color(1,0.25,0);
c1=earthframe.add("curve");
c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
c1.color=color(1.0,0.25,0);
c1.size=2;

#create equatorial grid
eqgridframe=globeframe.inclin.addsubframe("eqgrid");
for i=-8 to 8 do {
   a=deg2rad(10*i);
   crv=eqgridframe.add("Curve","Size":1,"Color":color(0.7,0.3,0));
   crv.makecircle(point(0,0,globeradius*sin(a)),vector(0,0,1),globeradius*cos(a),80);
}
for i=0 to 17 do {
   a=deg2rad(10*i);
   crv=eqgridframe.add("Curve","Size":1,"Color":color(0.7,0.2,0));
   crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),globeradius,80);
   if (i==0) or (i==9) then { crv.Size=2; crv.color=color(1,0.25,0,1); }
}
eqgridframe.visible=false;


#create viewer frame, but at earth center
vframe=earthframe.addsubframe("ViewerFrame");

function setviewerposition(ilongit,ilattit)
{
   vframe.transf.reset;
   vframe.transf.rotate(vector(0,0,1),ilongit);
   vframe.transf.rotate(vector(0,1,0),Pi/2-ilattit);
}

setviewerposition(HomeLongitude,HomeLattitude);


#create zenith axis & horizon
horframe=vframe.addsubframe("horizon");
horframe.visible=false;
zaxis=horframe.add("Curve","Name":"ZenithAxis");
zaxis.makeline(point(0,0,0),point(0,0,globeradius));
zaxis.size=2;
zaxis.color=color(0.3,0.6,0.3);
horline=horframe.add("Curve","Name":"HorizLine");
horline.makecircle(point(0,0,0),vector(0,0,1),1.01*globeradius,80);
horline.size=2;
horline.color=color(0.3,0.6,0.3);
horplane=horframe.add("Cylinder","Name":"HorPlane");
horplane.Axis=vector(0,0,0.01*globeradius);
horplane.radius=globeradius;
horplane.resolution=60;
horplane.EnableLight=false;
horplane.color=color(0.2,0.5,0.2,0.35);
horplane.BlendType=BlendTranslucent;
horplane.DepthMask=DepthMaskDisable;

horcap=horframe.add("Surface","Name":"HorCap");
horcap.generate(functor("point(1.01*globeradius*cos(u)*cos(v),1.01*globeradius*cos(u)*sin(v),-1.01*globeradius*sin(u))","u","v"),Pi/2,0,30,0,2*Pi,60);
horcap.enablelight=false;
horcap.color=color(0,0,0,0.7);
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


#initialise viewing parameters
vp=GetViewPort;
vp.cameradir=vecnorm(vector(0,-1,-0.5));
vp.camerapos=point(0,0,0)-4*globeradius*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;
vp.NearClipPlane=0.1*globeradius;
vp.FarClipPlane=20*globeradius;
vp.FocalDistance=4*globeradius;


root.time=CurrentTimeUT;

createvar(vc);
createvar(tm);
ang=0;
root.TimeSpeedFactor=1000;
while true do {
   tm=root.time;
   lasttimestep=incrtime;
   
   if false then {
      ang=ang+0.01;
      if (ang>2*Pi) then { ang=ang-2*Pi; arrow.reset; }
      pt=point(globeradius*cos(ang),0,globeradius*sin(ang));
      pt=point(cos(lattit)*globeradius,0,sin(lattit)*globeradius);
      pt=earthframe.getparent.transf*earthframe.transf*pt;
      arrow.addpoint(pt);
   }
   
   #track sun position
   vc=vecnorm(point(0,0,0)-earthframe.totaltransformation.origin);
   sunpoint.position=@point(globeradius*1.0*vc);
   
   #mechanism to co-move camera with earth
   if true then {
      ttf=globeframe.totaltransformation;
      if chk_corotate.checked then {
         inclin=deg2rad(-23.4457889);
         poledir=vector(0,-1*sin(inclin),cos(inclin));
        if not(root.Pauzed) then {
           rotang=lasttimestep*2*Pi*365/364/(24*60*60);
            vp.CameraPos=point(0,0,0)+vecrotate(vp.CameraPos-point(0,0,0),poledir,rotang);
            vp.CameraDir=vecrotate(vp.CameraDir,poledir,rotang);
            vp.CameraUpDir=vecrotate(vp.CameraUpDir,poledir,rotang);
         }
      }
      ttf.invert;
      root.viewports.main.transf=ttf;
   }
   
   if chk_showhorizon.WasModified then horframe.visible=chk_showhorizon.Checked;

   if chk_showeqgrid.WasModified then eqgridframe.visible=chk_showeqgrid.Checked;

   if chk_showazgrid.WasModified then azgridframe.visible=chk_showazgrid.Checked;
   
   if lst_position.WasModified then {
      setviewerposition(positionslongit(lst_position.SelectIdx),positionslattit(lst_position.SelectIdx));
   }
   
   if bt_jumptime.WasModified then {
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
   if RightMouseClicked then stop;


   render;
}
