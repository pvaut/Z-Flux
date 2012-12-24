
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

JID=0;
axiscount=6;
buttoncount=32;
axstrings=list;
btstrings=list;

yp=0;

for i=0 to axiscount-1 do {
   axstrings.add(rootframe.add("Text3D"));
   axstrings(i).Position=point(0,yp,0);
   yp=yp+0.75;
}

for i=0 to buttoncount-1 do {
   btstrings.add(rootframe.add("Text3D"));
   btstrings(i).Position=point(0,yp,0);
   yp=yp+0.75;
}

rkstring=rootframe.add("Text3D");
rkstring.Position=point(0,yp,0);



while true do {

   for i=0 to axiscount-1 do {
      axstrings(i).Content="Axis "+str(i,0)+" "+
         str(round(JoystickGetAxis(JID,i),2))+" "+
         str(round(JoystickGetAxisCorrected(JID,i),2));
   }
   for i=0 to buttoncount-1 do {
      btstrings(i).Content="Button "+str(i,0)+" "+str(JoystickButtonDown(JID,i));
   }
   
   rkstring.Content="Rocker "+str(JoystickRockerPos(JID));

   render;
}
