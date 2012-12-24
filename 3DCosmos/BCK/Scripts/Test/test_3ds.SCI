
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.3,0.3,0.3);
#sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

refframe=sss.addsubframe("refframe");


sf1=sss.addsubframe("sf1");
sf1.cancache=false;
#sf1.color=color(0.8,0.8,0.8);

sf1.transf.rotate(vector(1,0,0),-0.5*Pi);
#sf1.transf.rotate(vector(0,0,1),Pi);

if false then {
   bola=sf1.addobject("sphere");
   bola.position=point(0,0,0);
   bola.color=color(1,0.3,0.3,1);
   bola.radius=0.5;
   bola.resolution=20;
   
   bolb=sf1.addobject("sphere");
   bolb.position=point(1.2,0,0);
   bolb.color=color(0.3,0.3,1);
   bolb.radius=0.5;
   bolb.resolution=20;
}
objectpath="";objectname="";scalefactor=0;
if true then {
   objectpath="C:/3DCosmos/3DCosmos/Models/ISS_Building";
   #objectname="iss_1A";
   objectname="isscomplete";
   scalefactor=3.1;
}




if false then {
   objectpath="C:/3DCosmos/3DCosmos/Models/MER";
   #objectname="iss_1A";
   objectname="mer2";
   scalefactor=0.03;
}

if false then {
   objectpath="J:\Astro3D\models_3ds\cassini";
   objectname="cassini";
   scalefactor=1.1;
}

sf1.LoadObject(objectpath,objectname+".3ds","3DS",scalefactor);


comps=sf1.getmembers;
for i=0 to comps.size-1 do if typeof(comps(i))=="SolidObject" then {
##   comps(i).cancache=true;
#   comps(i).canbuffer=true;
   comps(i).enablelight=true;
}


root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=0.001;

while true do {
   incrtime;
   render;
}
