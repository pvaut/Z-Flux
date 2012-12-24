
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;
vp.enableusernavigation=false;
vp.EnableUserStop=true;


#Show list of videos
filelist=GetFileList(datadir+"\StereoVideos\*.AVI");

menu=T_createmenu("Select stereo video",true);
menu.sizex=0.5;
root.SC.Universe.MenuFrame.Color=color(1,1,1,1);

vp.ShowControls=true;

listmap=Map;
foreach filename in filelist do {
   filename=filename;filename.Invert;filename.split(".");filename.Invert;
   filename=filename.split("_R");filename=filename.split("_L");
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
if menu.SelectID=="[Close]" then stop;
filename=datadir+"\StereoVideos\"+menu.SelectID;
T_delmenu;

soundfilename=filename+".mp3";


screensizey=2*tan(vp.aperture/2);
screensizex=screensizey*vp.aspectratio;


sss.ambientlightcolor=color(0.3,0.3,0.3);

vdleft=addvideo(filename+"_L.avi");
vdright=addvideo(filename+"_R.avi");

vdxres=vdleft.GetXRes;
vdyres=vdleft.GetYRes;
vdframerate=round(vdleft.GetFrameRate);
vdframecount=vdleft.GetFrameCount;

frx=vp.aspectratio/vdxres;
fry=1/vdyres;
fr=min(frx,fry);
fr=fr*screensizey;


sizex=fr*vdxres;
sizey=fr*vdyres;


rc=sf.add("Rectangle","Name":"REC","Axis1":vector(sizex,0,0),"Axis2":vector(0,sizey,0),
      "position":point(0-0.5*sizex,0-0.5*sizey,0),"EnableLight":false);

txl=sf.CreateVideoTexture("left",vdleft);
txr=sf.CreateVideoTexture("right",vdright);
rc.Texture="left";
rc.TextureRight="right";


oldframerate=root.FrameRate;
root.FrameRate=vdframerate;

#Fade in
for i=0 to 30 do {
   vp.FadeColor=color(0,0,0,1-i/30);
   render;
}

#Start sound file
if FileIsPresent(soundfilename) then {
   snd=addsound(soundfilename);
   snd.Start;
}

#Play video
frnr=0;
while frnr<vdframecount do {
   incrtime;
   vdleft.CurrentFrame=frnr;
   vdright.CurrentFrame=frnr;
   frnr=frnr+1;
   render;
}

#fade out
for i=0 to 30 do {
   vp.FadeColor=color(0,0,0,i/30);
   render;
}

root.FrameRate=oldframerate;
   