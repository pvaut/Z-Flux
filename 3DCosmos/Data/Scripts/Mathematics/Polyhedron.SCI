codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

points=list;idx=list;

points.add(vecnorm(vector(1,0,0))); idx.add(0);
points.add(vecnorm(vector(0,1,0))); idx.add(0);
points.add(vecnorm(vector(0,0,1))); idx.add(0);
points.add(vecnorm(vector(-1,0,0))); idx.add(0);
points.add(vecnorm(vector(0,-1,0))); idx.add(0);
points.add(vecnorm(vector(0,0,-1))); idx.add(0);

points.add(vecnorm(vector(1,1,1))); idx.add(1);

points.add(vecnorm(vector(-1,1,1))); idx.add(1);
points.add(vecnorm(vector(1,-1,1))); idx.add(1);
points.add(vecnorm(vector(1,1,-1))); idx.add(1);

points.add(vecnorm(vector(1,-1,-1))); idx.add(1);
points.add(vecnorm(vector(-1,1,-1))); idx.add(1);
points.add(vecnorm(vector(-1,-1,1))); idx.add(1);

points.add(vecnorm(vector(-1,-1,-1))); idx.add(1);

points.add(vecnorm(vector(0,1,1))); idx.add(2);
points.add(vecnorm(vector(0,1,-1))); idx.add(2);
points.add(vecnorm(vector(0,-1,1))); idx.add(2);
points.add(vecnorm(vector(0,-1,-1))); idx.add(2);

points.add(vecnorm(vector(1,0,1))); idx.add(2);
points.add(vecnorm(vector(1,0,-1))); idx.add(2);
points.add(vecnorm(vector(-1,0,1))); idx.add(2);
points.add(vecnorm(vector(-1,0,-1))); idx.add(2);

points.add(vecnorm(vector(1,1,0))); idx.add(2);
points.add(vecnorm(vector(1,-1,0))); idx.add(2);
points.add(vecnorm(vector(-1,1,0))); idx.add(2);
points.add(vecnorm(vector(-1,-1,0))); idx.add(2);

planes=list;
foreach pt in points do planes.add(CreatePlane1(@point(pt),vecnorm(pt)));
s=Polyhedron(planes,idx);

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);
refframe=sss.addsubframe("refframe");

obj=refframe.add("SolidObject");
obj.CreateShape(s);
obj.setcolor(0,color(0.4,0.4,0.8));
obj.setcolor(1,color(0.4,0.8,0.4));
obj.setcolor(2,color(0.8,0.4,0.4));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

while true do render;
