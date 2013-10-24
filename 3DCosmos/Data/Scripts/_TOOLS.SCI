

function WaitReleaseAll()
{
   while UIIsLeftMouseButtonDown or UIIsRightMouseButtonDown or (abs(NavigationMove.z)>0.004) or
         UIIsKeyDown("return") or UIIsKeyDown("escape") do
      delay(0.05);
}

function LeftMouseClicked()
{
   if not(UIIsLeftMouseButtonDown) then return(false);
   while UIIsLeftMouseButtonDown do delay(0.05);
   return(true);
}

function RightMouseClicked()
{
   if not(UIIsRightMouseButtonDown) then return(false);
   while UIIsRightMouseButtonDown do delay(0.05);
   return(true);
}

function navigatorpressing()
{
   return(NavigationMove.z>0.014);

}

function navigatorpulling()
{
   return(NavigationMove.z<-0.005);
}




function T_scene_create_named(scenename)
{
   #resetallscenes;
  lst=root.getmembers;
   foreach member in lst do {
      if (typeof(member)=="Scene") and ((member.Name!="PSC") or (member.name==scenename)) then
         DelObject(member);
   }

   resetallvideos;
   
   
   vp=root.Viewports.main;
   
   vp.Framesize=0;

# !!! moet hier staan: ?
#   vp.XMinFrac=0;vp.XMaxFrac=1;
#   vp.YMinFrac=0;vp.YMaxFrac=1;
   vp.camerapos=point(0,6,10);
   vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
   vp.cameraupdir=vector(0,1,0);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.EnableUserStop=true;
   vp.NearClipPlane=0.05;
   vp.FarClipPlane=30;
   vp.FocalDistance=10;
   vp.Aperture=0.785398175;
   vp.FadeColor=color(0,0,0,0);
   vp.erasebackground=false;
   vp.showcontrols=true;
   vp.Transf.reset;
   
   sc=addscene;sc.name=scenename;
   sc.light0pos=point(50,50,100);
   vp.setscene(sc);
   sc.ambientlightcolor=color(0.1,0.1,0.1);
   sc.start;
   universe=sc.addsubframe("Universe");
   
   root.time=time(2008,1,1,0,0,0);
   root.TimeSpeedFactor=1;
   root.pauzed=false;
   root.showcontrols=true;
   root.mousedampingfactor=0.85;
   
   return(Universe);
}

function T_scene_create()
{
   return(T_scene_create_named("SC"));
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

function T_createmenu(title,usemouse)
{
   lmenuframe=T_getrootframe.addscreenframe("MenuFrame");
   lmenuframe.enablelight=false;
   lmenuframe.BlendType=BlendTranslucent;
   lmenuframe.DepthMask=DepthMaskDisable;
   lmenuframe.DepthTest=DepthTestDisable;
   lmenuframe.color=color(1,1,1,0.75);
   lmenu=lmenuframe.add("MenuControl","Name":"Menu","Position":point(0.01,0.99),"Size":0.027,"SizeX":0.2);
   if isvardefined("title") then {
      lmenu.position=point(0.1,0.9);
      txt=lmenuframe.add("TextControl","Name":"Title","Position":point(0.1,0.92),"Size":0.05,"Content":title);
      txt.color=color(0.8,0.8,0.8);
      activatecontrol(lmenu);
   }
   if isvardefined("usemouse") then if usemouse then {
      root.mousedampingfactor=0.25;
      root.SC.Universe.MenuFrame.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1.3,1),0.03);
   }
   return(lmenu);
}

function T_delmenu()
{
   if root.SC.Universe.MenuFrame.Menu.SelectID=="[Close]" then stop;
   root.SC.Universe.MenuFrame.dispose;
}


function T_getmenu()
{
   return(root.SC.Universe.MenuFrame.Menu);
}

