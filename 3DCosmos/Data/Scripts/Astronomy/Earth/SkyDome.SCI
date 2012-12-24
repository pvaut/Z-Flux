codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

ffc=0;

#codeblock readtextfile(ScriptFilepath+"\_Sub\_SUB_general.sci");


function calclongitlattit(pos,longit,lattit)
{
   pos=vecnorm(pos);
   lattit=asin(pos.z);
   longit=Pi+angle(pos.x,pos.y);
   while longit>Pi do longit=longit-2*Pi;
   while longit<-1*Pi do longit=longit+2*Pi;
}

function longit2string(longit)
{
   longit=longit/Pi*180;
   while (longit<-180) do longit=longit+360;
   while (longit> 180) do longit=longit-360;
   if longit>=0 then return(str(round(longit,1))+"° "+'East');
   else return(str(round(-1*longit,1))+"° "+'West');
}

function lattit2string(lattit)
{
   lattit=lattit/Pi*180;
   if lattit>=0 then return(str(round(lattit,1))+"° "+'North');
   else return(str(round(-1*lattit,1))+"° "+'South');
}

#############################################################################################
#############################################################################################
#############################################################################################
# START SHOW MOVEMENT ROUTINE
#############################################################################################
#############################################################################################
#############################################################################################

function MovementInit(infoset)
{

   infoset.scenescale=100000;
   infoset.globeradius=7*infoset.scenescale;
   infoset.earthradius=6371;

   sss=T_getscene;
   sss.VolumeShadowReset;
   sss.VolumeShadowAdd(0,color(0,0,0,0.25),0.01*infoset.scenescale,16*infoset.scenescale);

   myviewport=T_getviewport;
   myviewport.focaldistance=1.05*infoset.scenescale;
   myviewport.nearclipplane=infoset.scenescale;
   myviewport.farclipplane=200*infoset.scenescale;
   myviewport.cameradir=vecnorm(vector(0.1,-1,0));
   myviewport.cameraupdir=vector(0,0,1);
   cpos0=point(0,0,6*infoset.scenescale)-25*infoset.scenescale*myviewport.cameradir;
   myviewport.camerapos=cpos0;
   myviewport.enableusernavigation=true;
   
   offsetx=0.05;
   cl=color(0.7,0.7,0.7);
   PanelContentSub=root.SC.Universe.addscreenframe("PanelContentSub");
   PanelContentSub.add("Text3D","Content":'GMT Time (UT):',"EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.78,0),"Color":cl);
   PanelContentSub.add("Clock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.68,0),"Color":cl);
   PanelContentSub.add("Clock","Type":ClockTypeDate,"Size":0.02,"EnableLight":false,"Position":point(offsetx+0.045,0.58,0),"Color":cl);

   PanelContentSub.add("Text3D","Content":'Mean Solar Time:',"EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.48,0),"Color":cl);
   clk=PanelContentSub.add("Clock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.38,0),"Color":cl);
   clk.TimeShift=infoset.longit/Pi*12*60*60;

   #sun reference
   ssysframe.add("Sphere","color":color(1,1,0.5),"Radius":autokm(0.02),"EnableLight":false);
   
   earthframe=root.SC.Universe.SolarSystem.Earth;
   
   GetPlanetOrbit("Earth").visible=false;
   
   #Delete Earth object
   earthframe.Inclin.Globe.GlobeRendering.Dispose;
   earthglobeframe=earthframe.Inclin.Globe.addsubframe("GlobeRendering");
   
   #Create local viewer frame   
   earthpositframe=earthglobeframe.addsubframe("LocalPosit");
   earthpositframe.transf.origin=radial2point(infoset.earthradius,infoset.longit,infoset.lattit);
   earthpositframe.transf.Zaxis=@vector(radial2point(1,infoset.longit,infoset.lattit));
   earthpositframe.transf.Xaxis=vector(-1*sin(infoset.longit),cos(infoset.longit),0);
   earthpositframe.transf.Yaxis=vecnorm(earthpositframe.transf.Zaxis*earthpositframe.transf.Xaxis);
   
   #Earth surface
   surfaceframe=earthpositframe.addsubframe("Surface");
   surfaceframe.CastVolumeShadow=false;
   surfaceframe.add("Cylinder",
      "Position":point(0,0,-0.002*infoset.scenescale),
      "Radius":(20*infoset.scenescale),
      "Axis":vector(0,0,-0.00001*infoset.scenescale),
      "Color":color(30/255,60/255,30/255),
      "Resolution":80);
   surfaceframe.add("Cylinder",
      "Radius":(20*infoset.scenescale),
      "Position":point(0,0,-0.005*infoset.scenescale),
      "Axis":vector(0,0,-7.5*infoset.scenescale),
      "Color":color(0,0,0),
      "Resolution":80);
      
   #create viewer figure
   bl=Blob;
   r2=0.3;
   bl.AddSphere(point(0,0,0.95),0.2,0.3);
   bl.AddSegment(point(0,0,0),point(0,0,1),0.23,r2+0.03);
   bl.AddSegment(point(0.22,0,-1),point(0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(-0.22,0,-1),point(-0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(0.26,0,0.55),point(0.66,-0.4,0.94),0.185,r2);
   bl.AddSegment(point(-0.26,0,0.55),point(-0.66,-0.4,0.94),0.185,r2);
   sh=bl.CreateSolidShape(0.06);
   ttf=transformation;ttf.translate(vector(0,0,0.6*infoset.scenescale));ttf.scale(0.6*infoset.scenescale);
   sh.transform(ttf);
   ob=earthpositframe.add("SolidObject");
   ob.CreateShape(sh);
   ob.color=color(0.7,0.7,0.7);
      
      
   #create small sun
   smallsun=surfaceframe.add("sphere","Name":"SmallSun","radius":(0.13*infoset.scenescale),"color":color(1,1,0.5),"EnableLight":false);
   
   #create halo
   st_globeradius=str(infoset.globeradius);
   halfcircle=FlatContourSet;
   haloframe=earthpositframe.addviewdirframe(point(0,0,0),"haloframe");
   haloframe.CastVolumeShadow=false;
   halfcircle.generate(functor("point("+st_globeradius+"*sin(a),"+st_globeradius+"*cos(a),0)","a"),Pi,0,40);
   halo=haloframe.add("SolidObject","Name":"Halo");
   halo.Revolve(halfcircle,40);
   halo.BlendType=BlendTransparent;
   halo.RenderBack=true;
   halo.DepthMask=DepthMaskDisable;
   halo.EnableLight=false;
   halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/"+st_globeradius+")),0.25)","p"),VertexPropertyColor);
   halo.canbuffer=true;
      
   #Create wind directions
   signcolor=color(0.15,0.15,0.15);
   rd=8*infoset.scenescale;
   dirnames=list('East','North','West','South');
   for i=0 to 3 do {
      frame=surfaceframe.addviewdirframe(radial2point(rd,i*Pi/2,0),"DF_"+dirnames(i));
      frame.add("Text3D","Position":point(-0.28*infoset.scenescale,0.6*infoset.scenescale,0),
         "UnitX":vector(infoset.scenescale,0,0),"UnitY":vector(0,infoset.scenescale),"EnableLight":false,"Color":signcolor,"Content":dirnames(i));
      frame.add("Arrow","Position":point(0,0.2*infoset.scenescale,0),
         "Axis":vector(0,-0.4*infoset.scenescale,0),"NormAxis":vector(0,0,1),"Width":(0.2*infoset.scenescale),"EnableLight":false,"Color":signcolor);
   }
   surfaceframe.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,0,0),radial2point(rd,Pi,0));
   surfaceframe.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,-0.5*Pi,0),radial2point(rd,0.5*Pi,0));
      
   surfaceframe.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*infoset.scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(1,0,0),1.001*infoset.globeradius,80);
   surfaceframe.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*infoset.scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(0,1,0),1.001*infoset.globeradius,80);
   
   
   crv_sundir=surfaceframe.add("Curve","Name":"crv_sundir","EnableLight":false,"Color":color(1,1,0),"Size":2);
   crv_suntrace=surfaceframe.add("Curve","Name":"crv_suntrace","EnableLight":false,"Color":color(1,0.25,0),"Size":3);
   
   #create side panel
   if false then {
      PanelContent=SUB_G_leftpanel.addscreenframe("MarsSolarMovementInfo");
      offsetx=SUB_G_leftpaneloffsett+0.01;
      cl=SUB_G_sidepanelcolor;
      PanelContent.add("Text3D","Content":'Earth time (UT):',"EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.78,0),"Color":cl);
      PanelContent.add("Clock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.68,0),"Color":cl);
      PanelContent.add("Clock","Type":ClockTypeDate,"Size":0.02,"EnableLight":false,"Position":point(offsetx+0.045,0.58,0),"Color":cl);
      PanelContent.add("Text3D","Content":'Local Mars time:',"EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.18,0),"Color":cl);
      marsclock=PanelContent.add("Clock","Name":"MarsClock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.09,0),"Color":cl,"HasOwnTime":true);
      PanelLines_init(PanelContent,0.5);
      PanelLines_skip;
      PanelLines_add('Sun position:');
      sunpos_az=PanelLines_add("");
      sunpos_h=PanelLines_add("");
      infoset.panelcontent=panelcontent;
      infoset.marsclock=marsclock;
      infoset.sunpos_az=sunpos_az;
      infoset.sunpos_h=sunpos_h;
   }
}


function MovementAnimate(ilongit,ilattit)
{
   infoset=map;
   infoset.longit=ilongit;
   infoset.lattit=ilattit;
   
   MovementInit(Ref(infoset));

   distaxislast=-10;
   root.time=time(2008,9,6,0,0,0);
   
#   SUB_MarsSolarMovement_FindSunRise(Ref(infoset));
   
   earthpositframe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.LocalPosit;
   surfaceframe=earthpositframe.surface;
   minlast=floor(root.time.min/30);
   
#   SUB_showloading(false);
   
   root.TimeSpeedFactor=1000;
   root.time=currenttimeut;
   sundirprev=vector(0,0,-1);
   
   showconst=false;
   
   menu=T_createmenu;
   menu.add("",'Show globe');
   
   finished=false;
   while not(finished) do {
      if menu.WasModified and (menu.SelectID=='Show globe') then finished=true;
            
      incrtime;      
      
      
   #mechanism to co-move with position on Earth
      root.SC.Universe.transf.reset;
      tf1=earthpositframe.totaltransformation;
      tf2=tf1;tf2.invert;
      root.viewports.main.transf=tf2;#apply the inverse transformation to the viewport-> object becomes positioned in the origin
   
   #calculate position of sun in the local reference system
      sunpoint=tf2*point(0,0,0);
      sundir=vecnorm(sunpoint-point(0,0,0));
      surfaceframe.smallsun.position=point(0,0,0)+infoset.globeradius*sundir;
      surfaceframe.crv_sundir.makeline(point(0,0,0)+0.2*infoset.globeradius*sundir,sunpoint);
      sunaz=-0.5*Pi-angle(sundir.x,sundir.y);
      if sunaz<-1*Pi then sunaz=sunaz+2*Pi;
      if sunaz>Pi then sunaz=sunaz-2*Pi;
      sunang=Pi/2-vecangle(sundir,vector(0,0,1));
#      infoset.sunpos_az.content='Azimuth: '+str(round(sunaz/Pi*180,1))+"°";
#      infoset.sunpos_h.content='Height: '+str(round(sunang/Pi*180,1))+"°";
      sunc=sunang;
      if sunc>1 then sunc=1;
      if sunc<0 then sunc=0;
      sss.light0color=color(sunc,sunc,sunc,1);
      sss.VolumeShadowReset;
      sss.VolumeShadowAdd(0,color(0,0,0,0.5*sunc),0.01*infoset.scenescale,16*infoset.scenescale);
      render;
   }
   root.SC.Universe.transf.reset;
   root.viewports.main.transf.reset;
   root.SC.Universe.PanelContentSub.dispose;
   earthpositframe.dispose;
   ssys_showconstellations(false);
   root.SC.Universe.StarBackFrame.ConstellationNames.Visible=false;
   root.SC.Universe.StarBackFrame.StarNames.Visible=false;
   sss=T_getscene;
   sss.VolumeShadowReset;
   sss.light0color=color(1,1,1,1);
   T_delmenu;
}



#############################################################################################
#############################################################################################
#############################################################################################
# START MAIN ROUTINE
#############################################################################################
#############################################################################################
#############################################################################################

function createmainmenu()
{
   menu=T_createmenu;
   menu.add("",'Show daily movement',"ShowMovement");
   return(menu);
}




#Create solar system
ssysframe=ssys_create;
sss=T_getscene;
myviewport=T_getviewport;
#SUB_init;
#PanelLeft=SUB_createleftpanel;
#SUB_showloading(true);



ssys_showconstellations(false);
ssys_createearth(1000);
ssys_createmilkyway;
ssys_createconstellationnames;
ssys_createstarnames;
root.SC.Universe.StarBackFrame.ConstellationNames.Visible=false;
root.SC.Universe.StarBackFrame.StarNames.Visible=false;

sss.ambientlightcolor=color(0.05,0.05,0.05);
myviewport.NearClipPlane=0.01;
myviewport.FarClipPlane=10;
myviewport.focaldistance=0.0101;
myviewport.enableusernavigation=true;

myviewport.camerapos=radial2point(-3,0,0);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,0,1);

JID=0;#joystick ID

rootframe=sss.Universe;

resol=60;

#Create Earth globe
globeframe=rootframe.addsubframe("EarthGlobe");

surf=globeframe.add("sphere","radius":1,"position":point(0,0,0),"resolution":resol);
surf.canbuffer=true;
tx1=globeframe.createtexture("mars",DataDir+"\textures\earth_3.jpg");
surf.texture=tx1.name;


#create cursor point
cursorpos=point(-1,0,0);
cursor=globeframe.add("sphere","color":color(0,0.75,0),"radius":0.01);
cursor.position=cursorpos;
distaxislast=-100;

#create grid
gridframe=globeframe.addsubframe("grid");
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

#create windrose object
windrose=globeframe.addsubframe("WindRose");
windrosetx=windrose.CreateTexture("windrosetx",DataDir+"\textures\windrose1.jpg",DataDir+"\textures\windrose2.jpg");
windroserect=windrose.add("Rectangle","Position":point(-0.5,-0.5,0),"Axis1":vector(1,0,0),"Axis2":vector(0,1,0));
windroserect.texture=windrosetx.name;
windroserect.BlendType=BlendTranslucent;
windroserect.color=color(1,1,0.7,0.75);
windroserect.enablelight=false;
windroserect.DepthMask=DepthMaskDisable;
#windrose.visible=false;

PanelContentMain=root.SC.Universe.addscreenframe("PanelContentMain");

longinfo=PanelContentMain.add("TextControl","size":0.04,"Position":point(0,0.05));
lattinfo=PanelContentMain.add("TextControl","size":0.04,"Position":point(0,0.01));

menu=createmainmenu;

doalign=false;
ct=0;
distfac=1;dist=2;
while true do {
   incrtime;
   ct=ct+1;
   
   sss.Light0Pos=point(0,0,0)-100*myviewport.cameradir
      +30*myviewport.cameraupdir
      -40*myviewport.cameradir*myviewport.cameraupdir;
      

   cursorpos=point(0,0,0)-vecnorm(myviewport.cameradir);
   
   cursor.position=cursorpos;
   loc_z=vecnorm(cursorpos-point(0,0,0));
   loc_x=vecnorm(vector(0,0,1)*loc_z);
   loc_y=vecnorm(loc_z*loc_x);

   #re-position windrose
   cursorsize=0.03+0.3*distfac;
   windrose.transf.origin=@point(1.001*@vector(cursorpos));
   windrose.transf.Xaxis=cursorsize*loc_x;
   windrose.transf.Yaxis=cursorsize*loc_y;
   windrose.transf.Zaxis=cursorsize*loc_z;
    
   render;
  

   
   
   #blink cursor
   if ct/30-floor(ct/30)<0.3 then cursor.visible=false; else cursor.visible=true;
   
   
  longit=0;lattit=0;
  calclongitlattit(@vector(cursorpos),Ref(longit),Ref(lattit));
  longinfo.content='Longit'+"="+Longit2string(longit);
  lattinfo.content='Lattit'+"="+lattit2string(lattit);
   
   #show sun movement
   if menu.wasmodified and (menu.SelectID=="ShowMovement") then {
      T_delmenu;
      oldcamerapos=myviewport.camerapos;
      oldcameradir=myviewport.cameradir;
      oldcameraupdir=myviewport.cameraupdir;
      oldnearclipplane=myviewport.nearclipplane;
      oldfarclipplane=myviewport.farclipplane;
      oldfocaldistance=myviewport.focaldistance;
      globeframe.visible=false;
      PanelContentMain.visible=false;
      MovementAnimate(longit,lattit);
      globeframe.visible=true;
      PanelContentMain.visible=true;
      myviewport.camerapos=oldcamerapos;
      myviewport.cameradir=oldcameradir;
      myviewport.cameraupdir=oldcameraupdir;
      myviewport.nearclipplane=oldnearclipplane;
      myviewport.farclipplane=oldfarclipplane;
      myviewport.focaldistance=oldfocaldistance;
      menu=createmainmenu;
   }
   
      
   
}
