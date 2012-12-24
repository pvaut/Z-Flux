function animatemodel(objectpath,objectname,objectext,transfo,scalefactor,isimpleversion)
{
   simpleversion=false;
   if isvardefined("isimpleversion") then simpleversion=isimpleversion;

   sf=T_scene_create;
   sc=T_getscene;
   vp=T_getviewport;
   vp.nearclipplane=0.1;
   vp.farclipplane=50;
   
   if simpleversion then
      vp.UserNavigationAxisLevelRotate=2;
   
   sc.ambientlightcolor=color(0.1,0.1,0.1);
   ssys_createstarback;
   ssys_createmilkyway;
#   sc.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);
   
   sf1=sf.addsubframe("sf1");
   sf1.transf=transfo;
   sf1.cancache=false;
   sf1.color=color(0.7,0.7,0.7);
   sf1.LoadObject(objectpath,objectname+"."+objectext,objectext,scalefactor);
   
   dist0=distance(vp.camerapos,point(0,0,0));
   
   root.time=time(2008,1,1,0,0,0);
   root.TimeSpeedFactor=0.001;
   while true do {
      incrtime;
      vp.focaldistance=(2*dist0+distance(vp.camerapos,point(0,0,0)))/3;
      dr1=-1*vp.cameradir;
      dr2=vecnorm(dr1*vector(0,1,0));
      dr3=vecnorm(dr1*dr2);
      sc.light0pos=point(0,0,0)+50*(dr1+dr2-0.5*dr3);
      if RightMouseClicked then stop;
      render;
   }
}
