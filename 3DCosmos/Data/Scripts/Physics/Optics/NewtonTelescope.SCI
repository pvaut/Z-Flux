
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

codeblock readtextfile(ScriptDir+"\Physics\Optics\_Optics.sci");

rootframe=T_scene_create;
SetOpticFrame(rootframe);

sss=T_getscene;
vp=T_getviewport;
vp.CameraPos=point(0,0,10);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;

sss.ambientlightcolor=color(0.25,0.25,0.25);

refrprop=CreateRefractingProperties();
mirrorprop=CreateReflectingProperties();

AddSphericalMirror(point(-2,0,0),vector(-1,0,0),1,0,-11,0.3,mirrorprop);
#AddSphericalMirror(point(3,0,0),vector(1,1,0),0.2,0,200,0.05,mirrorprop);

#secundary mirror
sh=Cylinder(point(0,0,0),vector(0,0,1),0.2,30);
sh2=Bar(point(-0.2,-0.5,-0.6),vector(1,1,1));
tf=Transformation;
tf.rotate(vector(0,1,0),deg2rad(90+52));
sh2.Transform(tf);
sh=sh-sh2;
tf=Transformation;
tf.rotate(vector(0,1,0),pi/2);
tf.translate(vector(0,0,2));
sh.transform(tf);
AddReflectingObject(sh,mirrorprop);


rayprop=CreateRayProperties();

for i=0 to 2 do
   AddRay(point(5,0.4,i/5),vector(-1,0,0),rayprop);


TraceRays();

#Create tubus
tubusframe=rootframe.addsubframe("TubusFrame");
tubusframe.Transf.translate(vector(-2,0,0));
tubusframe.Transf.rotate(vector(0,1,0),Pi/2);

cs=FlatContourSet;
fnc=Functor("point(sin(a),cos(a),0)","a");
cs.generate(fnc,0,2*Pi,60);
cs.close();
cs.newcontour();
fnc=Functor("point(1.02*cos(a),1.02*sin(a),0)","a");
cs.generate(fnc,0,2*Pi,60);
cs.close();

sh=ExtrudedShape(cs,5,2);


tubusframe1=tubusframe.addsubframe("TubusFrame1");
tubusframe1.addclipplane(CreatePlane1(point(0,-1,2),vector(1,1,-0.3)));
obj=tubusframe1.add("SolidObject","color":color(0.5,0.6,0.5));
obj.CreateShape(sh);

tubusframe2=tubusframe.addsubframe("TubusFrame2");
tubusframe2.addclipplane(CreatePlane1(point(0,-1,2),-1*vector(1,1,-0.3)));
obj=tubusframe2.add("SolidObject","color":color(0.5,0.5,0.6,0.3));
obj.BlendType=BlendTranslucent;obj.DepthMask=DepthMaskDisable;
obj.CreateShape(sh);




while true do {
   render;
}
   