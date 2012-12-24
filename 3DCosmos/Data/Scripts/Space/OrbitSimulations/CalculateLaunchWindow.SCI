codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

sf=T_scene_create;
sc=T_getscene;
vp=T_getviewport;
vp.nearclipplane=0.1;
vp.farclipplane=50;

vp.camerapos=point(0,0,10);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=13;

sc.Light0Pos=point(100,300,100);

sc.ambientlightcolor=color(0.1,0.1,0.1);

textcl=color(0.5,0.5,0.5);

eqposx=-5.5;
eqposy=3.5;
eqlineh=0.95;
textframe=sf.addsubframe("TextFrame");
textframe.transf.translate(vector(0,0,-1.5));
textframe.transf.rotate(vector(0,1,0),Pi/8);
function addeq(content)
{
   txt=textframe.add("FormattedText","Position":point(eqposx,eqposy,0),"Size":0.35,"MaxLenX":9999,"Content":content,
            "Enablelight":false,"color":textcl);
   eqposy=eqposy-eqlineh;
}

function addsep(py) { eqposy=eqposy-py; }

addeq("$\Omega_a(t) = \alpha_a + \omega_a t$\ \ \ $(\omega_a=\frac{2 \pi}{T_a})$");

addeq("$\Omega_b(t) = \alpha_b + \omega_b t$\ \ \ $(\omega_b=\frac{2 \pi}{T_b})$");




AA=1.4;AB=2.2;
TA=360.0/(0.9856076686/(AA^1.5));
TB=360.0/(0.9856076686/(AB^1.5));

AC=(AA+AB)/2;
TC=360.0/(0.9856076686/(AC^1.5));
EC=(AB-AA)/(AA+AB);

colA=color(0,0.4,0.7);colB=color(0.7,0.4,0);colC=color(0,0.6,0);

ocenter=sf.addsubframe("OrbitCenter");
ocenter.transf.origin=point(1.75,1.2,0.7);

ocenter.transf.rotate(vector(1,0,0),-0.6*Pi/2);
#ocenter.transf.rotate(vector(0,0,1),Pi/6);

function createorbit(A,T,E,periang,cl,createcurve)
{
   oframe=ocenter.addsubframe;
   oframe.motion=MotionKepler.create(oframe);
   oframe.motion.SemiMajorAxis=A;
   oframe.motion.Eccentricity=E;
   oframe.motion.Period=T;
   oframe.motion.ArgPerigee=periang;
   oframe.motion.PeriTime=root.time;
   oframe.add("sphere","radius":0.05,"color":(cl+color(1,1,1)));
   if createcurve then {
      crv=ocenter.add("Curve","color":cl);
      oframe.motion.CreateCurve(crv,200);
   }
   return(oframe);
}

Astartang=1.4*Pi/4;
Bstartang=Astartang+Pi-((TC/2)/TB)*2*Pi;

aframe=createorbit(AA,TA,0,Astartang,colA,true);
bframe=createorbit(AB,TB,0,Bstartang,colB,true);


cframe=createorbit(AC,TC,EC,Astartang,colC,false);

ocenter.add("Curve","size":2,"color":colc).track(cframe,0.01);

ocenter.add("Curve","color":textcl,"CurveRenderType":CurveRenderDash).makeline(point(0,0,0),point(1.2*AB,0,0));
ocenter.add("Curve","color":textcl,"CurveRenderType":CurveRenderDash).makeline(point(0,0,0),radial2point(1.2*AB,Astartang,0));
ocenter.add("Curve","color":textcl,"CurveRenderType":CurveRenderDash).makeline(point(0,0,0),radial2point(1.2*AB,Bstartang,0));

a1=ocenter.add("Curve","Size":2,"Arrow2Size":0.15,"color":textcl);
a1.Generate(functor("radial2point(AA+0.1,a,0)","a"),0,Astartang,60);

labelf=ocenter.addviewdirframe(radial2point(AA+0.4,Astartang/2,0));
labelf.add("FormattedText","position":point(-0.2,0.2),"enablelight":false,"color":textcl,"Content":"$\Omega_a$","size":0.3);

a1=ocenter.add("Curve","Size":2,"Arrow2Size":0.15,"color":textcl);
a1.Generate(functor("radial2point(AB+0.1,a,0)","a"),0,Bstartang,60);

labelf=ocenter.addviewdirframe(radial2point(AB+0.4,Bstartang/2,0));
labelf.add("FormattedText","position":point(-0.2,0.2),"enablelight":false,"color":textcl,"Content":"$\Omega_b$","size":0.3);

stepnr=0;
root.TimeSpeedFactor=0;
starttime=root.time;
while true do {
   incrtime;
   donextstep=false;
   if UIGetKeyPressed=="Return" then {
      donextstep=true;
      stepnr=stepnr+1;
   }
   
   if (donextstep) and (stepnr==1) then {
      addeq('Start at'+" $t=t_0$ $\rightarrow$ "+'rendez-vous'+": \\ \begin{itemize} \item $t~'=t_0+T_c\,/2$ \item $\Omega~'=\Omega_a(t_0)+\pi$ \end{itemize}");
      addsep(0.7);
   }
   
   
   if (donextstep) and (stepnr==2) then {
      root.TimeSpeedFactor=10000000;
   }

   if (donextstep) and (stepnr==3) then {
      addeq('Rendez-vous succeeded'+": \\ $\Omega_b(t~')=\Omega~'+2 \pi . n$");
   }

   if (donextstep) and (stepnr==4) then {
      addeq("$\alpha_b + \omega_b ( t_0+T_C / 2 ) = \alpha_a + \omega_a t_0 + \pi + 2 \pi . n$");
   }

   if (donextstep) and (stepnr==5) then {
      addsep(-0.3);
      addeq("$t_0=C+\frac{2 \pi . n}{\omega_b-\omega_a} = C + n . \frac{T_a T_b}{T_a - T_b}$");
   }

   if (donextstep) and (stepnr==6) then {
      addeq("$\frac{T_a T_b}{T_a - T_b}=$ \textbf{"+'synodic period'+"}");
   }
   
   
   if (root.TimeSpeedFactor>1) and (root.time-starttime>TC/2*24*60*60) then {
      root.TimeSpeedFactor=0;
      ocenter.add("Curve","color":textcl,"CurveRenderType":CurveRenderDash).makeline(point(0,0,0),radial2point(1.2*AB,Astartang+Pi,0));
   }
   
   render;
}
