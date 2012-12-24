
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


refrprop.freqindex=-0.1;
cs=FlatContourSet;
cs.addpoint(point(0,0,0));cs.addpoint(point(2,0,0));cs.addpoint(point(0,2,0));cs.close();
cs.calcflatnormals();
sh=ExtrudedShape(cs,1,2);
obj=AddRefractingObject(sh,refrprop);
obj.RenderBack=true;
rayprop=CreateRayProperties();
rayprop.type=2;
AddMultiColorRay(point(2,5,0.5),vector(-0.2,-1,0),rayprop);


TraceRays();

while true do {
   render;
}
   