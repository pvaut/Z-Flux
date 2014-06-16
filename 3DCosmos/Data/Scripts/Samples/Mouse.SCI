
   displayname=root.displays.getmembers.get(0).name;


codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

vp=myviewport;
vp.camerapos=point(10,15,40);
vp.cameradir=vecnorm(point(10,15,0)-vp.camerapos);
vp.cameraupdir=vector(0,1,0);
vp.enableusernavigation=true;
vp.EnableUserTimeControl=true;
vp.NearClipPlane=0.05;
vp.FarClipPlane=60;
vp.FocalDistance=10;
   
vp.enableusernavigation=false;


sss.ambientlightcolor=color(0.05,0.05,0.05);


rootframe=sss.Universe;

buttoncount=3;
btstrings=list;

yp=0;

for i=0 to buttoncount-1 do {
   btstrings.add(rootframe.add("Text3D"));
   btstrings(i).Position=point(0,yp,0);
   yp=yp+0.75;
}


while true do {

   btstrings(0).Content="Left Button "+str(UIIsLeftMouseButtonDown);
   btstrings(1).Content="Middle Button "+str(UIIsMiddleMouseButtonDown);
   btstrings(2).Content="Right Button "+str(UIIsRightMouseButtonDown);
   
   render;
}