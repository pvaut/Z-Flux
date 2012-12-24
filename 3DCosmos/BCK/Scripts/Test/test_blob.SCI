
codeblock readtextfile(ScriptDir+"\TOOLS.sci");



id=3;

#break;
b=Blob;

if id==4 then {
   b.Addtorus(point(-3,0,0),vector(0,1,0),2,0.5,2);
   b.Addtorus(point( 0,0,0),vector(0,0,1),2,0.5,2);
   b.Addtorus(point( 3,0,0),vector(0,1,0),2,0.5,2);
   b.Addtorus(point( 6,0,0),vector(0,0,1),2,0.5,2);
}


if id==3 then {
   b.AddSphere(point(0.2,0,0),1.5,3,-0.25);
   b.AddTorus(point(2,0,0),vector(0,0,1),2,0.5,1.5);
   b.AddTorus(point(1,0,0),vector(0,1,0),2,0.5,2,);
   b.AddDisc(point(0,0,0),vector(0,0,1),2,0.5,2);
}

if id==1 then {
   b.AddSegment(point(-1,0,0),point(2,0,0),0.5,0.8);
   b.AddSegment(point(-1,-2,0),point(1,2,0),0.5,0.8);
}

if id==2 then {
   b.AddSphere(point(0,0,0),1,2.5);
   b.AddSphere(point(1,0,0),0.5,1.5,-1);
   b.AddSphere(point(2,0,0),1,2.5);
   b.AddSphere(point(3,0,0),1,2.5);
   b.AddSphere(point(0,1,0),1,2.5);
   b.AddSphere(point(0,2,0),1,2.5);
   b.AddSphere(point(0,3,1),0.75,2.5);
   b.AddSphere(point(0,4,2),0.5,2.5);
}

s=b.CreateSolidShape(0.07);


#s=s-Cylinder(point(0,0,0),vector(3,0,0),1);


#s=CreateSurface(fnc,0.1,7,point(0,0,0),false);

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
obj.canbuffer=true;
obj.setcolor(0,color(0.4,0.4,0.8));
obj.setcolor(1,color(0.4,0.8,0.4));
obj.setcolor(2,color(0.8,0.4,0.4));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

refframe.add("Sphere","Radius":0.1,"Position":point(0,0,0));
refframe.add("Sphere","Radius":0.1,"Position":point(2,0,0));

root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=0.001;

while true do {
   incrtime;
   render;
}
