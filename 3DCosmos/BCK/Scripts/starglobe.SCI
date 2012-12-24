
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
codeblock readtextfile(ScriptDir+"\SSYS.sci");

lattit=51/180*Pi;

ssysframe=ssys_create;
ssys_createearth(200);

ssys_showstarback(false);

#Create dialog controls
dialogframe=T_createdialogframe;

chk_showhorizon=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,0.95));
chk_showhorizon.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,0.95),"Content":"Toon horizon");

chk_corotate=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,0.9));
chk_corotate.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,0.9),"Content":"Roteer mee");




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
   shad.color=color(0,0,0,0.6);
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
halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/globeradius)),0.25)","p"),VertexPropertyColor);
halo.canbuffer=true;

sp=globeframe.add("sphere");
sp.color=color(1,1,1);

if true then {
   tx=globeframe.createtexture("star",DataDir+"\textures\star4.bmp");
   st1=globeframe.add("StarGlobe","Name":"StarglobeFront");
   st1.radius=globeradius;
   st1.texture="star";
   st1.StarSize=0.01*globeradius;
   st1.LineSize=0.005*globeradius;
   st1.renderback=false;
   st2=globeframe.add("StarGlobe","Name":"StarGlobeBack");
   st2.radius=globeradius;
   st2.texture="star";
   st2.StarSize=0.01*globeradius;
   st2.LineSize=0.005*globeradius;
   st2.renderfront=false;
   st2.color=color(1,1,1,0.3);
}

c1=globeframe.add("curve");
c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
c1.color=color(1,0.5,0);
c1.size=2;

arrow=globeframe.add("curve");
arrow.size=3;
arrow.color=color(1,0,0);
arrow.Arrow2Size=globeradius*0.08;


#create earth pole axis & equator
eaxis=earthframe.add("Curve","Name":"Axis");
eaxis.makeline(point(0,0,-1*globeradius),point(0,0,+1*globeradius));
eaxis.size=2;
eaxis.color=color(0,0.3,0.3);
c1=earthframe.add("curve");
c1.makecircle(point(0,0,0),vector(0,0,1),globeradius,80);
c1.color=color(0,0.5,0.5);
c1.size=2;

#create viewer frame, but at earth center
vframe=earthframe.addsubframe("ViewerFrame");
vframe.transf.rotate(vector(0,1,0),Pi/2-lattit);

vframe.visible=false;

#create zenith axis & horizon
zaxis=vframe.add("Curve","Name":"ZenithAxis");
zaxis.makeline(point(0,0,0),point(0,0,globeradius));
zaxis.size=2;
zaxis.color=color(0.3,0.6,0.3);

horline=vframe.add("Curve","Name":"HorizLine");
horline.makecircle(point(0,0,0),vector(0,0,1),1.01*globeradius,80);
horline.size=2;
horline.color=color(0.3,0.6,0.3);

horplane=vframe.add("Cylinder","Name":"HorPlane");
horplane.Axis=vector(0,0,0.01*globeradius);
horplane.radius=globeradius;
horplane.resolution=60;
horplane.EnableLight=false;
horplane.color=color(0.2,0.5,0.2,0.35);
horplane.BlendType=BlendTranslucent;
horplane.DepthMask=DepthMaskDisable;

horcap=vframe.add("Surface","Name":"HorCap");
horcap.generate(functor("point(1.01*globeradius*cos(u)*cos(v),1.01*globeradius*cos(u)*sin(v),-1.01*globeradius*sin(u))","u","v"),Pi/2,0,30,0,2*Pi,60);
horcap.enablelight=false;
horcap.color=color(0,0,0,0.7);
horcap.BlendType=BlendTranslucent;
horcap.DepthMask=DepthMaskDisable;

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
vp.EyeSeparation=vp.FocalDistance/10.0;


root.time=time(2008,3,21,0,0,0);

createvar(vc);
createvar(tm);
ang=0;
root.TimeSpeed=100;
while true do {
   tm=root.time;
   incrtime;
   
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
         rotang=root.Timespeed*2*Pi*365/364/(24*60*60);
         vp.CameraPos=point(0,0,0)+vecrotate(vp.CameraPos-point(0,0,0),poledir,rotang);
         vp.CameraDir=vecrotate(vp.CameraDir,poledir,rotang);
         vp.CameraUpDir=vecrotate(vp.CameraUpDir,poledir,rotang);
      }
      ttf.invert;
      root.viewports.main.transf=ttf;
   }
   
   if chk_showhorizon.WasModified then vframe.visible=chk_showhorizon.Checked;
   
   render;
}
