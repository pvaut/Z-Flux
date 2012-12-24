codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);

b=Blob;
b.AddSphere(point(0.2,0,0),1.5,3,-0.25);
b.AddTorus(point(2,0,0),vector(0,0,1),2,0.5,1.5);
b.AddTorus(point(1,0,0),vector(0,1,0),2,0.5,2,);
b.AddDisc(point(0,0,0),vector(0,0,1),2,0.5,2);
s=b.CreateSolidShape(0.07);

refframe=sss.addsubframe("refframe");
obj=refframe.add("SolidObject");
obj.CreateShape(s);
obj.renderfront=true;
obj.renderback=true;
obj.canbuffer=true;
obj.setcolor(0,color(0.4,0.4,0.8));
obj.setcolor(1,color(0.4,0.8,0.4));
obj.setcolor(2,color(0.8,0.4,0.4));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

while true do render;
