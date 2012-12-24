codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.15,0.15,0.15);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.02);
rootframe.add("sphere","radius":0.02,"position":point(1,0,0),"color":color(1,0,0));
rootframe.add("sphere","radius":0.02,"position":point(0,1,0),"color":color(0,1,0));
rootframe.add("sphere","radius":0.02,"position":point(0,0,1),"color":color(0,0,1));

surf=rootframe.add("Surface","color":color(0.7,0.7,0.6));

p=0;
if false then {
   surf.renderback=true;
   surf.SpecularValue=30;
   surf.SpecularColor=color(0.35,0.35,0.35);
   
   ctt=20;
   for i=0 to ctt do {
      p=0.04*i;
      
      fnc=functor("radial2point(0.2*u,u+p*v,v+0.5*p*u)","u","v");
      surf.generate(fnc,0,2*Pi,50,-0.5*Pi,0.5*Pi,25);
      surf.GenerateVertexProperty(functor("color(0.75,0.75,0.5+p.x)","p"),VertexPropertyColor);
      surf.CopyToFrame(i/ctt);
   }
   saveobject(surf,DataDir+"\surf.mdl");
}
else {
   readobject(surf,DataDir+"\surf.mdl");
}

   
fr=0;
incr=1/2000;
while true do {
   if fr>0.9999999 then incr=-1*incr;
   fr=fr+incr;
   if fr<0.0000001 then incr=-1*incr;
   surf.InterpolFrame(fr);
   render;
}
