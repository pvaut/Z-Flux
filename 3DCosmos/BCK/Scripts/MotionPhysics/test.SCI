

codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

myviewport.focaldistance=7;

sss.ambientlightcolor=color(0.15,0.15,0.15);


rootframe=sss.Universe;

refframe=rootframe.addsubframe("refframe");

sp=refframe.add("surface");
if false then {
   sp.RenderBack=true;
   sp.canbuffer=true;
   fnc=functor("point(x,0.25*sin(25*x*y),y)","x","y");
   sp.generate(fnc,-1,1,300,-1,1,300);
   SaveObject(sp,"c:\temp\test.txt");
} else {
   ReadObject(sp,"c:\temp\test.txt");
}

sp.color=color(0.5,0.6,0.7);


incr=0.01;
root.time=time(2009,1,1,0,0,0);
root.TimeSpeed=incr;

while true do {
   incrtime;
   render;
}
