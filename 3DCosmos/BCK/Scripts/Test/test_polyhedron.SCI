
codeblock readtextfile(ScriptDir+"\TOOLS.sci");



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

#planes.add(CreatePlane1(point(0,0,0),vector(-1,0,0)));
#idx.add(2);

s=Polyhedron(planes,idx);






sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.15,0.15,0.15);

#sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

refframe=sss.addsubframe("refframe");


#break;

obj=refframe.add("SolidObject");
obj.CreateShape(s);
#obj.RenderOutLine=true;
obj.renderfront=true;
obj.renderback=true;
obj.setcolor(0,color(0.4,0.4,0.8));
obj.setcolor(1,color(0.4,0.8,0.4));
obj.setcolor(2,color(0.8,0.4,0.4));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

refframe.add("Sphere","Radius":0.1,"Position":point(2,0,0));

root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=0.001;

while true do {
   incrtime;
   render;
}
