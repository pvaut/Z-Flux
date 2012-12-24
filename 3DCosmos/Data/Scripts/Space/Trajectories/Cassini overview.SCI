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

craftmt=MotionTable.create(ssysframe,"craft");
craftmt.loadfile(ScriptFilePath+"\cassini.pth");
craftmt.InterpolType=InterpolLinear;
craft=ssysframe.addsubframe("craft");
craft.MotionName=craftmt.name;
textframe=craft.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.12)).color=color(1,0.5,0);
addlightpoint(textframe,autokm(0.06)).color=color(1,1,0.5);
craftorbit=ssysframe.add("Curve","Color":color(1,0.5,0,0.6),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
craftorbit.size=2;
craftorbit.Track(craft,0.01);




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

starttm=timejd(2450736);
curvedurat=7*365;

   


root.time=starttm;
tmlast=root.time;
#root.time=time(2001,9,1,0,0,0);
root.TimeSpeedFactor=2500000;
xpslast=-1;
while true do {
   incrtime;
   
   #keep sun in focal plane
   myviewport.FocalDistance=1.0*distance(myviewport.camerapos,point(0,0,0));

   posit=craftmt.position;
   if (root.time.month!=tmlast.month) and ((root.time.month==1) or (root.time.month==6)) then {
      ssysframe.add("Curve","Color":color(1,0.5,0),"Size":2).makeline(posit-au2km*vector(0,0,0.1),posit+au2km*vector(0,0,0.1));
      st=str(root.time.month)+"/"+str(root.time.year).substring(2,3);
      textframe=ssysframe.addviewdirframe(posit-au2km*vector(0,0,0.25));
      txt=textframe.add("TextFlat","Position":point(autokm(-0.13),0,0),"Content":st,
         "Size":autokm(0.15),"Color":color(1,0.5,0));
      tmlast=root.time;
   }
   if RightMouseClicked then stop;
   render;
}