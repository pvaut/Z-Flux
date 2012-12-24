codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");


JID=0;

ssysframe=ssys_create;

ssys_showconstellations(false);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
ssys_createmilkyway;
ssys_createsun;

planetnamelist=ssys_getplanetlist;
foreach planetname in planetnamelist do ssys_createplanet(planetname,1000);


myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.13);
myviewport.FarClipPlane=autokm(200);
myviewport.FocalDistance=autokm(20);
myviewport.enableusernavigation=true;

myviewport.camerapos=point(autokm(10),autokm(5),autokm(5));
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);

myviewport.camerapos=point(-706736269.6979877, -143993368.38512406, 419738685.1908094);
myviewport.cameradir=vector(0.790813353, 0.421717813, -0.44358576);

#clockframe=ssysframe.addscreenframe("ClockFrame");
#info_clock=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.065,0.065,0),"Size":0.06,
#   "Color":color(0.7,0.7,0.7),"EnableLight":false);

#root.time=CurrentTimeUT;
root.time=time(2001,9,1,0,0,0);
root.TimeSpeedFactor=0;
while true do {
   incrtime;
   render;
}