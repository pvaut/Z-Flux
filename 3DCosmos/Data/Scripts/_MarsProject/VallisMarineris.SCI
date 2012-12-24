codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_CreateMarsSurface.sci");


sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

SUB_init;
PanelLeft=SUB_createleftpanel;
SUB_showloading(true);


myviewport.NearClipPlane=0.01;
myviewport.FarClipPlane=10;
myviewport.Focaldistance=1.01*myviewport.NearClipPlane;

myviewport.enableusernavigation=false;
JID=0;#joystick ID

sss.ambientlightcolor=color(0.05,0.05,0.05);


rootframe=sss.Universe;



myviewport.camerapos=radial2point(10,0,0);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,0,1);


longit=deg2rad(-73);
lattit=deg2rad(-9.6);
rs1=450;
rs2=250;

SUB_RenderMarsSurface(rootframe,longit,lattit,rs1,rs2);
