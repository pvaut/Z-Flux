
codeblock readtextfile(ScriptDir+"\TOOLS.sci");


#fnc=functor("0.5*x*x+0.3*y*y-0.6*z*z+1","x","y","z");

fnc=PolynomialFunctor("x","y","z");
fnc.AddPolynomialComponent(0.5,2,0,0);
fnc.AddPolynomialComponent(0.3,0,2,0);
fnc.AddPolynomialComponent(-0.6,0,0,2);
fnc.AddPolynomialComponent(-0.5,0,0,0);

fnc.AddPolynomialComponent(-0.1,4,0,0);

fnc.AddPolynomialComponent(0.005,6,0,0);

#s=CreateSurface(fnc,0.1,3);

s=CreateSurface(fnc,0.1,7,point(0,0,0),false);





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
