codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;
myviewport.focaldistance=7;
sss.ambientlightcolor=color(0.05,0.05,0.05);
rootframe=sss.Universe;

createnew=true;


tx=rootframe.createtexture("star",DataDir+"\textures\star2.bmp");
pe=rootframe.add("PEngine");

if createnew then {
   pe.HasParticleColors=true;
   pe.BlendType=BlendTransparent;
   pe.DepthMask=DepthMaskDisable;
   pe.pointsize=5;
   pe.Texture=tx.name;
   pe.PEngineRenderType=PengineRenderPoints;
   
   count=10000;
   pe.setsize(count);
   pe.AddFrame(time(2009,1,1,0,0,0));
   pe.AddFrame(time(2009,1,2,0,0,0));
   pe.AddFrame(time(2009,1,3,0,0,0));
   
   for i=0 to count-1 do {
      pt=point(random,random,random);
      col=color(random,random,random,0.3);
      pe.FramePtPosition(0,i)=pt;
      pe.FramePtPosition(1,i)=pt+(pt-point(0,0,0));
      pe.FramePtPosition(2,i)=point(0,0,0);
      
      pe.FramePtColor(0,i)=col;
      pe.FramePtColor(1,i)=col+color(0,0,1);
      pe.FramePtColor(2,i)=color(1,0,0);
   }
   saveobject(pe,datadir+"\particle_engine.mdl");
} else {
   readobject(pe,datadir+"\particle_engine.mdl");
}


scf=rootframe.addscreenframe("sf");
clk=scf.add("Clock");
clk.position=point(0.2,0.2);
clk.EnableLight=false;
clk.Size=0.05;
clk.Type=ClockTypeAnalog;


root.time=time(2009,1,1,0,0,0);
root.TimeSpeedFactor=1000;

while true do {
   incrtime;
   render;
}