function T_menu_appendclose()
{
   root.SC.Universe.MenuFrame.Menu.Add("","","-");
   root.SC.Universe.MenuFrame.Menu.Add("",'[Close]',"[Close]");
}

function wait(secs)
{
   chr=chrono;
   while chr.elapsed<secs do {
      render;
   }
}


function fadeout(secs)
{
   chr=chrono;
   while chr.elapsed<secs do {
      root.Viewports.main.fadecolor=color(0,0,0,chr.elapsed/secs);
      render;
   }
   root.Viewports.main.fadecolor=color(0,0,0,1);
   render;
}
function fadein(secs)
{
   chr=chrono;
   while chr.elapsed<secs do {
      root.Viewports.main.fadecolor=color(0,0,0,1-chr.elapsed/secs);
      render;
   }
   root.Viewports.main.fadecolor=color(0,0,0,0);
   render;
}


function CreateNewViewPort(x0,y0,x1,y1)
{
   displayname=ReadSetting("DisplayName","");
   if displayname=="" then displayname="\\.\DISPLAY1";
   myviewport=addviewport(x0,y0,x1,y1,displayname,displayname);
   myviewport.Usestereo=root.Viewports.main.Usestereo;
   myviewport.SwapStereo=root.Viewports.main.SwapStereo;
   myviewport.StretchFactor=root.Viewports.main.StretchFactor;
   myviewport.HMirrorLeft=root.Viewports.main.HMirrorLeft;
   myviewport.HMirrorRight=root.Viewports.main.HMirrorRight;
   myviewport.VMirrorLeft=root.Viewports.main.VMirrorLeft;
   myviewport.VMirrorRight=root.Viewports.main.VMirrorRight;
   return(myviewport);
}



function hiderendermessage()
{
   root.SC.Universe.Drop("MessageFrame");
}

function rendermessage(line1,line2)
{
   root.SC.Universe.Drop("MessageFrame");
   msgframe=root.SC.Universe.addscreenframe("MessageFrame");
   msgframe.add("Text3D","Name":"MessageBox","Position":point(0.07,0.4,0),"Content":line1,
                "enablelight":false,"color":color(0.7,0.7,0.7),
                "UnitX":vector(0.03,0,0),"UnitY":vector(0,0.03,0));
   if isvardefined("line2") then
      msgframe.add("Text3D","Position":point(0.07,0.35,0),"Content":line2,
                "enablelight":false,"color":color(0.7,0.7,0.7),
                "UnitX":vector(0.03,0,0),"UnitY":vector(0,0.03,0));
   render;
}

function renderbasis(frame,sf)
{
   fcol=color(0.7,0.7,0.7);
   
   frame.add("Curve","EnableLight":false,"Color":fcol,"Size":2,"Arrow2Size":(sf/10)).makeline(point(0,0,0),point(sf,0,0));
   txf=frame.addviewdirframe(point(1.15*sf,0,0));
   txf.add("Text3D","EnableLight":false,"Color":fcol,
        "UnitX":vector(sf/10,0,0),"UnitY":Vector(0,sf/10,0),"Content":"X");

   frame.add("Curve","EnableLight":false,"Color":fcol,"Size":2,"Arrow2Size":(sf/10)).makeline(point(0,0,0),point(0,sf,0));
   txf=frame.addviewdirframe(point(0,1.15*sf,0));
   txf.add("Text3D","EnableLight":false,"Color":fcol,
        "UnitX":vector(sf/10,0,0),"UnitY":Vector(0,sf/10,0),"Content":"Y");

   frame.add("Curve","EnableLight":false,"Color":fcol,"Size":2,"Arrow2Size":(sf/10)).makeline(point(0,0,0),point(0,0,sf));
   txf=frame.addviewdirframe(point(0,0,1.15*sf));
   txf.add("Text3D","EnableLight":false,"Color":fcol,
        "UnitX":vector(sf/10,0,0),"UnitY":Vector(0,sf/10,0),"Content":"Z");
}

