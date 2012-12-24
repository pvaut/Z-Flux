
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

AddSphericalLens(point(0,0,0),vector(1,0,0),1,4,90,0.03,refrprop);
AddSphericalLens(point(1,0,0),vector(1,0,0),1,3,3,0.03,refrprop);
AddSphericalMirror(point(-1,0,0),vector(-1,0,0),1,0.15,-2,0.3,mirrorprop);
AddSphericalLens(point(2,-0.7,0),vector(1,-0.5,0),0.7,1,1,0.1,refrprop);


rayprop=CreateRayProperties();

for i=0 to 10 do
   AddRay(point(3,1,0.5),vector(-1,-0.05*i,0),rayprop);


TraceRays();

while true do {
   render;
}
   