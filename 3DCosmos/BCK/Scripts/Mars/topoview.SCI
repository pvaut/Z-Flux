codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.05,0.05,0.05);


rootframe=sss.Universe;

#rootframe.add("sphere","radius":0.1);
#rootframe.add("sphere","radius":0.1,"position":point(1,0,0),"color":color(1,0,0));
#rootframe.add("sphere","radius":0.1,"position":point(0,1,0),"color":color(0,1,0));
#rootframe.add("sphere","radius":0.1,"position":point(0,0,1),"color":color(0,0,1));

surf=rootframe.add("Surface","color":color(1,1,1));
surf.canbuffer=true;
#surf.CalculateEdges=false;
#surf.renderback=true;

#tx=rootframe.createtexture("mars",DataDir+"\textures\mars3.jpg");
#surf.texture=tx.name;

#break;
topo=matrix;

mapresol=16;

pos_longit=deg2rad(50);
pos_lattit=deg2rad(0);

idx_longit=floor(pos_longit/(2*Pi)*360*mapresol);
idx_lattit=floor((Pi/2+pos_lattit)/Pi*180*mapresol);
#break;

filename=DataDir+"\topo\Mars_Topo_"+str(mapresol)+".img";
topo.LoadFile(filename,
   360*mapresol,idx_longit-200,idx_longit+200,
   180*mapresol,idx_lattit-100,idx_lattit+100,
   2);

#surf.CreateTopoSphere(topo,
#   -0.25*Pi,0.25*Pi,
#   -0.25*Pi,0.25*Pi,
#   0.000002,1);

surf.Position=point(-1,0,0);
surf.CreateTopoSphere(topo,
   -0.5,0.5,
   -0.3,0.3,
   0.000001,1);
   
#myviewport.FocalDistance=0.5;
#myviewport.EyeSeparation=myviewport.FocalDistance/10;
#myviewport.CameraPos=point(0.1,0,0);
#myviewport.CameraDir=vector(-0.3,1,0);
#myviewport.CameraUpDir=vector(1,0,0);
   
#surf.GenerateVertexProperty(functor("color(0.5-10*p.z,0.5-10*p.z,0.5+10*p.z)","p"),VertexPropertyColor);


while true do {
   render;
}
