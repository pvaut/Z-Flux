
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.15,0.15,0.15);

#sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

refframe=sss.addsubframe("refframe");
sf1=sss.addsubframe("sf1");
obj=sf1.add("SolidObject");

cset=Contour(list(point(0,0,0),point(0,1,0),point(1,0,0)));
s=ConeShape(cset,point(1,1,1));

   
obj.CreateShape(s);
   

#obj.BlendType=BlendTransparent;
#obj.DepthMask=DepthMaskDisable;
#obj.RenderOutLine=true;
obj.renderback=true;
obj.color=color(0.4,0.6,0.8);
obj.SetColor(0,color(0.4,0.6,1));
obj.SetColor(1,color(1,0.5,0.25));
obj.SetColor(2,color(1,0,0));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

root.time=time(2008,1,1,0,0,0);
root.TimeSpeed=0.001;

while true do {
   incrtime;
   render;
}
