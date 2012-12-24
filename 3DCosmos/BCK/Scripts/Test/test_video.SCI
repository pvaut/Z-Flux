codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

#myviewport.usestereo=false;
#myviewport.start;

sss.ambientlightcolor=color(0.05,0.05,0.05);


vd=addvideo(DataDir+"\textures\Dewinne2.avi");
#vd=addvideo("C:\temp\test.avi");
vd.name="DeWinne";
vd.playing=true;

vd.FrameReduction=3;

framecount=vd.GetFrameCount;
framerate=vd.GetFrameRate;
xres=vd.GetXRes;
yres=vd.GetYRes;

#break;


rootframe=sss.Universe.addsubframe("Root");
rootframe.add("sphere","radius":0.1);
rootframe.add("sphere","radius":0.1,"position":point(1,0,0),"color":color(1,0,0));
rootframe.add("sphere","radius":0.1,"position":point(0,1,0),"color":color(0,1,0));

tx=rootframe.createtexture("star",DataDir+"\textures\star2.bmp");

vtx=rootframe.CreateVideoTexture("vtx",vd);

s=rootframe.add("bar","position":point(0,0,1),"color":color(1,1,1));
#s.BlendType=BlendTranslucent;
#s.DepthMask=DepthMaskDisable;
#s.texture=vtx.name;
s.texture=vtx.name;

#myviewport.StartRecording("c:\temp\test.avi");
ct=0;
while ct<30000 do {
   ct=ct+1;
   render;
}

#myviewport.StopRecording;