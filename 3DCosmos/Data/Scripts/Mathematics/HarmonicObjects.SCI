codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);

######################################################################################
# SETTINGS DIALOG BOX
######################################################################################

dsize=0.03;
psy=0.95;

controlids=     list("fsa","psa","fca","pca","fsb","psb","fcb","pcb");
controls=list;

cframe=root.SC.Universe.addscreenframe("Controls Frame");
cframe.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,1),0.03);
cframe.enablelight=false;
cframe.BlendType=BlendTranslucent;
cframe.DepthMask=DepthMaskDisable;
cframe.color=color(1,1,1,0.5);

foreach ctrlid in controlids do {
   cframe.add("TextControl","Size":dsize,"Position":point(0.01,psy),"Content":ctrlid);
   ctrl=cframe.add("ScalarControl","Size":dsize,"Position":point(0.09,psy));
   ctrl.SizeX=0.04;ctrl.RangeSizeX=0.1;
   ctrl.min=0;ctrl.max=6;ctrl.step=1;
   ctrl.value=0;
   if ctrlid=="fsa" then ctrl.value=3;
   if ctrlid=="psa" then ctrl.value=2;
   if ctrlid=="fcb" then ctrl.value=4;
   if ctrlid=="pcb" then ctrl.value=2;
   psy=psy-0.05;
   controls.add(ctrl);
}

cframe.add("TextControl","Size":dsize,"Position":point(0.28,0.95),"Content":'Resolution');
resolctrl=cframe.add("ScalarControl","Size":dsize,"Position":point(0.28+0.12,0.95));
resolctrl.SizeX=0.08;resolctrl.RangeSizeX=0.1;
resolctrl.min=30;resolctrl.max=200;resolctrl.step=10;
resolctrl.value=70;


calcbutton=cframe.add("ButtonControl","Size":dsize,"SizeX":0.11,"Position":point(0.28,0.9),"Content":'Calculate');


#Initialise variables
fsa=0;psa=0;fca=0;pca=0;fsb=0;psb=0;fcb=0;pcb=0;


function createobject()
{
   refframe.clearobjects;
   rendermessage('Calculating');
   resol=resolctrl.value;
   
   fsa=controls(0).value;
   psa=controls(1).value;
   fca=controls(2).value;
   pca=controls(3).value;
   fsb=controls(4).value;
   psb=controls(5).value;
   fcb=controls(6).value;
   pcb=controls(7).value;
   
   fnc=functor("radial2point(sin(fsa*a)^psa+cos(fca*a)^pca+sin(fsb*b)^psb+cos(fcb*b)^pcb,a,b)","a","b");
   
   cfnc=functor("color(0.3*(@vector(p).size),0.5,1-0.3*(@vector(p).size))","p");
   
   obj=refframe.add("Surface");
   obj.renderback=true;
   obj.color=color(1,1,1);
   obj.SpecularValue=30;
   obj.SpecularColor=color(0.35,0.35,0.35);
   obj.canbuffer=true;
   obj.Generate(fnc,0,2*Pi,resol,-0.5*Pi,0.5*Pi,resol);
   
   obj.GenerateVertexProperty(cfnc,VertexPropertyColor);
   
   #make formula
   txtframe.clearobjects;
   txt=txtframe.add("FormattedText","Position":point(0.02,0.07),"Size":0.05,"Color":color(0.7,0.7,0.7),"MaxLenX":10);
   st="";const=0;
   if (fsa>0) and (psa>0) then {
      if st.length>0 then st=st+" + ";
      st=st+"sin";
      if psa>1 then st=st+"^"+str(psa);
      if fsa>1 then st=st+" "+str(fsa);
      st=st+" \theta";
   } else const=const+1;
   if (fca>0) and (pca>0) then {
      if st.length>0 then st=st+" + ";
      st=st+"cos";
      if pca>1 then st=st+"^"+str(pca);
      if fca>1 then st=st+" "+str(fca);
      st=st+" \theta";
   } else const=const+1;
   if (fsb>0) and (psb>0) then {
      if st.length>0 then st=st+" + ";
      st=st+"sin";
      if psb>1 then st=st+"^"+str(psb);
      if fsb>1 then st=st+" "+str(fsb);
      st=st+" \phi";
   } else const=const+1;
   if (fcb>0) and (pcb>0) then {
      if st.length>0 then st=st+" + ";
      st=st+"cos";
      if pcb>1 then st=st+"^"+str(pcb);
      if fcb>1 then st=st+" "+str(fcb);
      st=st+" \phi";
   } else const=const+1;
   if const>0 then {
      if st.length>0 then st=st+" + ";
      st=st+str(const);
   }
   txt.content=txt.content+"$";
   txt.content="$R="+st+"$";
   txt.enablelight=false;
   
   hiderendermessage;
}

refframe=sf.addsubframe("refframe");
refframe.transf.rotate(vector(1,0,0),Pi/2);

txtframe=sf.addscreenframe("txtframe");

createobject;


vp=root.Viewports.main;

while true do {
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
   if calcbutton.wasmodified then createobject;
   render;
}
