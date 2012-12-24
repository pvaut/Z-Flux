
sf=root.scene_1.Frame_A.addobject("surface","Ojee");

sf.generate(functor("point(x,y,x*y)","x","y"),0,1,10,0,1,10);

p=sf.points(3,3);
n=sf.normals(3,3);
c=sf.colors(3,3);