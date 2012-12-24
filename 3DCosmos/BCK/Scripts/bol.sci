
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

sss.ambientlightcolor=color(0.3,0.3,0.3);
sss.VolumeShadowAdd(0,color(0,0,0,0.5),0.0002,20);

bol=sf.add("sphere");
bol.color=color(1,0.5,0.5);
bol.radius=0.5;
bol.resolution=30;
bol.canbuffer=true;

if true then {
   bol2=sf.add("sphere");
   bol2.position=point(1,1,1);
   bol2.color=color(1,1,0);
   bol2.radius=0.25;
   bol2.resolution=30;
   bol2.canbuffer=true;
}

if true then {
   bar=sf.addbar(point(0,2,0),1,1,1);
   bar.color=color(0,1,0);
   sf.motion=motionrotate.create(sf);
   sf.motion.normdir=vector(0,1,0);
   sf.motion.rotspeed=0.001;
}

function vertexcolor(p)
{
   return(color(p.x,p.y,p.z));
}

if true then {
   surf=sf.add("surface");
   func=functor("point(u,0.75*sin(u*v),v)","u","v");
#   surf.generate(func,-3,3,120,-3,3,120);
   surf.generate(func,-1.5,1.5,30,-1.5,1.5,30);
#   surf.GenerateVertexProperty(functor("vertexcolor(p)","p"),VertexPropertyColor);
#   surf.GenerateVertexProperty(FunctionFunctor("vertexcolor"),VertexPropertyColor);
   surf.renderback=true;
   surf.color=color(0.4,0.4,0.6);
   surf.SpecularValue=40;
   surf.SpecularColor=color(0.5,0.5,0.5);
   surf.canbuffer=false;
}


root.time=time(2008,1,1,0,0,0);
while root.time-time(2008,1,1,3,0,0)<0 do {
   incrtime;
   render;
   ttm=objectroot.time+0;
}
   