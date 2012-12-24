codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
vp=T_getviewport;

sss.ambientlightcolor=color(0.15,0.15,0.15);

vp.camerapos=point(0,3,3);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=distance(point(0,0,0),vp.camerapos);

rendermessage('Calculating');


rootframe=sss.Universe;

surf=rootframe.add("Surface");

p=0;
if true then {
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

hiderendermessage;
fr=0;
incr=1/500;
while true do {
   if fr>0.999 then incr=-1*incr;
   fr=fr+incr;
   if fr<0.001 then incr=-1*incr;
   surf.InterpolFrame(fr);
   render;
}
