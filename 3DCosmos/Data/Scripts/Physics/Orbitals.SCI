codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


#function density(xc,yc,zc)
#{
#   vl=exp(-0.5*sqrt(xc*xc+yc*yc+zc*zc))*xc;
#   return(vl);
#}

function density(xc,yc,zc)
{
   rd=sqrt(xc*xc+yc*yc+zc*zc);
   return(  (6-rd)*exp(-0.5*rd) * (2*sqr(zc)-(sqr(xc)+sqr(yc)))  );
}

#function density(xc,yc,zc)
#{
#   return(xc);
#}


sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);
refframe=sss.addsubframe("refframe");

if false then {
   fnc=functor("exp(-0.5*sqrt(x*x+y*y+z*z))*x-0.4","x","y","z");
   s1=CreateSurface(fnc,0.25,5);
   obj=refframe.add("SolidObject");
   obj.CreateShape(s1);
   obj.renderback=true;
   obj.setcolor(0,color(0.4,0.4,0.8));
   obj.SpecularValue=30;
   obj.SpecularColor=color(0.35,0.35,0.35);
}

tx=refframe.createtexture("star",DataDir+"\textures\star2.bmp",DataDir+"\textures\star2.bmp");
pe=refframe.add("PEngine");

pe.HasParticleColors=true;
#pe.BlendType=BlendNormal;pe.DepthMask=DepthMaskNormal;
pe.BlendType=BlendTransparent;pe.DepthMask=DepthMaskDisable;
#pe.BlendType=BlendTransLucent;pe.DepthMask=DepthMaskDisable;pe.sort=true;
pe.pointsize=8;
pe.PEngineRenderType=PengineRenderPoints;
pe.texture=tx.name;
   
count=100000;
pe.setsize(count);

bbox=24;
ct=0;
createvar(x);createvar(y);createvar(z);
createvar(rd);createvar(d);

fnc=PolynomialFunctor("x","y","z");
fnc.AddPolynomialComponent(-1,2,0,0);
fnc.AddPolynomialComponent(-1,0,2,0);
fnc.AddPolynomialComponent(2,0,0,2);

while ct<count do {
   x=(2*random-1)*bbox;
   y=(2*random-1)*bbox;
   z=(2*random-1)*bbox;
#   d=density(x*fc,y*fc,z*fc);
   
   rd=sqrt(x*x+y*y+z*z);
   d=0.3*  (6-rd)*exp(-0.5*rd) * fnc.eval(x,y,z)  ;

   rd=random;
   if d-0.1>rd then {
      pe.Pposition(ct)=point(x,y,z);
      pe.Pcolor(ct)=color(0.8,0.15,0,0.3*min(1,d));
      ct=ct+1;
   }
   if -1*d-0.1>rd then {
      pe.Pposition(ct)=point(x,y,z);
      pe.Pcolor(ct)=color(0,0.15,0.8,0.3*min(1,-1*d));
      ct=ct+1;
   }
}

#refframe.addclipplane(plane(1,0,0,0));

vp=T_getviewport;
vp.FocalDistance=bbox*2;
vp.NearClipPlane=0.1;
vp.FarClipPlane=bbox*10;
vp.camerapos=point(2*bbox,0,0);
vp.cameradir=vector(-1,0,0);
   
while true do render;
