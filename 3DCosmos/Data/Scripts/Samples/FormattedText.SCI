codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;
sss.ambientlightcolor=color(0.05,0.05,0.05);

rootframe=sss.Universe.addsubframe("Root");

st="This is an example of some \textbf{LaTeX}-formatted text rendered in a 3D scene.";
st=st+"It contains some random proza, and an example of an inline formula: $x^2+\alpha$. We end with some displayed math: ";
st=st+"\begin{equation} ";
st=st+"\sum_{i=1}^n i^2 + \left| \, \vec{v} \times \vec{w} \right| + x_1^2 +  \sin \beta - (1+x)^2 + \sqrt{ \frac{1}{1+x} }";
st=st+" \end{equation}";

txt=rootframe.add("FormattedText");
txt.enablelight=false;
txt.position=point(-5,0,0);
txt.size=0.25;
txt.content=st;
txt.MaxLenX=10;
txt.color=color(0.7,0.7,0.7);
txt.renderback=true;

myviewport.camerapos=point(5,5,10);
myviewport.cameradir=vecnorm(point(0,0,0)-myviewport.camerapos);
myviewport.cameraupdir=vector(0,1,0);

while true do render;
