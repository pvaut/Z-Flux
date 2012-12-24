codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.05,0.05,0.05);


rootframe=sss.Universe;

rootframe.add("sphere","radius":0.1);
rootframe.add("sphere","radius":0.1,"position":point(1,0,0),"color":color(1,0,0));
rootframe.add("sphere","radius":0.1,"position":point(0,1,0),"color":color(0,1,0));
rootframe.add("sphere","radius":0.1,"position":point(0,0,1),"color":color(0,0,1));

surf=rootframe.add("Surface","color":color(1,1,1));
surf.canbuffer=true;
surf.CalculateEdges=false;
#surf.renderback=true;

#tx=rootframe.createtexture("mars",DataDir+"\textures\mars3.jpg");
#surf.texture=tx.name;

#break;
topo=matrix;

if false then {
   filename=DataDir+"\topo\Mars_Topo_04.img";
   topo.LoadFile(filename,
      360*4,0,-1,
      180*4,0,-1,
      2);
}

if true then {
   filename=DataDir+"\topo\Mars_Topo_16.img";
   topo.LoadFile(filename,
      360*16,1000,3000,
      180*16,1000,2000,
      2);
}


if false then {
   filename=DataDir+"\topo\Mars_Topo_32.img";
   topo.LoadFile(filename,
      360*32,1000,3000,
      180*32,1000,2000,
      2);
}


surf.CreateTopoSphere(topo,
   -0.25*Pi,0.25*Pi,
   -0.25*Pi,0.25*Pi,
   0.000002,1);


while true do {
   render;
}
