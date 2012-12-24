codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.3,0.3,0.3);
sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);
sss.Light0Pos=point(0,0,100);

refframe=sss.addsubframe("refframe");

sf1=sss.addsubframe("sf1");

bola=sf1.add("sphere");
bola.position=point(0,0,0);
bola.color=color(0.4,0.3,0.3,0.3);
bola.radius=0.5;
bola.resolution=20;
sf1.add("Arrow","Axis":vector(3,0,0),"NormAxis":vector(0,1,0),"Width":0.5,"Thickness":0.1);
bolb=sf1.add("sphere");
bolb.position=point(1.2,0,0);
bolb.color=color(0.3,0.3,0.3);
bolb.radius=0.5;
bolb.resolution=20;

cframe=refframe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);


rc1=cframe.add("FrameControl");
rc1.position=point(0.05,0.05);
rc1.SizeX=0.4;
rc1.SizeY=0.65;
rc1.color=color(0.2,0.2,0.2);

bt1=cframe.add("ButtonControl");
bt1.content="Stop button";
bt1.position=point(0.1,0.6);
bt1.sizex=0.3;

vl1=cframe.add("ScalarControl");
vl1.position=point(0.1,0.5);
vl1.SizeX=0.13;vl1.RangeSizeX=0.2;
vl1.min=-5;vl1.max=5;vl1.step=0.1;vl1.DecimalCount=2;
vl1.value=0;

ch1=cframe.add("CheckControl");
ch1.checked=true;
ch1.position=point(0.1,0.4);
txt1=cframe.add("TextControl");
txt1.Content='Show object';
txt1.Position=point(0.17,0.4);

t1=cframe.add("EditControl");
t1.position=point(0.1,0.3);
t1.sizex=0.3;
t1.content='Some text';

c1=cframe.add("ListControl");
c1.position=point(0.1,0.1);
c1.SizeX=0.3;
c1.Size=0.05;
c1.CountY=3;
c1.list.add('Red');
c1.list.add('Green');
c1.list.add('Blue');
c1.list.add('Yellow');
c1.list.add('Purple');
c1.list.add('White');
c1.wasmodified=true;

while true do {
   if c1.wasmodified then {
      cval=c1.selected;
      if cval=='Red' then bola.color=color(1,0,0);
      if cval=='Green' then bola.color=color(0,1,0);
      if cval=='Blue' then bola.color=color(0,0,1);
      if cval=='Yellow' then bola.color=color(1,1,0);
      if cval=='Purple' then bola.color=color(1,0,1);
      if cval=='White' then bola.color=color(1,1,1);
   }
   if ch1.wasmodified then bola.visible=ch1.checked;
   if vl1.wasmodified then bola.position=point(vl1.value,0,0);
   if bt1.wasmodified then stop;
   render;
   
}
