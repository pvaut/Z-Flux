Codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;

sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(2,2,10); #beetje perspectief op z as
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.3,0.3,0.3);
#bol=sf.add("sphere");#bol.color=color(1,0.5,0.5);#bol.radius=1;#bol.resolution=30;
blue=color(0,0,1);green=color(0,1,0);red=color(1,0,0);yellow=blue+green;white=blue+green+red;
# assen en tekst labels x y z
xas=sf.add("curve");xas.makeline(point(0,0,0),point(4,0,0));xas.arrow2size=0.3;
yas=sf.add("curve");yas.makeline(point(0,0,0),point(0,3,0));yas.arrow2size=0.3;
zas=sf.add("curve");zas.makeline(point(0,0,0),point(0,0,3));zas.arrow2size=0.3;
xt1r=sf.add("textflat");xt1r.content= "x";xt1r.size=.2;xt1r.position=point(4.05,0.05,0.05);
yt1r=sf.add("textflat");yt1r.content= "y";yt1r.size=.2;yt1r.position=point(0.05,3.05,0.05);
zt1r=sf.add("textflat");zt1r.content= "z";zt1r.size=.2;zt1r.position=point(0.05,0.05,3.05);

# tehtrahedron 4 hoekpunten :definitie als point /renderbaar maken als solidpoint
x1=point(-1,0,0);x2=point(1,0,0);y1=point(0,tan(pi/3),1);y2=point(0,tan(pi/3),-5);
c1=point(0,tan(pi/3)/2,0);
px1r=sf.add("SolidPoint");px1r.position=x1;px1r.size=5;px1r.color=yellow;
px2r=sf.add("SolidPoint");px2r.position=x2;px2r.size=5;px2r.color=blue;
py1r=sf.add("SolidPoint");py1r.position=y1;py1r.size=5;py1r.color=red;
py2r=sf.add("SolidPoint");py2r.position=y2;py2r.size=5;py2r.color=green;
pc1r=sf.add("SolidPoint");pc1r.position=c1;pc1r.size=10;pc1r.color=red;
x1arm=sf.add("curve");x1arm.makeline(x1,c1);
x2arm=sf.add("curve");x2arm.makeline(x2,c1);
y1arm=sf.add("curve");y1arm.makeline(y1,c1);
y2arm=sf.add("curve");y2arm.makeline(y2,c1);
# tetra method 1: 4 faces tetrahedron defined als 4 lists of 3 vertices 
# define solidshape tetra1 and add 4 faces, renderbaar maken met solidobject tetra in blue
verticesFront=list;vertices2=list;vertices3=list;vertices4=list;
verticesFront.add(x1);verticesFront.add(x2);verticesFront.add(y1);
vertices2.add(y1);vertices2.add(x2);vertices2.add(y2);
vertices3.add(x1);vertices3.add(y1);vertices3.add(y2);
vertices4.add(x1);vertices4.add(y2);vertices4.add(x2);
tetra1=solidshape; tetra1.addface(verticesFront,1);tetra1.addface(vertices2,2);
tetra1.addface(vertices3,3);tetra1.addface(vertices4,4);
tetra1r=sf.add("solidobject");tetra1r.createshape(tetra1);tetra1r.color=blue;
tetra1r.setcolor(1,blue);tetra1r.position = Point(2,0,0);

# tetra method 2: 4 faces tetrahedr as list of 4 planes through 3 points
# define solidshape tetra2 as ployhedron with 4 faces
# renderbaar maken met solidobject in green en verschuiven op xas
planes=list(createplane2(x1,x2,y1),createplane2(y1,x2,y2),
        createplane2(x1,y1,y2),createplane2(x1,y2,x2));tetra2=solidshape(polyhedron(planes));
tetra2r = sf.add("solidobject"); tetra2r.createshape(tetra1); tetra2r.color=green;
tetra2r.setcolor(1,red);tetra2r.blendtype=blendtransparent;

#spirals
crv=sf.add("curve");fnc=functor("point(0.3*cos(t),t/5,0.3*sin(t))","t");
crv.generate(fnc,0,4*pi,100);
while true do {
   render;
}
   