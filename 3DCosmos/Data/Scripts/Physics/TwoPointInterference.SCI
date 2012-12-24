codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
vp=T_getviewport;
sss.ambientlightcolor=color(0.15,0.15,0.15);
rootframe=sss.Universe;

vp.camerapos=point(0,3,3);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=distance(point(0,0,0),vp.camerapos);


refframe=rootframe.addsubframe("refframe");



######################################################################################
# SETTINGS DIALOG BOX
######################################################################################
cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);

dsize=0.03;
psy=0.95;
psx1=0.01;psx2=0.12;
lineh=0.05;

cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Distance');
dist=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
dist.SizeX=0.08;dist.RangeSizeX=0.1;
dist.min=0;dist.max=4;dist.step=0.2;dist.DecimalCount=1;
dist.value=1;
psy=psy-lineh;

cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Ampl. 1');
ampl1=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
ampl1.SizeX=0.08;ampl1.RangeSizeX=0.1;
ampl1.min=0;ampl1.max=2;ampl1.step=0.1;ampl1.DecimalCount=1;
ampl1.value=1;
psy=psy-lineh;

cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Freq. 1');
freq1=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
freq1.SizeX=0.08;freq1.RangeSizeX=0.1;
freq1.min=0;freq1.max=2;freq1.step=0.1;freq1.DecimalCount=1;
freq1.value=1;
psy=psy-lineh;


cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Ampl. 2');
ampl2=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
ampl2.SizeX=0.08;ampl2.RangeSizeX=0.1;
ampl2.min=0;ampl2.max=2;ampl2.step=0.1;ampl2.DecimalCount=1;
ampl2.value=1;
psy=psy-lineh;

cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Freq. 2');
freq2=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
freq2.SizeX=0.08;freq2.RangeSizeX=0.1;
freq2.min=0;freq2.max=2;freq2.step=0.1;freq2.DecimalCount=1;
freq2.value=1;
psy=psy-lineh;


cframe.add("TextControl","Size":dsize,"Position":point(psx1,psy),"Content":'Resol');
resolctrl=cframe.add("ScalarControl","Size":dsize,"Position":point(psx2,psy));
resolctrl.SizeX=0.08;resolctrl.RangeSizeX=0.1;
resolctrl.min=30;resolctrl.max=200;resolctrl.step=10;
resolctrl.value=70;

psy=psy-lineh;

calcbutton=cframe.add("ButtonControl","Size":dsize,"SizeX":0.11,"Position":point(psx1,psy),"Content":'Calculate');



######################################################################################
# Object creation function
######################################################################################


fc1=0;w1=0;fc2=0;w2=0;dst2=0;

function createobject()
{
   refframe.clearobjects;
   rendermessage('Calculating');

   sp=refframe.add("surface");
   sp.RenderBack=false;
   sp.canbuffer=true;
   sp.SpecularValue=30;
   sp.SpecularColor=color(0.25,0.25,0.25);
   
   fc1=0.01*ampl1.value;
   w1=30*freq1.value;
   fc2=0.01*ampl2.value;
   w2=30*freq2.value;
   
   resol=resolctrl.value;
   
   dst2=dist.value/2;
   
   expr="";
   expr=expr+"fc1*cos(w1*distfunc(x-dst2,y))/(0.2+distfunc(x-dst2,y))";
   expr=expr+"+";
   expr=expr+"fc2*cos(w2*distfunc(x+dst2,y))/(0.2+distfunc(x+dst2,y))";
   
   fnc=functor("point(x,"+expr+",y)","x","y");
   sp.generate(fnc,-1,1,resol,-1,1,resol);
   
   cfnc=functor("color(0.5+6*p.y,0.4,0.5-6*p.y)","p");
   sp.GenerateVertexProperty(cfnc,VertexPropertyColor);
   
   hiderendermessage;
}


createobject;


while true do {
   #position light
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
   if calcbutton.wasmodified then createobject;
   render;
}
