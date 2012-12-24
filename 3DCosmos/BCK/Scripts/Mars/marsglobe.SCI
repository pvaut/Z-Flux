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

surf=rootframe.add("sphere","radius":1,"position":point(0,0,0));
surf.canbuffer=true;
#surf.CalculateEdges=false;
#surf.renderback=true;

tx=rootframe.createtexture("mars",DataDir+"\textures\mars3.jpg");
surf.texture=tx.name;


while true do {
   render;
}
