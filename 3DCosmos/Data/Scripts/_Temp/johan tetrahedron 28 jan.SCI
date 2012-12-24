Codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,10);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.3,0.3,0.3);
#bol=sf.add("sphere");#bol.color=color(1,0.5,0.5);#bol.radius=1;#bol.resolution=30;
blue=color(0,0,1);green=color(0,1,0);red=color(1,0,0);yellow=blue+green;white=yellow+red;

# tehtrahedron 4 hoekpunten :definitie als point /renderbaar maken als solidpoint
x1=point(-1,0,0);x2=point(1,0,0);y1=point(0,tan(pi/3),-1);y2=point(0,tan(pi/3),1);
pp1=sf.add("SolidPoint");pp1.position=x1;pp1.size=5;pp1.color=yellow;
pp2=sf.add("SolidPoint");pp2.position=x2;pp2.size=5;pp2.color=blue;
pp3=sf.add("SolidPoint");pp3.position=y1;pp3.size=5;pp3.color=red;
pp4=sf.add("SolidPoint");pp4.position=y2;pp4.size=5;pp4.color=green;

# method 1: 4 faces tetrahedron defined als 4 lists of 3 vertices 
# define solidshape tetra1 and add 4 faces, renderbaar maken met solidobject tetra in blue
vertices1=list;vertices2=list;vertices3=list;vertices4=list;
vertices1.add(x1);vertices1.add(x2);vertices1.add(y1);
vertices2.add(y1);vertices2.add(x2);vertices2.add(y2);
vertices3.add(x1);vertices3.add(y2);vertices3.add(y1);
vertices4.add(x1);vertices4.add(y2);vertices4.add(x2);
tetra1=solidshape; # Q1 ????? waarom keyword "solidshape" niet in kleur ??????????????
tetra1.addface(vertices1,1);tetra1.addface(vertices2,2);
tetra1.addface(vertices3,3);tetra1.addface(vertices4,4);
tetra=sf.add("solidobject");tetra.createshape(tetra1);tetra.color=blue;
tetra.setcolor(2,red); # Q 4 changing color of one face causes other faces to change into another color
tetra.renderback=true;# Q2  why keyword "renderback" not in kleur ????
# Q3 ?? BUG ??? without this instruction 2 faces are not shown ???? 


# method 2: 4 faces tetrahedr as list of 4 planes through 3 points
# define solidshape tetra2 as ployhedron with 4 faces
# renderbaar maken met solidobject in green
planes=list; planes.add(createplane2(x1,x2,y1)); planes.add(createplane2(y1,x2,y2));
             planes.add(createplane2(x1,y2,y1)); planes.add(createplane2(x1,y2,x2));
tetra2=solidshape(polyhedron(planes));
tetra3 = sf.add("solidobject"); tetra3.createshape(tetra1); tetra3.color=green;
tetra3.renderback=true;
tetra3.position = Point(2,0,0);

#l1=createline1(x1,x2);

while true do {
   render;
}
   