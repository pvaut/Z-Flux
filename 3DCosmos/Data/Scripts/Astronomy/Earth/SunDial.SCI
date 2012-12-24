
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

#Earth radius
radius=6378.0;

#Sundial dimensions
sundialhradius=1000;
sundialthickness=0.0002*sundialhradius;
sundiallength=300;
offs=sundialhradius/5;

#List of all active sundials
sundials=List;


function cleartraces()
{
   foreach sundial in sundials do {
      sundial.trackframe.clearobjects;
      sundial.lasttracepoint=point(999999,999999,999999);
   }
}

function buildsundials()
{
   sundials.clear;asundial=Map;
   
   asundial.dir=vector(1,0,0);asundial.center=point(sundialhradius,0,sundialhradius-offs);
   sundials.add(asundial);
   
   asundial.dir=vector(0,1,0);asundial.center=point(0,sundialhradius,sundialhradius-offs);
   sundials.add(asundial);
   
   asundial.dir=vector(-1,0,0);asundial.center=point(-1*sundialhradius,0,sundialhradius-offs);
   sundials.add(asundial);
   
   
   asundial.dir=vector(0,-1,0);asundial.center=point(0,-1*sundialhradius,sundialhradius-offs);
   sundials.add(asundial);
   
   asundial.dir=vector(0.0001,0,1);asundial.center=point(0,0,2*sundialhradius-offs);
   sundials.add(asundial);
   
   #create sundial objects
   foreach sundial in sundials do {
      sundial.frame=locframe.addsubframe("SundialFrame");
      sundial.trackframe=sundial.frame.addsubframe("TrackFrame");
      sundial.frame.transf.origin=sundial.center;
      sundial.frame.transf.Zaxis=vecnorm(sundial.dir);
      sundial.frame.transf.Xaxis=vecnorm(vector(0,0,1)*sundial.dir);
      sundial.frame.transf.Yaxis=vecnorm(sundial.frame.transf.Zaxis*sundial.frame.transf.Xaxis);
      br=sundial.frame.add("Bar","Position":point(-1*sundialhradius,-1*sundialhradius,sundialthickness),"Color":Color(0.9,0.9,0.9),
                       "SizeX":(2*sundialhradius),"SizeY":(2*sundialhradius),"SizeZ":(2*sundialthickness));
      ax=sundial.frame.add("Cylinder","Axis":vector(0,0,sundiallength),"Radius":(sundialhradius/30),"Color":color(0.3,0.8,0.3));
      sundial.top=point(0,0,sundiallength);
      sundial.traceplane=CreatePlane1(point(0,0,0.05*sundiallength),vector(0,0,1));
      sundial.lasttracepoint=point(999999,999999,999999);
   }
   
}

function setposition(lattit)
{
   locframe.transf.reset;
   locframe.transf.rotate(vector(0,1,0),Pi/2-lattit);
   locframe.transf.translate(vector(0,0,radius));
}

lattit=51.0;

###############################################################################
# START
###############################################################################

ssysframe=ssys_create;
ssys_createearth(1);
#ssys_createluna(1);
ssys_showstarback(true);
GetPlanetOrbit("Earth").visible=false;

root.time=time(2009,9,1,9,0,0);
root.TimeSpeedFactor=300;


#Create dialog controls
dialogframe=T_createdialogframe;yps=0.95;ysep=0.05;

chk_corotate=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_corotate.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Co-rotate with Earth');
yps=yps-ysep;

chk_varyseason=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_varyseason.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Vary season');
yps=yps-ysep;

chk_track=dialogframe.add("CheckControl","Size":0.03,"Position":point(0.02,yps));
chk_track.checked=false;
dialogframe.add("TextControl","Size":0.03,"Position":point(0.06,yps),"Content":'Track position');
yps=yps-ysep;

lst_position=dialogframe.add("ListControl","Size":0.03,"Position":point(0.02,yps),"CountY":1,"SizeX":0.25);
positions=list;
lst_position.List.add('North Pole');          positions.add(deg2rad(89.0));
lst_position.List.add('Nortern hemisphere');  positions.add(deg2rad(51.0));
lst_position.List.add('Equator');             positions.add(deg2rad(0.0));
lst_position.List.add('Southern hemisphere'); positions.add(deg2rad(-45.0));
lst_position.List.add('South Pole');          positions.add(deg2rad(-89.0));
lst_position.SelectIdx=1;

ActivateControl(chk_corotate);


#enhanced texture on Earth
eglobe=GetPlanetBodyFrame("Earth");
etx2=eglobe.CreateTexture("Earth2",DataDir+"\textures\earth_3.jpg");
eglobe.GlobeRendering.Earth.Texture=etx2.name;


#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);

#initialise shadow
root.SC.VolumeShadowAdd(0,color(0,0,0,0.5),10,10000);





