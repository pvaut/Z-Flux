#by S. Giehl 17-07-2011
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;
vp.enableusernavigation=true;
vp.EnableUserStop=true;


#Show list of panoramas
filelist=GetFileList(datadir+"\StereoPanorama\*.jpg");

menu=T_createmenu("Select stereo panorama",true);
menu.sizex=0.5;
root.SC.Universe.MenuFrame.Color=color(1,1,1,1);

vp.ShowControls=true;

listmap=Map;
foreach filename in filelist do {
   filename=filename;filename.Invert;filename.split(".");filename.Invert;
   filename=filename.split("_r");filename=filename.split("_l");
   if not(listmap.IsPresent(filename)) then {
      listmap.AddItem(filename:1);
      menu.Add("",filename,filename);
   }
}
T_menu_appendclose;

menufinished=false;
while not(menufinished) do {
   if menu.WasModified then {
      menufinished=true;
   }
   render;
}
filename=datadir+"\StereoPanorama\"+menu.SelectID;
if menu.SelectID=="[Close]" then stop;
T_delmenu;


#Set camera and viewport
vp.CameraPos=point(0,0,0);
vp.CameraDir=vector(0,1,0);
vp.cameraUpDir=vector(0,0,1);
vp.aperture=0.5;

vp.FocalDistance=1;
vp.NearClipPlane=20;
vp.FarClipPlane=100;
vp.EyeSeparation=0;
sss.ambientlightcolor=color(1,1,1);

frameleft=sf.addsubframe("FrameLeft");
bol1=frameleft.add("sphere");
bol1.radius=100;
bol1.resolution=100;
bol1.RenderBack=true;
bol1.RenderFront=False;
bol1.ShowRightEye=false;
TX1=frameleft.createtexture("P1",filename+"_r.jpg");
bol1.Texture=TX1.name;

frameright=sf.addsubframe("FrameRight");
bol2=frameright.add("sphere");
bol2.radius=100;
bol2.resolution=100;
bol2.RenderBack=true;
bol2.RenderFront=False;
bol2.ShowLeftEye=false;
TX2=frameright.createtexture("P2",filename+"_l.jpg");
bol2.Texture=TX2.name;


vp.enableusernavigation=false;


while true do {

  #Horizontal movement check
  vl=0;
   if UIGetAxisActive(UIAxisX,UIAxisLevel2) then vl=UIGetAxisPos(UIAxisX,UIAxisLevel2);
   if UIGetAxisActive(UIAxisX,UIAxisLevel0) then vl=UIGetAxisPos(UIAxisX,UIAxisLevel0);
   if UIIsKeyDown("SHIFT") then {
      #change parallax
      frameleft.transf.rotate(vector(0,0,1),-0.0002*vl);
      frameright.transf.rotate(vector(0,0,1),0.0002*vl);
   } else {
      sf.Transf.rotate(vector(0,0,1),+0.003*vl);
   }

  #Vertical movement check
  vl=0;
   if UIGetAxisActive(UIAxisY,UIAxisLevel2) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel2);
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
   if UIIsKeyDown("SHIFT") then {
      vp.aperture=vp.aperture-0.002*vl;
      if vp.aperture<0.3 then vp.aperture=0.3;
      if vp.aperture>0.8 then vp.aperture=0.8;
   } else {
      vp.cameradir.z=vp.cameradir.z+0.003*vl;
   }
      
   if UIGetAxisActive(UIAxisZ,UIAxisLevel2) then {
      vl=UIGetAxisPos(UIAxisZ,UIAxisLevel2);
      vp.aperture=vp.aperture-0.005*vl;
      if vp.aperture<0.3 then vp.aperture=0.3;
      if vp.aperture>0.8 then vp.aperture=0.8;
   }

   
   render;
}

