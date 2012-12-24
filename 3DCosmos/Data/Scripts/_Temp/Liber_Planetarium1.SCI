
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

longit=(3+43/60.0)/180*Pi;
lattit=(51+3/60.0)/180*Pi;
ei=+23.4457889/180*Pi;

scfac=100000;


ssysframe=ssys_create;
ssys_createearth(1);
ssys_createluna(2);
ssys_createmilkyway;
ssys_createconstellationnames;
#ssys_createstarnames;

ssys_showconstellations(true);

root.framerate=30;


#Create moon halo object
frame_moonhalo=ssysframe.addviewdirframe(point(0,0,0),"MoonHalo");
ssysframe.moveobject(frame_moonhalo.name,-1000);
tx=frame_moonhalo.createtexture("halo",DataDir+"\textures\star1.bmp",DataDir+"\textures\star1.bmp");
moonhalo=frame_moonhalo.add("Rectangle","EnableLight":false,
   "Color":color(0.7,0.7,0.7,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
fc=scfac*0.6;
moonhalo.Texture=tx.name;
moonhalo.position=point(-1*fc,-1*fc,0);
moonhalo.Axis1=vector(2*fc,0,0);
moonhalo.Axis2=vector(0,2*fc,0);

#Create sun halo object
#frame_sunhalo=ssysframe.addviewdirframe(point(0,0,0),"SunHalo");
frame_sunhalo=ssysframe.addsubframe("SunHalo");
ssysframe.moveobject(frame_sunhalo.name,-1000);
tx=frame_sunhalo.createtexture("halo",DataDir+"\textures\star1.bmp");
#large halo
rc=frame_sunhalo.add("Rectangle","EnableLight":false,
   "Color":color(1,1,0.7,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
fc=scfac*1;
rc.Texture=tx.name;
rc.position=point(-1*fc,-1*fc,0);
rc.Axis1=vector(2*fc,0,0);
rc.Axis2=vector(0,2*fc,0);
#small halo
rc=frame_sunhalo.add("Rectangle","EnableLight":false,
   "Color":color(1,1,0.5,1.0),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
fc=scfac*0.05;
rc.Texture=tx.name;
rc.position=point(-1*fc,-1*fc,0);
rc.Axis1=vector(2*fc,0,0);
rc.Axis2=vector(0,2*fc,0);
#sunset/sunrise halo
sunsethalo=frame_sunhalo.add("Rectangle","EnableLight":false,
   "Color":color(1,0.5,0.0,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
fc=scfac*1;
sunsethalo.Texture=tx.name;
sunsethalo.position=point(-3*fc,-1*fc,0);
sunsethalo.Axis1=vector(6*fc,0,0);
sunsethalo.Axis2=vector(0,2*fc,0);

#hide Earth orbit
root.SC.Universe.SolarSystem.EarthOrbit.visible=false;


#create planet objects
frame_planets=ssysframe.addsubframe("PlanetsIndicators");
ssysframe.moveobject(frame_planets.name,-1000);
planlist=list("Mercurius","Venus","Mars","Jupiter","Saturnus","Neptunus");
for i=0 to planlist.size-1 do {
   ssys_createplanet(planlist(i),1);
   GetPlanetOrbit(planlist(i)).visible=false;
   fr=GetPlanetFrame(planlist(i));
   fr.clearobjects;
   fr=frame_planets.addsubframe(planlist(i));
   fr.add("LightPoint","Size":5000,"Color":color(1,0.5,0));
   fr.add("TextFlat","Size":8000,"Color":color(1,0.25,0,0.75),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,
      "Content":(" "+planlist(i)));
}

#starback=root.SC.Universe.StarBackFrame.StarBack;
#starback.LineSize=autokm(1000);
#starback.LineColor=color(1,0,0);


#ssysframe.add("Sphere","Name":"Sun","Radius":autokm(0.01),"EnableLight":false);

earthframe=GetPlanetBodyFrame("Earth");
earthframe.GlobeRendering.visible=false;
myviewport=T_getviewport;
myviewport.EnableUserNavigation=false;

#create viewer frame, but at earth center
vframe=earthframe.addsubframe("ViewerFrame");
vframe.transf.rotate(vector(0,1,0),Pi/2-lattit);
vframe.transf.rotate(vecrotate(vector(0,0,1),vector(0,1,0),-1*(Pi/2-lattit)),longit);



#s=20000*1000;
#vframe.add("Bar","Position":point(0,0,0),"SizeX":s,"SizeY":s,"SizeZ":s);

#vframe.add("Curve").makeline(point(0,0,0),point(0,0,2*s));


#Build scene

#atmosphere horizon glow
#atm1=vframe.add("Sphere","Name":"AtmopshereGlow");
atm1=vframe.add("Cylinder","Name":"AtmopshereGlow");
vframe.createtexture("Atm1",DataDir+"\textures\gradient1.bmp",DataDir+"\textures\gradient1.bmp");
atm1.renderback=true;atm1.renderfront=true;
atm1.enablelight=false;
atm1.Texture="Atm1";
atm1.BlendType=BlendTransparent;atm1.DepthMask=DepthMaskDisable;
atm1.radius=1.0*scfac;
atm1.Axis=vector(0,0,1.5*scfac);
atm1.IsClosed=false;
atm1.position=point(0,0,-0.05*scfac);

#full atmosphere (equal light)
atm2=vframe.add("Sphere","Name":"AtmosphereEqual");
atm2.renderback=true;atm2.renderfront=false;
atm2.enablelight=false;
atm2.BlendType=BlendTransparent;atm2.DepthMask=DepthMaskDisable;
atm2.radius=2*scfac;
atm2.resolution=5;

#azimuth grid
#for i=0 to 15 do {
#   ang=i/16*2*Pi;
#   vframe.add("Curve","Color":color(0,0.6,0.5,0.5),"BlendType":BlendTransparent,
#      "Size":2).makecircle(point(0,0,0),vector(cos(ang),sin(ang),0),1.3*scfac,200);
#}
#for i=0 to 6 do {
#   ang=i/6*Pi/2;
#   vframe.add("Curve","Color":color(0,0.6,0.5,0.5),"BlendType":BlendTransparent,
#      "Size":2).makecircle(point(0,0,1.3*scfac*sin(ang)),vector(0,0,1),1.3*scfac*cos(ang),200);
#}

#backdrop
bdframe=vframe.addsubframe("BackDropFrame");
bdframe.transf.rotate(vector(0,0,1),-42/180*Pi-Pi);
vframe.createtexture("bd",DataDir+"\textures\panovsrug.jpg",DataDir+"\textures\panovsrug_mask.jpg");
vframe.createtexture("bd2",DataDir+"\textures\koepel.jpg",DataDir+"\textures\koepel_mask.jpg");
bd=bdframe.add("Cylinder","Name":"BackDrop",
   "Position":point(0,0,(-0.55+0.04)*scfac),
   "Axis":vector(0,0,2*0.55*scfac),
   "Radius":scfac);
bd.renderback=true;
bd.resolution=70;
bd.IsClosed=false;
bd.enablelight=false;
bd.Texture="bd";
bd.BlendType=BlendTranslucent;

bd2frame=vframe.addsubframe("BackDropFrame");
bd2frame.transf.rotate(vector(0,0,1),-42/180*Pi+Pi/2);
bd2=bd2frame.add("Surface","Name":"BackDropKoepel");
bd2.enablelight=false;
bd2.renderback=true;
bd2.dim(2,2);
bd2.points(0,0)=@point(scfac*vecnorm(vector(+0.8,-1,0))+scfac*vector(0,0,-0.44));
bd2.points(1,0)=@point(scfac*vecnorm(vector(-0.15,-1,0))+scfac*vector(0,0,-0.44));
bd2.points(1,1)=@point(scfac*vecnorm(vector(-0.22,-1,0))+scfac*vector(0,0,+0.7));
bd2.points(0,1)=@point(scfac*vecnorm(vector(+0.9,-1,0))+scfac*vector(0,0,+0.7));
bd2.Texture="bd2";
bd2.BlendType=BlendTranslucent;


myviewport.CameraPos=point(0,0,0);
myviewport.Cameradir=vecnorm(vector(0.123060774, 0.95865996, 0.256567978));
myviewport.FocalDistance=0.1*scfac;
myviewport.NearClipPlane=0.1*scfac;
myviewport.Aperture=80/180*Pi;

#clock overlay
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
#time
clock=clockframe.add("Clock");
clock.enablelight=false;
clock.Position=point(0.085,0.085,0);
clock.Size=0.075;
clock.DepthTest=DepthTestDisable;
clock.DepthMask=DepthMaskDisable;
clock.BlendType=BlendTranslucent;
clock.color=color(0.7,0.7,0.7);
#date
clock=clockframe.add("Clock");
clock.enablelight=false;
clock.Position=point(0.15,0.01,0);
clock.Type=ClockTypeDateTime;
clock.Size=0.02;
clock.DepthTest=DepthTestDisable;
clock.DepthMask=DepthMaskDisable;
clock.BlendType=BlendTranslucent;
clock.color=color(0.7,0.7,0.7);


root.time=time(2011,6,1,3,20,0);
#root.time=CurrentTimeUT;

fr=0;

root.TimeSpeedFactor=0;
while true do {
   incrtime;
#root.time=CurrentTimeUT;
#   root.time=CurrentTimeUT+0*60*60;

   earthpos=GetPlanetPosition("Earth");
   moonpos=GetLunaPosition;
   
   #update positions of planet indicator
   for i=0 to planlist.size-1 do
      frame_planets.get(planlist(i)).transf.origin=earthpos+2*scfac*vecnorm(GetPlanetPosition(planlist(i))-earthpos);
   
   #update position of moon halo & calculate intensity
   frame_moonhalo.transf.origin=earthpos+1.5*scfac*vecnorm(@vector(moonpos));
   
   fr=vecnorm(@vector(moonpos))^vecnorm(@vector(earthpos));
   fr=1+fr;if (fr>1) then fr=1;
   fr=fr*fr;
   moonhalo.color=color(0.7,0.7,0.7,0.3*fr);
   
   
   
   #update position of sun halo
   ez=vecnorm(-1*@vector(earthpos));
   ex=vecnorm(ez*(vframe.totaltransformation*vector(0,0,1)));
   ey=ex*ez;
   frame_sunhalo.transf.origin=earthpos+2*scfac*ez;
   frame_sunhalo.transf.Xaxis=ex;
   frame_sunhalo.transf.Yaxis=ey;
   frame_sunhalo.transf.Zaxis=ez;
   
   #calculate sun presence
   vertdir=vframe.totaltransformation*vector(0,0,1);
   sunang=vecnorm(point(0,0,0)-earthpos)^vertdir;
   sunfr=(sunang+0.1)/0.2;
   if sunfr<0 then sunfr=0;
   if sunfr>1 then sunfr=1;
   
   #adjust atmosphere color to sun presence
   atm1.color=color(0.3,0.5,0.7,0.3+0.58*sunfr);
   atm2.color=color(0.3,0.5,0.7,0.15+0.73*sunfr);
   
   #adjust sunset/sunrise halo brightness
   fr=6*(sunang+0.1);fr=1-fr*fr;
   if fr<0 then fr=0;
   sunsethalo.color=color(1,0.5,0.0,0.6*fr);
   
   #backdrop illumunation
   fr2=0.15+0.8*sunfr;
   bd.color=color(fr2,fr2,fr2,1);
   bd2.color=color(fr2,fr2,fr2,1);
   

   

   #mechanism to co-move camera with earth
   if true then {
      root.SC.Universe.transf.reset;
      ttf=vframe.totaltransformation;
#      myviewport.CameraUpDir=ttf*vector(0,0,1);
      ttf.invert;
#      root.SC.Universe.transf=ttf;
#      root.sc.light0pos=ttf*point(0,0,0);
      root.viewports.main.transf=ttf;
   }
   
   render;
   
   if UIGetAxisActive(UIAxisX,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisX,UIAxisLevel0);
      myviewport.CameraDir=vecrotate(myviewport.CameraDir,vector(0,0,1),0.03*vl);
   }
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then {
      ok=false;
      ang0=vecangle(myviewport.CameraDir,vector(0,0,1));
      vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
      if vl>2 then vl=2;
      if vl<-2 then vl=-2;
      if (vl>0) and (ang0>5/180*Pi) then ok=true;
      if (vl<0) and (ang0<75/180*Pi) then ok=true;
      if ok then {
         dr1=vecnorm(myviewport.CameraDir*vector(0,0,1));
         myviewport.CameraDir=vecrotate(myviewport.CameraDir,dr1,0.03*vl);
      }
   }
   if UIGetAxisActive(UIAxisZ,UIAxisLevel0) then {
      vl=UIGetAxisPos(UIAxisZ,UIAxisLevel0);
      myviewport.Aperture=myviewport.Aperture*(1-vl/20);
      if myviewport.Aperture<30/180*Pi then myviewport.Aperture=30/180*Pi;
      if myviewport.Aperture>80/180*Pi then myviewport.Aperture=80/180*Pi;
   }
   
#   if IsKeyDown("F4") then stop;
}
