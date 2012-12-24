
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.15,0.15,0.15);

#sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

refframe=sss.addsubframe("refframe");
sf1=sss.addsubframe("sf1");

tf=Transformation.rotate(vector(0,0,1),Pi/5);

s=Bar(point(0,-1,0),vector(2,1,1));
s.subsample(0.5);
s2=Sphere(point(1,0,0.25),0.4,20);
s3=Cylinder(point(0.5,-2,0.5),vector(0,4,0),0.3,30);
#s3.subsample(0.5);
s3.Transform(tf);
#s3=Sphere(point(0.5,0,0.5),0.2,15);

s2.setlabel(1);
s3.setlabel(2);
s=s-s2;
s=s-s3;

#break;

obj=sf1.add("SolidObject");
obj.CreateShape(s);
#obj.RenderOutLine=true;
#obj.renderback=true;
obj.setcolor(0,color(0.4,0.4,0.8));
obj.setcolor(1,color(0.4,0.8,0.4));
obj.setcolor(2,color(0.8,0.4,0.4));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=0.001;

while true do {
   incrtime;
   render;
}
