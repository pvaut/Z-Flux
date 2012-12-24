codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

T_scene_create;
sc=T_getscene;
myviewport=T_getviewport;
myviewport.CameraPos=point(0,0,1);
myviewport.CameraDir=vector(0,0,-1);
myviewport.FocalDistance=1;

refframe=sc.addsubframe("refframe");

cframe=refframe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);


px=0.05;py=0.9;
px2=px+0.4;

textsize=0.03;

root.mousedampingfactor=0.25;
cframe.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,1),0.03);


cframe.add("TextControl","Content":'Stereo pictures folder:',"Size":textsize,"Position":point(px,py));
stereofolder=cframe.add("EditControl","Size":textsize,"SizeX":0.6);
stereofolder.position=point(px,py-0.05);
stereofolder.content=ReadSetting("StereoFolder","");

py=py-0.15;

cframe.add("TextControl","Content":'Rows in browser:',"Size":textsize,"Position":point(px,py));
rows=cframe.add("ScalarControl","Size":textsize,"SizeX":0.1,"RangeSizeX":0.2);
rows.min=2;
rows.max=6;
rows.step=1;
rows.position=point(px+0.2,py);
rows.value=ReadSetting("StereoBrowserRows",3);

py=py-0.2;

btok=cframe.add("ButtonControl","Content":'Apply',"Position":point(px,py),"Size":textsize,"SizeX":0.2);
btcancel=cframe.add("ButtonControl","Content":'Cancel',"Position":point(px2,py),"Size":textsize,"SizeX":0.2);

finished=false;
savesettings=false;
while not(finished) do {
   if btcancel.wasmodified then finished=true;
   if btok.wasmodified then {
      savesettings=true;
      finished=true;
   }
   render;
}

if savesettings then {
   WriteSetting("StereoFolder",stereofolder.content);
   WriteSetting("StereoBrowserRows",rows.value);
}
