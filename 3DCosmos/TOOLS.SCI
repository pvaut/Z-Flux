function T_scene_create()
{
#   resetall;
   resetallscenes;
   resetallviewports;
   
   displayname=root.displays.getmembers.get(0).name;
   
   vp=addviewport(0,0,1,1,displayname,displayname);
   vp.name="main";
   vp.start;
   
   vp.camerapos=point(0,6,10);
   vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
   vp.cameraupdir=vector(0,1,0);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.NearClipPlane=0.05;
   vp.FarClipPlane=30;
   vp.FocalDistance=10;
   vp.EyeSeparation=0.5;
   vp.FrameSize=0;
   sc=addscene;sc.name="SC";
   sc.light0pos=point(50,50,100);
   vp.setscene(sc);
   scene.ambientlightcolor=color(0.1,0.1,0.1);
   scene.start;
   universe=sc.addsubframe("Universe");
   
   root.time=time(2008,1,1,0,0,0);
   root.TimeSpeed=1;
   
   return(root.SC.Universe);
}

function T_getrootframe()
{
   return(root.SC.Universe);
}

function T_getscene()
{
   return(root.SC);
}

function T_getviewport()
{
   return(root.Viewports.main);
}

function T_createdialogframe()
{
   ldialogframe=T_getrootframe.addscreenframe("DialogFrame");
   ldialogframe.enablelight=false;
   ldialogframe.BlendType=BlendTranslucent;
   ldialogframe.DepthMask=DepthMaskDisable;
   ldialogframe.color=color(1,1,1,0.5);
   return(ldialogframe);
}

function T_createmenu()
{
   lmenuframe=sss.universe.addscreenframe("MenuFrame");
   lmenuframe.enablelight=false;
   lmenuframe.BlendType=BlendTranslucent;
   lmenuframe.DepthMask=DepthMaskDisable;
   lmenuframe.color=color(1,1,1,0.5);
   lmenu=lmenuframe.add("MenuControl","Name":"Menu","Position":point(0.01,0.99),"Size":0.027,"SizeX":0.2);
   return(lmenu);
}
