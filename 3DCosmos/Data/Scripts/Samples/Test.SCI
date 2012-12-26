
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,10);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.3,0.3,0.3);


bmp=loadbitmap("D:\SourceCode\Z-Flux\3DCosmos\Data\StereoPictures\Mars\PhlegethonCatena_CTX_Anaglyph.jpg");
imageframe=sf.addsubframe("ImageFrame");

bmp1=bmp.ConvertGray(1);
bmp2=bmp.ConvertGray(3);

tx1=imageframe.CreateBitmapTexture("tx1",bmp1);
tx2=imageframe.CreateBitmapTexture("tx2",bmp1);


rc=imageframe.add("Rectangle",
    "Position":point(0,0,0),
    "Axis1":vector(6,0,0),
    "Axis2":vector(0,3,0),
    "Color":color(1.0,0.7,0.5));
rc.Texture=tx1.name;
rc.TextureRight=tx2.name;


while true do {
   render;
}
   