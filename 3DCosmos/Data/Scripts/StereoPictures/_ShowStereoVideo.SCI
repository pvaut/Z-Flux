
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

filename=ScriptArgument;

T_scene_create;

sss=T_getscene;
vp=T_getviewport;
sf=T_getrootframe;

vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;
vp.enableusernavigation=false;
vp.EnableUserStop=true;


soundfilename=filename+".mp3";


screensizey=2*tan(vp.aperture/2);
screensizex=screensizey*vp.aspectratio;


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


rc=sf.add("Rectangle","Name":"VidREC","Axis1":vector(sizex,0,0),"Axis2":vector(0,sizey,0),
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

rc.dispose;
   