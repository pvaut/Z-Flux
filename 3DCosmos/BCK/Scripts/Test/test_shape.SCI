
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.15,0.15,0.15);

#sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.002,20);

refframe=sss.addsubframe("refframe");
sf1=sss.addsubframe("sf1");
obj=sf1.add("SolidObject");

createvar(s);
if true then {
   s=Bar(point(-1,-1,-1),vector(2,2,2));
   s.subsample(0.25);
   for i=0 to 5 do {
      pt=point(0.5,0,-1*i/5);
      u=Bar(pt,vector(2,2,2));
      u.subsample(0.5);
      U.Transform(Transformation.rotate(vector(0,0,1),i/2));
      u.SetLabel(1);
      s=s-u;
   }
   s.subsample(0.25);
   
   s.WarpSpiral(0.25);
   s.WarpConalPinch(2);
   
   s.subsample(0.2);
   
   s3=sphere(point(0,0.5,0),0.4,30);s3.setlabel(2);
   s=s-s3;
}
   
#   break;
obj.CreateShape(s);
   

if false then {
   cs=FlatContourSet;
   cs.addpoint(point(0,0));
   cs.addpoint(point(1,0));
   cs.addpoint(point(2,2));
   cs.addpoint(point(1,2));
   cs.addpoint(point(0,1));
   cs.close;
   cs.calcflatnormals;
   
   cs.newcontour;
   fnc=functor("point(0.5+0.3*cos(a),0.6-0.5*sin(a),0)","a");
   cs.generate(fnc,0,2*Pi,40);
      
#   s1=ExtrudedShape(cs,0.5);
#   s=s1-Sphere(point(1,1,0.5),0.6);
#   obj.CreateShape(s);
}

if false then {
   cs=FlatContourSet;
   cs.addpoint(point(0,1));
   cs.addpoint(point(1,1.5));
   cs.addpoint(point(2,1));
   cs.calcflatnormals;

   s1=RevolvedShape(cs,80);
   s=s1-Sphere(point(1.6,0,0.5),1.2,40);
   obj.CreateShape(s);
}


#break;

#obj.CreateFlatShape(cs);

#obj.BlendType=BlendTransparent;
#obj.DepthMask=DepthMaskDisable;
#obj.RenderOutLine=true;
obj.renderback=true;
obj.color=color(0.4,0.6,0.8);
obj.SetColor(0,color(0,0.5,1));
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
