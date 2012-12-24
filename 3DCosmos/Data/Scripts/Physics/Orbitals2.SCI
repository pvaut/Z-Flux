codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


#function density(xc,yc,zc)
#{
#   vl=exp(-0.5*sqrt(xc*xc+yc*yc+zc*zc))*xc;
#   return(vl);
#}

function density(xc,yc,zc)
{
   rd=sqrt(xc*xc+yc*yc+zc*zc);
   f1=(6-rd)*exp(-0.5*rd);
   vl=f1*(2*sqr(zc)-(sqr(xc)+sqr(yc)));
   return(vl);
}

function denscolor(pt)
{
   rd=sqrt(sqr(pt.x)+sqr(pt.y)+sqr(pt.z));
   f1=(6-rd)*exp(-0.5*rd);
   vl=f1*(2*sqr(pt.z)-(sqr(pt.x)+sqr(pt.y)));
   cl=color(min(1,max(0,0.5*vl)),0,min(1,max(0,-0.5*vl)),0.02*abs(vl));
   return(cl);
}

#break;
#cl=denscolor(point(1,2,3));

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);
refframe=sss.addsubframe("refframe");


fnc=FunctionFunctor("denscolor","pt");


rres=30;
rfc=15/rres;

if false then {
   for i=1 to rres do {
      rd=rfc*(rres-1-i);
      s=Sphere(point(0,0,0),rd,20);
      
      obj=refframe.add("SolidObject");
      obj.CreateShape(s);
      
      obj.GenerateVertexProperty(fnc,VertexPropertyColor);
      obj.enablelight=false;
      obj.BlendType=BlendTransLucent;
      obj.DepthMask=DepthMaskDisable;
      obj.RenderFront=false;
      obj.RenderBack=true;
   }
}

for i=1 to rres do {
   rd=rfc*i;
   s=Sphere(point(0,0,0),rd,20);
   
   obj=refframe.add("SolidObject");
   obj.CreateShape(s);
   
   obj.GenerateVertexProperty(fnc,VertexPropertyColor);
   obj.enablelight=false;
   obj.BlendType=BlendTransLucent;
   obj.DepthMask=DepthMaskDisable;
#   obj.RenderBack=true;
}

#refframe.addclipplane(plane(1,0,0,0));

vp=T_getviewport;
vp.FocalDistance=10;
vp.NearClipPlane=0.1;
vp.FarClipPlane=100;

   
while true do render;
