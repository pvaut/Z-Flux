codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);

rendermessage('Calculating');

refframe=sss.addsubframe("refframe");
sf1=sss.addsubframe("sf1");
obj=sf1.add("SolidObject");

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
   
obj.CreateShape(s);

obj.color=color(0.4,0.6,0.8);
obj.SetColor(0,color(0,0.5,1));
obj.SetColor(1,color(1,0.5,0.25));
obj.SetColor(2,color(1,0,0));
obj.SpecularValue=30;
obj.SpecularColor=color(0.35,0.35,0.35);

hiderendermessage;

while true do render;