earthframe=GetPlanetFrame("Earth");
globeframe=GetPlanetBodyFrame("Earth");

#Create frame for local position on Earth
locframe=globeframe.addsubframe("LocFrame");
setposition(deg2rad(51.0));

#Build the sundials
buildsundials;

#create sun arrow
arrowframe=ssysframe.addsubframe("ArrowFrame");
arrowframe.CastVolumeShadow=false;
arrowframe.add("Arrow","Position":point(-5*sundialhradius,0,0),"Axis":vector(2.0*sundialhradius,0,0),"NormAxis":vector(0,1,0),"Width":200,"Thickness":0,
               "EnableLight":false,"Color":Color(1,0.5,0));


#initialise viewing parameters
vp=GetViewPort;
vp.enableusernavigation=true;
vp.NearClipPlane=4000;
vp.FarClipPlane=160000;
vp.FocalDistance=20000;
vp.cameradir=vecnorm(earthframe.Transf.origin-point(0,0,0))-vector(0,0,0.4);
vp.camerapos=point(0,0,0)-vp.FocalDistance*vp.cameradir;

varyseason=false;

lasthour=root.time.hour;
createvar(lasttimestep);
while true do {
   if not(varyseason) then lasttimestep=incrtime;
   else {
      lasttimestep=24*60*60;
      root.time=root.time+24*60*60;
   }
   
   if chk_varyseason.WasModified then {
      varyseason=false;
      if chk_varyseason.Checked then varyseason=true;
      cleartraces;
   }
   
   if lst_position.WasModified then {
      setposition(positions(lst_position.SelectIdx));
      cleartraces;
   }
   
  if chk_corotate.Checked then {#co-move camera with Earth rotation
     inclin=deg2rad(-23.4457889);
     poledir=vector(0,-1*sin(inclin),cos(inclin));
     if not(root.Pauzed) then {
        rotang=lasttimestep*2*Pi*365/364/(24*60*60);
        vp.CameraPos=point(0,0,0)+vecrotate(vp.CameraPos-point(0,0,0),poledir,rotang);
        vp.CameraDir=vecrotate(vp.CameraDir,poledir,rotang);
        vp.CameraUpDir=vecrotate(vp.CameraUpDir,poledir,rotang);
      }
  }
   
   
   #co-move camera with Earth movement (enhanced precision mechanism)
   root.sc.universe.transf.reset;
   ttf=earthframe.totaltransformation;ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #position sun direction arrow
   sundialpos=earthframe.transf*earthframe.inclin.transf*globeframe.transf*locframe.transf*point(0,0,0);
   arrowframe.transf.origin=sundialpos;
   arrowframe.transf.Xaxis=vecnorm(sundialpos-point(0,0,0));
   arrowframe.transf.Yaxis=vecnorm(arrowframe.transf.Xaxis*vector(0,1,0));
   arrowframe.transf.Zaxis=vecnorm(arrowframe.transf.Xaxis*arrowframe.transf.Yaxis);

   zenithdir=earthframe.transf*earthframe.inclin.transf*globeframe.transf*locframe.transf*vector(0,0,1);
   
   #track sundial paths
   if chk_track.Checked and (arrowframe.transf.Xaxis^zenithdir<0) then {
      foreach sundial in sundials do {
         ttf=earthframe.transf*earthframe.inclin.transf*globeframe.transf*locframe.transf*sundial.frame.transf;
         ttf.invert;
         pline=CreateLine1(ttf*point(0,0,0),sundial.top);
         cpoint=sundial.traceplane and pline;
         if (pline.Direction.z<0) then {
            if (abs(cpoint.x)<sundialhradius) and (abs(cpoint.y)<sundialhradius) then {
               if distance(cpoint,sundial.lasttracepoint)>60 then
                  sundial.trace=sundial.trackframe.add("Curve","Color":color(1,0,0),"Size":2);
               sundial.trace.addpoint(cpoint);
               sundial.lasttracepoint=cpoint;
            }
            if lasthour<root.time.hour then {#create hour indication
               cp2=cpoint;#clip to edge of sundial
               if cp2.x>sundialhradius then { cp2.y=cp2.y*sundialhradius/cp2.x; cp2.x=sundialhradius; }
               if cp2.x<-1*sundialhradius then { cp2.y=-1*cp2.y*sundialhradius/cp2.x; cp2.x=-1*sundialhradius; }
               if cp2.y>sundialhradius then { cp2.x=cp2.x*sundialhradius/cp2.y; cp2.y=sundialhradius; }
               if cp2.y<-1*sundialhradius then { cp2.x=-1*cp2.x*sundialhradius/cp2.y; cp2.y=-1*sundialhradius; }
               line=sundial.trackframe.add("Curve","Color":color(1,0,0),"Size":1);
               line.makeline(point(0,0,0),cp2);
            }
         }
      }
   }
   
   
   lasthour=root.time.hour;
   render;
}
