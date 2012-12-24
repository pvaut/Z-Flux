

lst=GetFileList(ScriptDir+"\*.*",true);

codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.3,0.3,0.3);
sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

sss.Light0Pos=point(0,0,100);

refframe=sss.addsubframe("refframe");

refframe.add("Fog","Type":FogExponential,"Density":0.02,"Color":color(0,0,1));


sf1=sss.addsubframe("sf1");
sf1.motion=motionrotate.create(sf1);
sf1.motion.normdir=vector(0,1,0);
sf1.motion.rotspeed=0.000001;
sf1.cancache=false;
sf1.color=color(0.8,0.8,0.8);

cl=sf1.add("Clock",
   "position":point(0,2,0),
   "size":1,
   "Enablelight":false,
   "RenderBack":true,
   "TimeShift":(-2*60*60),
   "Type":ClockTypeCalendar
   );
cl.Monthnames.clear;
cl.Monthnames.add("Januari");
cl.Monthnames.add("Februari");
cl.Monthnames.add("Maart");
cl.Monthnames.add("April");
cl.Monthnames.add("Mei");
cl.Monthnames.add("Juni");
cl.Monthnames.add("Juli");
cl.Monthnames.add("Augustus");
cl.Monthnames.add("September");
cl.Monthnames.add("Oktober");
cl.Monthnames.add("November");
cl.Monthnames.add("December");

createvar(bola);
if true then {
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
}

cframe=refframe.addscreenframe("Controls Frame");
cframe.enablelight=false;
#cframe.DepthTest=DepthTestDisable;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
#cframe.DepthTest=DepthTestDisable;
cframe.color=color(1,1,1,0.5);

txt=cframe.add("TextFlat");
txt.Position=point(0.01,0.9,0);
txt.Content="Test";
txt.Size=0.2;

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
txt1.Content="Show object";
txt1.Position=point(0.17,0.4);

t1=cframe.add("EditControl");
t1.position=point(0.1,0.3);
t1.sizex=0.3;
t1.content="Some text";

c1=cframe.add("ListControl");
c1.position=point(0.1,0.1);
c1.SizeX=0.3;
c1.Size=0.05;
c1.CountY=3;
c1.list.add("Red");
c1.list.add("Green");
c1.list.add("Blue");
c1.list.add("Yellow");
c1.list.add("Purple");
c1.list.add("White");
c1.wasmodified=true;

root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=2;

#root.showcontrols=false;

st="";

while true do {
   incrtime;
   if c1.wasmodified then {
      cval=c1.selected;
      if cval=="Red" then bola.color=color(1,0,0);
      if cval=="Green" then bola.color=color(0,1,0);
      if cval=="Blue" then bola.color=color(0,0,1);
      if cval=="Yellow" then bola.color=color(1,1,0);
      if cval=="Purple" then bola.color=color(1,0,1);
      if cval=="White" then bola.color=color(1,1,1);
   }
   if ch1.wasmodified then bola.visible=ch1.checked;
   if vl1.wasmodified then bola.position=point(vl1.value,0,0);
   if bt1.wasmodified then stop;
   render;
   
}
