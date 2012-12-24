
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

codeblock readtextfile(ScriptDir+"\Physics\Optics\_Optics.sci");

SetOpticFrame(T_scene_create);

sss=T_getscene;
vp=T_getviewport;
vp.CameraPos=point(0,0,10);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.3,0.3,0.3);

refrprop=CreateRefractingProperties();
mirrorprop=CreateReflectingProperties();

refrprop.freqindex=-0.02;
sh=Sphere(point(0,0,0),1,30);
AddRefractingObject(sh,refrprop);
rayprop=CreateRayProperties();
rayprop.type=2;
rayprop.alsoreflectbouncenrs=list(2);

for i=0 to 5 do
   AddMultiColorRay(point(3,i/10,0.5),vector(-1,-0.05,0),rayprop);


TraceRays();

while true do {
   render;
}
   