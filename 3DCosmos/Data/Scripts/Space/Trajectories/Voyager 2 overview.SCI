codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

au2km=149598000;


JID=0;

ssysframe=ssys_create;

showconstellations=true;
ssys_showconstellations(showconstellations);
root.SC.Universe.StarBackFrame.StarBack.linecolor=color(0.3,0.3,0.5,0.4);
ssys_createmilkyway;
ssys_createsun;


pttx=ssysframe.createtexture("point",DataDir+"\textures\star1.bmp");
function addlightpoint(frame,sz)
{
   sz=sz/2;
   rc=frame.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0));
   rc.Texture=pttx.name;
   rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   return(rc);
}


planetnamelist=ssys_getplanetlist;
foreach planetname in planetnamelist do {
   pframe=ssys_createplanet(planetname,1000);
   pframe.inclin.dispose;
   textframe=pframe.addviewdirframe(point(0,0,0));
#   addlightpoint(textframe,autokm(0.05)).color=color(0.3,0.3,1);
   addlightpoint(textframe,autokm(0.12)).color=color(0,1,1);
   addlightpoint(textframe,autokm(0.06)).color=color(1,1,1);
   txt=textframe.add("TextFlat","Position":point(autokm(-0.033),autokm(0.03),0),"Content":planetname,
      "Size":autokm(0.15),"Color":color(0.5,0.5,0.5));
}

voyagermt=MotionTable.create(ssysframe,"Voyager");
voyagermt.loadfile(ScriptFilePath+"\voyager2.pth");
voyager=ssysframe.addsubframe("Voyager");
voyager.MotionName=voyagermt.name;
textframe=voyager.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.12)).color=color(1,0.5,0);
addlightpoint(textframe,autokm(0.06)).color=color(1,1,0.5);
voyagerorbit=ssysframe.add("Curve","Color":color(1,0.5,0,0.6),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
voyagerorbit.size=2;
voyagerorbit.Track(voyager,0.01);




######################################################################################
# START VIEWPORT
######################################################################################

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.13);
myviewport.FarClipPlane=autokm(80);
myviewport.FocalDistance=autokm(10);
myviewport.enableusernavigation=true;

myviewport.camerapos=point(autokm(10),autokm(5),autokm(5));
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);

clockframe=ssysframe.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.01,0.01,0),"Size":0.04,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);

starttm=timejd(2443376.2);
curvedurat=15*365;

   
######################################################################################
# START GRAPH
######################################################################################

gsizex=myviewport.aspectratio;
gsizey=0.2;
goffsy=0.8;
goffsx=0.03;


gframe=ssysframe.addscreenframe("Graphics");
gframe.enablelight=false;
gframe.add("Rectangle","Position":point(0,1-gsizey-0.025,0),"Axis1":vector(gsizex,0,0),"Axis2":vector(0,gsizey+0.025,0),
   "Color":color(1,1,1,0.1),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   
# plot years
tm1=starttm;tm1.month=1;tm1.day=1;tm1.hour=0;tm1.min=0;tm1.sec=0;
for i=0 to 50 do {
   tm1.year=tm1.year+1;
   xps=goffsx+tm1.DiffDays(starttm)/curvedurat*gsizex;
   gframe.add("Curve","Color":color(0.5,0.5,0.5)).makeline(point(xps,1,0),point(xps,goffsy,0));
   st=str(tm1.year);
   txt=gframe.add("Text3D","Content":st,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Color":color(0.5,0.5,0.5));
   txt.Position=point(xps-0.5*0.02*txt.GetSizeX,goffsy-0.02,0);
}

# plot y scale
for i=0 to 10 do {
   spd=i*10/40;
   yps=goffsy+spd*gsizey;
   gframe.add("Curve","Color":color(0.5,0.5,0.5)).makeline(point(0,yps,0),point(gsizex,yps,0));
}   
crvdist=gframe.add("Curve","Color":color(1,0.5,0),"Size":2);


root.time=starttm;
tmlast=root.time;
#root.time=time(2001,9,1,0,0,0);
root.TimeSpeedFactor=5000000;
xpslast=-1;
while true do {
   incrtime;
   
   #keep sun in focal plane
   myviewport.FocalDistance=1.0*distance(myviewport.camerapos,point(0,0,0));

   posit=voyagermt.position;
   xps=goffsx+root.time.DiffDays(starttm)/curvedurat*gsizex;
   if (xps>xpslast+0.0002) and (xps<gsizex) then {
      spd=voyagermt.speed.size/40;
      crvdist.addpoint(point(xps,goffsy+spd*gsizey,0));
      xpslast=xps;
   }
   if root.time.year>tmlast.year then {
      ssysframe.add("Curve","Color":color(1,0.5,0),"Size":2).makeline(posit-au2km*vector(0,0,0.2),posit+au2km*vector(0,0,0.2));
      st=str(root.time.year).substring(2,3);
      textframe=ssysframe.addviewdirframe(posit-au2km*vector(0,0,0.4));
      txt=textframe.add("TextFlat","Position":point(autokm(-0.1),0,0),"Content":st,
         "Size":autokm(0.2),"Color":color(1,0.5,0));
      tmlast=root.time;
   }
   if RightMouseClicked then stop;
   render;
}