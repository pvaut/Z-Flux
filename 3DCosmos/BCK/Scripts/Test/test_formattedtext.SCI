



codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.05,0.05,0.05);



rootframe=sss.Universe.addsubframe("Root");

rootframe.add("Sphere","Name":"SF01","Radius":0.02,"Color":color(1,0,0));

txt=rootframe.add("FormattedText");
txt.enablelight=false;

if true then {
   txt.size=0.25;
   txt.content="$\sum_{i=1}^n i^2 + \left| \, \vec{v} \times \vec{w} \right| + x_1^2 +  \sin \beta - (1+x)^2 + \sqrt{ \frac{1}{1+x} } $";
}

if false then {
   st="Dit is een test om te zien hoe \textit{goed} dit gaat. Er is hier nu wat proza.";
   st=st+"En hier volgt een inline formule: $x^2+\alpha$. Grappig, he?";
   txt.size=0.25;
   txt.content=st;
   txt.MaxLenX=10;
}





root.time=time(2000,1,1,0,0,0);
root.TimeSpeed=10;

myviewport.camerapos=point(0,0,20);
myviewport.cameradir=vector(0,0,-1);
myviewport.cameraupdir=vector(0,1,0);

while true do {
   incrtime;
   render;
}
