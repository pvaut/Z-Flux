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


cframe.add("TextControl","Content":'Left image color matrix:',"Size":textsize,"Position":point(px,py));
py=py-2*textsize;

leftcontrols=list;
for i1=0 to 2 do {
   leftcontrols.add(list);
   for i2=0 to 2 do {
      ctrl=cframe.add("EditControl","Size":textsize,"SizeX":0.07);
      ctrl.position=point(px+0.1*i2,py);
      defval=0;if i1==i2 then defval=1;
      ctrl.content=str(ReadSetting("ColorMatrixLeft"+str(i1)+str(i2),defval));
      leftcontrols(i1).add(ctrl);
   }
   py=py-1.5*textsize;
}

py=py-2*textsize;
cframe.add("TextControl","Content":'Right image color matrix:',"Size":textsize,"Position":point(px,py));
py=py-2*textsize;

rightcontrols=list;
for i1=0 to 2 do {
   rightcontrols.add(list);
   for i2=0 to 2 do {
      ctrl=cframe.add("EditControl","Size":textsize,"SizeX":0.07);
      ctrl.position=point(px+0.1*i2,py);
      defval=0;if i1==i2 then defval=1;
      ctrl.content=str(ReadSetting("ColorMatrixRight"+str(i1)+str(i2),defval));
      rightcontrols(i1).add(ctrl);
   }
   py=py-1.5*textsize;
}


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

   for i1=0 to 2 do {
      for i2=0 to 2 do {
         vl=leftcontrols.get(i1).get(i2).content;
         WriteSetting("ColorMatrixLeft"+str(i1)+str(i2),vl);
      }
   }

   for i1=0 to 2 do {
      for i2=0 to 2 do {
         vl=rightcontrols.get(i1).get(i2).content;
         WriteSetting("ColorMatrixRight"+str(i1)+str(i2),vl);
      }
   }


   #message('Please restart the software now');
   Execute(ExecDir+"\z-flux.exe",execdir,false);
   ExitProgram;
}
