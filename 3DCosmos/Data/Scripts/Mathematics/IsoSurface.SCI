codeblock readtextfile(ScriptDir+"\_TOOLS.sci");



sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);
refframe=sss.addsubframe("refframe");
vp=root.Viewports.main;

vp.camerapos=point(3,10,20);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=distance(point(0,0,0),vp.camerapos);


rendermessage('Calculating','This may take a while!');

x=0;y=0;z=0;
a=0.93;
k=4.5;
b=0.8;
fnc=functor("sqr(sqr(x)+sqr(y)+sqr(z)-a*sqr(k)) - b * (sqr(z-k)-2*sqr(x)) * (sqr(z+k)-2*sqr(y))","x","y","z");

s=CreateSurface(fnc,0.25,6);


obj=refframe.add("SolidObject");
obj.CreateShape(s);
obj.renderback=true;
obj.setcolor(0,color(0.4,0.4,0.8));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);
obj.canbuffer=true;

renderbasis(refframe,5);

#render formula
st="$ \left[ x^2 + y^2 + z^2 - a k^2 \right] ^2 - b \left[ (z-k)^2 - 2 x^2 \right] \left[ (z+k)^2 - 2 y^2 \right] = 0$";
txtframe=sf.addscreenframe("txtframe");
txt=txtframe.add("FormattedText","Position":point(0.02,0.1),"Size":0.05,"Color":color(0.7,0.7,0.7),"MaxLenX":10);
txt.content=st;
txt.enablelight=false;
txt.renderback=true;


hiderendermessage;

while true do {
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
   render;
}
