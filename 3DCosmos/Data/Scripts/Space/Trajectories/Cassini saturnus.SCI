
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\Astronomy\_SUB\LoadPlanetMoonsKepler.sci");



planetname="Saturnus";
startdist=10000000;

ssysframe=ssys_create;

sc=T_getscene;
#sc.VolumeShadowAdd(0,color(0,0,0,0.5),1000,1000*300);
sc.ambientlightcolor=color(0.02,0.02,0.02);


planetframe=ssys_createplanet(planetname,1);

GetPlanetOrbit(planetname).dispose;

LoadPlanetMoonsKepler(planetframe,datadir+"\AstroData\"+planetname+"_moons.txt");

vp=T_getviewport;
vp.cameradir=vecnorm(vector(0,1,-1));
vp.camerapos=point(0,0,0)-startdist*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;
vp.EnableUserTimeControl=true;
vp.NearClipPlane=0.1*startdist;
vp.FarClipPlane=100*startdist;
vp.FocalDistance=startdist;

menu=T_createmenu;
menu.SizeX=0.28;
menu.add("",'Show moons',"ShowMoons",true);
menu.add("",'Show moon orbits',"ShowMoonOrbits",true);
menu.add("",'Show moon names',"ShowMoonNames",true);

planetframe.Inclin.MoonsFrame.visible=false;
planetframe.Inclin.MoonOrbitsFrame.visible=false;

#create enhanced saturnus path
planetframe.motion=MotionTable.create(planetframe,"SaturnusEnhanced");
planetframe.motion.loadfile(datadir+"\paths\saturnus_sat.pth");

#create cassini trajectory
tm1=time(2004,6,1);
tm2=time(2010,1,1);

tm1=time(2005,1,1);
tm2=time(2006,1,1);

cassinimt=MotionTable.create(ssysframe,"Cassini");
#cassinimt.loadfile(ScriptFilePath+"\cassini.pth");
cassinimt.loadfile(datadir+"\paths\cassini_sat.pth");

cassiniframe=ssysframe.addsubframe("CassiniFrame");
cassiniframe.motion=cassinimt;
cassiniframe.add("sphere","radius":30000,"color":color(1,0.5,0));

#crv.track(cassiniframe,100);

#create tracked path
fle=datadir+"\temp\cassinipath.flf";
crv=planetframe.add("curve","size":2,"color":color(0.2,1,0.2,0.3),"BlendType":BlendTransparent,"DepthMask":DepthMaskDisable);
if FileIsPresent(fle) then {
   readobject(crv,fle);
} else {
   root.time=time(2004,6,1,0,0,0);
   root.TimeSpeedfactor=1000000;
   incr=0;elapstm=0;
   tottm=24*60*60*365*3.5;
   while elapstm<tottm do {
      root.time=root.time+incr;
      casspt=cassiniframe.transf.origin;
      satpt=planetframe.transf.origin;
      reldr=casspt-satpt;
      crv.addpoint(@point(reldr));
      incr=0.02*reldr.size;
      elapstm=elapstm+incr;
   }
   saveobject(crv,fle);
}

#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeDateTime,"Position":point(0.02,0.02,0),"Size":0.03);

root.time=time(2005,1,1,0,0,0);
root.TimeSpeedfactor=100000;
while true do {
   incrtime;

   #mechanism to co-move camera with planet
   root.SC.Universe.transf.reset;
   ttf=planetframe.totaltransformation;
   ttf.invert;
   root.sc.universe.transf=ttf;
   root.sc.light0pos=ttf*point(0,0,0);
   
   #keep planet in focal plane
   vp.FocalDistance=1.0*distance(vp.camerapos,point(0,0,0));
   
   if menu.WasModified and (menu.SelectID=="ShowMoons") then
      planetframe.Inclin.MoonsFrame.visible=menu.Checked("ShowMoons");
   if menu.WasModified and (menu.SelectID=="ShowMoonOrbits") then
      planetframe.Inclin.MoonOrbitsFrame.visible=menu.Checked("ShowMoonOrbits");
   if menu.WasModified and (menu.SelectID=="ShowMoonNames") then
   {
      menu.Checked("ShowMoons")=true;
      planetframe.Inclin.MoonsFrame.visible=true;
      foreach moon in planetframe.Inclin.MoonsFrame.getmembers do
         moon.label.visible=menu.Checked("ShowMoonNames");
   }

   if RightMouseClicked then stop;
   render;
}


