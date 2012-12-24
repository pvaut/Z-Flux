codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;
sss.ambientlightcolor=color(0.05,0.05,0.05);

rootframe=sss.Universe.addsubframe("Root");

st=readtextfile(datadir+"\basichelp.txt");

txt=rootframe.add("FormattedText");
txt.enablelight=false;
txt.position=point(-5,3.5,0);
txt.size=0.15;
txt.content=st;
txt.MaxLenX=10;
txt.color=color(0.7,0.7,0.7);
txt.renderback=true;

myviewport.camerapos=point(0,0,10);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,1,0);

WaitReleaseAll;
while true do {
   render;
   if LeftMouseClicked() then stop;
}
