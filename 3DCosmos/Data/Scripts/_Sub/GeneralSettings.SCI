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

root.mousedampingfactor=0.25;
cframe.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,1),0.03);


px=0.07;py=0.9;
px2=px+0.4;

textsize=0.03;

showstereo=cframe.add("CheckControl","Size":textsize);
showstereo.checked=ReadSetting("ShowStereo",false);
showstereo.position=point(px,py);
cframe.add("TextControl","Content":'Show in stereo',"Size":textsize,"Position":point(px+1.5*textsize,py));

swapleftright=cframe.add("CheckControl","Size":textsize);
swapleftright.checked=ReadSetting("SwapLeftRight",false);
swapleftright.position=point(px2,py);
cframe.add("TextControl","Content":'Swap left and right',"Size":textsize,"Position":point(px2+1.5*textsize,py));



py=py-2.5*textsize;

mirrorleftH=cframe.add("CheckControl","Size":textsize);
mirrorleftH.checked=ReadSetting("MirrorLeftH",false);
mirrorleftH.position=point(px,py);
cframe.add("TextControl","Content":'Left mirror horizontal',"Size":textsize,"Position":point(px+1.5*textsize,py));

mirrorrightH=cframe.add("CheckControl","Size":textsize);
mirrorrightH.checked=ReadSetting("MirrorRightH",false);
mirrorrightH.position=point(px2,py);
cframe.add("TextControl","Content":'Right mirror horizontal',"Size":textsize,"Position":point(px2+1.5*textsize,py));

py=py-1.5*textsize;

mirrorleftV=cframe.add("CheckControl","Size":textsize);
mirrorleftV.checked=ReadSetting("MirrorLeftV",false);
mirrorleftV.position=point(px,py);
cframe.add("TextControl","Content":'Left mirror vertical',"Size":textsize,"Position":point(px+1.5*textsize,py));

mirrorrightV=cframe.add("CheckControl","Size":textsize);
mirrorrightV.checked=ReadSetting("MirrorRightV",false);
mirrorrightV.position=point(px2,py);
cframe.add("TextControl","Content":'Right mirror vertical',"Size":textsize,"Position":point(px2+1.5*textsize,py));

py=py-1.5*textsize;

cframe.add("TextControl","Content":'Horizontal Stretch:',"Size":textsize,"Position":point(px,py));
stretchfactor=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
stretchfactor.position=point(px+0.25,py);
stretchfactor.content=ReadSetting("HorizontalStretch","1.000");

py=py-1.5*textsize;

cframe.add("TextControl","Content":'Eye separation factor:',"Size":textsize,"Position":point(px,py));
eyesepfactor=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
eyesepfactor.position=point(px+0.25,py);
eyesepfactor.content=ReadSetting("EyeSepFactor","0.025");


py=py-2.5*textsize;

cframe.add("TextControl","Content":'Display',"Size":(1.2*textsize),"Position":point(px,py));
py=py-1.5*textsize;

cframe.add("TextControl","Content":'Name:',"Size":textsize,"Position":point(px,py));
dispname=cframe.add("EditControl","Size":textsize,"SizeX":0.25);
dispname.position=point(px+0.15,py);
dispname.content=ReadSetting("DisplayName","");

fullscreen=cframe.add("CheckControl","Size":textsize);
fullscreen.checked=ReadSetting("FullScreen",false);
fullscreen.position=point(px2+0.1,py);
cframe.add("TextControl","Content":'Full screen',"Size":textsize,"Position":point(px2+0.1+1.5*textsize,py));


py=py-1.5*textsize;

cframe.add("TextControl","Content":'Offset X:',"Size":textsize,"Position":point(px,py));
viewportx0=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
viewportx0.position=point(px+0.15,py);
viewportx0.content=ReadSetting("WinOffsetX","");


cframe.add("TextControl","Content":'Size X:',"Size":textsize,"Position":point(px2,py));
viewportlx=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
viewportlx.position=point(px2+0.15,py);
viewportlx.content=ReadSetting("WinSizeX","");

py=py-1.5*textsize;

cframe.add("TextControl","Content":'Offset Y:',"Size":textsize,"Position":point(px,py));
viewporty0=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
viewporty0.position=point(px+0.15,py);
viewporty0.content=ReadSetting("WinOffsetY","");


cframe.add("TextControl","Content":'Size Y:',"Size":textsize,"Position":point(px2,py));
viewportly=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
viewportly.position=point(px2+0.15,py);
viewportly.content=ReadSetting("WinSizeY","");






py=py-2.5*textsize;

cframe.add("TextControl","Content":'Frame rate:',"Size":textsize,"Position":point(px,py));
framerate=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
framerate.position=point(px+0.15,py);
framerate.content=ReadSetting("FrameRate","");

cframe.add("TextControl","Content":'Sync factor:',"Size":textsize,"Position":point(px2,py));
syncfactor=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
syncfactor.position=point(px2+0.15,py);
syncfactor.content=ReadSetting("SyncFactor","0");

py=py-2.5*textsize;

cframe.add("TextControl","Content":'Lattitude:',"Size":textsize,"Position":point(px,py));
lattitude=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
lattitude.position=point(px+0.15,py);
lattitude.content=ReadSetting("Lattitude","51");

cframe.add("TextControl","Content":'Longitude:',"Size":textsize,"Position":point(px2,py));
longitude=cframe.add("EditControl","Size":textsize,"SizeX":0.15);
longitude.position=point(px2+0.15,py);
longitude.content=ReadSetting("Longitude","0");

py=py-2.5*textsize;


cframe.add("TextControl","Content":'Language:',"Size":textsize,"Position":point(px,py));
language=cframe.add("ListControl","Size":textsize,"SizeX":0.3,"CountY":1);
language.position=point(px+0.15,py);
llist=GetLanguageList;
language.List=llist;
actlanguage=ReadSetting("Language","English");
for i=0 to llist.size-1 do
   if llist(i)==actlanguage then language.SelectIdx=i;


py=py-3*textsize;

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
   WriteSetting("ShowStereo",showstereo.Checked);
   WriteSetting("SwapLeftRight",swapleftright.Checked);

   WriteSetting("MirrorLeftH",mirrorleftH.Checked);
   WriteSetting("MirrorLeftV",mirrorleftV.Checked);
   WriteSetting("MirrorRightH",mirrorrightH.Checked);
   WriteSetting("MirrorRightV",mirrorrightV.Checked);
   
   WriteSetting("HorizontalStretch",stretchfactor.content);
   WriteSetting("EyeSepFactor",eyesepfactor.content);

   WriteSetting("DisplayName",dispname.content);
   WriteSetting("FullScreen",fullscreen.checked);

   WriteSetting("WinOffsetX",viewportx0.content);
   WriteSetting("WinOffsetY",viewporty0.content);
   WriteSetting("WinSizeX",viewportlx.content);
   WriteSetting("WinSizeY",viewportly.content);

   WriteSetting("FrameRate",framerate.content);
   WriteSetting("SyncFactor",syncfactor.content);

   WriteSetting("Longitude",longitude.content);
   WriteSetting("Lattitude",lattitude.content);

   WriteSetting("Language",language.Selected);

   #message('Please restart the software now');
   Execute(ExecDir+"\z-flux.exe",execdir,false);
   ExitProgram;
}
