
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

if true then {
   bol=sf.add("sphere");
   bol.color=color(1,0.5,0.5);
   bol.radius=0.1;
   bol.resolution=30;
}

obj=sf.add("Pie");


while true do {
   render;
}
   