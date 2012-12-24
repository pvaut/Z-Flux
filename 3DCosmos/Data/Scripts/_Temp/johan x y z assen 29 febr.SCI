Codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,10);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.3,0.3,0.3);
#bol=sf.add("sphere");#bol.color=color(1,0.5,0.5);#bol.radius=1;#bol.resolution=30;

# assen
### vraag ?????: yas en zas zijn niet zichtbaar , xas wel, waar zit de fout ???????
w=0.1;
xas=sf.add("arrow");xas.axis=vector(5,0,0);xas.width=w;xas.thickness=w;
yas=sf.add("arrow");yas.axis=vector(0,3,0);yas.width=w;yas.thickness=w;yas.NormAxis=vector(1,0,0);
zas=sf.add("arrow");zas.axis=vector(0,0,3);zas.width=w;zas.thickness=w;zas.NormAxis=vector(0,1,0);
# as labels x, y, z
xt1=sf.add("textflat");xt1.content= "x";xt1.size=.3;xt1.position=point(5.05,0.05,0.05);
yt1=sf.add("textflat");yt1.content= "y";yt1.size=.3;yt1.position=point(0.05,3.05,0.05);
zt1=sf.add("textflat");zt1.content= "z";zt1.size=.3;zt1.position=point(0.05,0.05,3.05);

#### ALTERNATIEVE MANIER OM ASSENSTELSEL TE TEKENEN
sf.add("Curve","Size":2,"Arrow2Size":0.2).makeline(point(1,1,1),point(3,1,1));
sf.add("Curve","Size":2,"Arrow2Size":0.2).makeline(point(1,1,1),point(1,3,1));
sf.add("Curve","Size":2,"Arrow2Size":0.2).makeline(point(1,1,1),point(1,1,3));

while true do {
   render;
}
   