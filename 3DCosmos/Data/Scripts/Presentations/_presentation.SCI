
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

function SetViewportPresentParams()
{
   vp=T_getviewport;
   vp.CameraPos=point(0,0,10);
   vp.CameraDir=vecnorm(point(0,0,0)-vp.camerapos);
   vp.CameraUpDir=vector(0,1,0);
   vp.FocalDistance=10;
   vp.NearClipPlane=0.1;
   vp.FarClipPlane=200;
   vp.enableusernavigation=true;
   vp.EnableUserStop=false;
}

#Can be "None",""MoveForward","
CurrentAnimation="MoveForward";
CurrentAnimationSpeed=1.0;

rootframe=T_scene_create_named("PSC");

sss=root.PSC;

SetViewportPresentParams();
vp=T_getviewport;
vp.EnableUserStop=false;

xyratio=vp.aspectratio;


sss.ambientlightcolor=color(0.3,0.3,0.3);

backframe=rootframe.AddSubFrame("BackFrame");

createvar(lastitemframe);

cint=0.3;

origz=-4;

origx=-7;
origy=-2.5;
for cnr=0 to 20 do {
  cv=backframe.add("Curve","EnableLight":false);
  cv.Size=1;
  cv.BlendType=BlendTransparent;
  fr=1+cnr/20;
  for i=0 to 120 do {
     vl=i/10;
     pt=point(xyratio*(origx+vl+2*sin(vl)),origy+vl+sin(fr*vl),origz+2*sin(vl)*cos(fr*vl));
     cl=color(cint*(0.5+0.5*sin(vl)),cint*(0.5-0.5*cos(vl)),cint*0.5);
     cv.addpoint(pt,cl);
  }
}

origx=-7.5;
origy=-3.5;
for cnr=0 to 20 do {
  cv=backframe.add("Curve","EnableLight":false);
  cv.Size=1;
  cv.BlendType=BlendTransparent;
  fr=1+cnr/20;
  for i=0 to 120 do {
     vl=i/10;
     pt=point(xyratio*(origx+2*vl+2*sin(vl)),origy+vl+cos(fr*vl)-1,origz+2*sin(fr*vl)*cos(vl));
     cl=color(cint*(0.5+0.5*sin(vl)),cint*(0.5-0.5*cos(vl)),cint*0.5);
     cv.addpoint(pt,cl);
  }
}


origx=-2;
origy=-6;
for cnr=0 to 20 do {
  cv=backframe.add("Curve","EnableLight":false);
  cv.Size=1;
  cv.BlendType=BlendTransparent;
  fr=1+cnr/20;
  for i=0 to 120 do {
     vl=i/10;
     pt=point(xyratio*(origx+1.2*vl+cos(2*vl)),origy+vl+cos(fr*vl)-1,origz+2*sin(fr*vl)*cos(vl));
     cl=color(cint*(0.5+0.5*sin(vl)),cint*(0.5-0.5*cos(vl)),cint*0.5);
     cv.addpoint(pt,cl);
  }
}


vpsizex=2*tan(vp.Aperture/2)*distance(vp.camerapos,point(0,0,0))*xyratio;#viewport size in x dir
vpsizey=2*tan(vp.Aperture/2)*distance(vp.camerapos,point(0,0,0));#viewport size in y dir

slidepresent=false;
createvar(slideframe);
createvar(origx);
createvar(origy);
createvar(posy);
createvar(indents);
createvar(textsize);createvar(leftmargin);createvar(rightmargin);createvar(topmargin);

textcolor=color(0.6,0.6,0.6);
titlecolor=color(0.8,0.7,0.6);

function AddItemFrame(posx,posy)
{
   fr=slideframe.AddSubFrame("ItemFrame");
   fr.Transf.origin=point(posx,posy,-999);
   fr.custom.posx=posx;
   fr.custom.posy=posy;
   fr.custom.animfrac=0;
   fr.custom.animtype=CurrentAnimation;
   fr.custom.animspeed=CurrentAnimationSpeed;
   return(fr);
}


function SetAnimation(tpe)
{
   CurrentAnimation=tpe;
}

function SetAnimationSpeed(spd)
{
   CurrentAnimationSpeed=spd;
}


# Options:
# "Bullet":true
# "Bold":true
# "Italic":true
# "Color":color
# "Size":sizefactor
function text(content,m1,m2,m3,m4,m5,m6,m7,m8)
{
   options=map;
   if isvardefined("m1") then options.AddItem(m1);
   if isvardefined("m2") then options.AddItem(m2);
   if isvardefined("m3") then options.AddItem(m3);
   if isvardefined("m4") then options.AddItem(m4);
   if isvardefined("m5") then options.AddItem(m5);
   if isvardefined("m6") then options.AddItem(m6);
   if isvardefined("m7") then options.AddItem(m7);
   if isvardefined("m8") then options.AddItem(m8);
   
   strcontent=content;

   if options.IsPresent("Italic") then
      if options.Bold then
         strcontent="\textit{"+strcontent+"}";

   if options.IsPresent("Bold") then
      if options.Bold then
         strcontent="\textbf{"+strcontent+"}";
   
   if options.IsPresent("Bullet") then
      if options.Bullet then
         strcontent="\begin{itemize}~n\item "+strcontent+"~n\end{itemize}";
         
   mycolor=textcolor;
   if options.IsPresent("Color") then
      mycolor=options.Color;

   sizefactor=1;
   if options.IsPresent("Size") then
      sizefactor=options.Size;

   itemframe=AddItemFrame(origx+leftmargin*vpsizex,origy-posy);
   txt=itemframe.add("FormattedText",
      "MaxLenX":((1-leftmargin-rightmargin)*vpsizex),
      "Size":(textsize*sizefactor),"position":point(0,0),
      "EnableLight":false,);
   txt.Content=strcontent;
   
   #dry-run style rendering to determine y size
   txt.color=color(1,1,1,0);txt.BlendType=BlendTransparent;txt.DepthMask=DepthMaskDisable;
   render;
   txt.color=mycolor;txt.BlendType=BlendNormal;txt.DepthMask=DepthMaskNormal;
   posy=posy+txt.GetSizeY+0.3*textsize;
}

function GetCurPosY()
{
   return(posy);
}

function indent(value)
{
   indents.add(value);
   leftmargin=leftmargin+value;
}

function outdent()
{
   if indents.size<=0 then throw("No more indents to outdent");
   leftmargin=leftmargin-indents(indents.size-1);
   indents.del(indents.size-1);
}

function skipy(value)
{
   posy=posy+value;
}

function picture(filename,posit,sizey,m1,m2,m3,m4,m5,m6,m7,m8)
{
   options=map;
   if isvardefined("m1") then options.AddItem(m1);
   if isvardefined("m2") then options.AddItem(m2);
   if isvardefined("m3") then options.AddItem(m3);
   if isvardefined("m4") then options.AddItem(m4);
   if isvardefined("m5") then options.AddItem(m5);
   if isvardefined("m6") then options.AddItem(m6);
   if isvardefined("m7") then options.AddItem(m7);
   if isvardefined("m8") then options.AddItem(m8);
   
   isstereo=false;
   if options.IsPresent("Stereo") then
      isstereo=options.Stereo;

   itemframe=AddItemFrame(origx+leftmargin*vpsizex+posit.x,origy-posit.y);
   bmp=loadbitmap(filename);
   xr=bmp.xres;
   yr=bmp.yres;
   if isstereo then
      xr=bmp.xres/2;

   sizex=sizey/yr*xr;
   
   b=itemframe.add("Rectangle","Axis1":vector(sizex,0,0),"Axis2":vector(0,sizey,0),
      "position":point(0,-1*sizey,posit.z),"EnableLight":false,"color":color(1,1,1),"RenderBack":true);
      
   if isstereo then {
      bmp1=bmp.crop(0,0,xr,yr);
      bmp2=bmp.crop(xr,0,xr,yr);
      tx1=itemframe.CreateBitmapTexture("tx1",bmp1);
      tx2=itemframe.CreateBitmapTexture("tx2",bmp2);
      b.Texture=tx1.name;
      b.TextureRight=tx2.name;
   } else {
      tx=itemframe.CreateBitmapTexture("tx",bmp);
      b.Texture=tx.name;
   }
      
      
   lastitemframe=itemframe;
}




function Slide(title)
{
   if slidepresent then {
      for i=0 to 150 do {
         slideframe.Transf.translate(vector(0,-0.1,-0.08));
         slideframe.Transf.rotate(vector(0,1,-0.2),0.015);
         slideframe.Transf.scale(0.995);
         render;
      }
      slideframe.dispose;
   }
   slideframe=rootframe.addsubframe("Slide");
   slidepresent=true;
   
   textsize=vpsizey/20;
   leftmargin=0.1;#in fraction of vpsizex;
   rightmargin=0.1;#in fraction of vpsizex;
   topmargin=0.05;#in fraction of vpsizey;

   origx=-0.5*vpsizex;
   origy= 0.5*vpsizey;
   posy=topmargin*vpsizey;
   indents=list;
   
   Text(title,"Size":1.4,"Bold":true,"Color":titlecolor);
   SetViewportPresentParams;
}




function AnimateStep()
{
   foreach subframe in sss.Universe.Slide.getmembers do {
      frac=subframe.custom.animfrac+0.02*subframe.custom.animspeed;
      if frac>1 then frac=1;
      subframe.custom.animfrac=frac;
      fr2=1-frac;
      if subframe.custom.animtype=="MoveForward" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,-18*fr2*fr2);
      }
      if subframe.custom.animtype=="None" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,0);
      }
      if subframe.custom.animtype=="FadeIn" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,0);
         subframe.color=color(1,1,1,frac);
         subframe.BlendType=BlendTranslucent;
      }
      if subframe.custom.animtype=="MoveBackWard" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,6*fr2*fr2);
         subframe.color=color(1,1,1,frac);
         subframe.BlendType=BlendTranslucent;
      }
      if subframe.custom.animtype=="ZoomX" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,0);
         subframe.transf.Xaxis=vector(frac,0,0);
      }
      if subframe.custom.animtype=="ZoomRotate" then {
         subframe.transf.origin=point(subframe.custom.posx,subframe.custom.posy,-5*fr2*fr2);
         ang=Pi*fr2;
         subframe.transf.Yaxis=vector(0,cos(ang),sin(ang));
         subframe.transf.Zaxis=vector(0,-1*sin(ang),cos(ang));
      }
   }
   render;
}


function Sleep(elapse)
{
   chr=Chrono;chr.start;
   while chr.Elapsed<elapse do {
      AnimateStep;
   }
}

function Click()
{
   clicked=false;
   while not(clicked) do {
      AnimateStep;
      if abs(JoystickRockerPos(0)-90)<1 then {
         while abs(JoystickRockerPos(0)-90)<1 do AnimateStep;
         clicked=true;
      }
      if abs(JoystickRockerPos(0)-180)<1 then {
         while abs(JoystickRockerPos(0)-180)<1 do AnimateStep;
         clicked=true;
      }
      if UIGetKeyPressed=="Return" then clicked=true;
      if LeftMouseClicked then clicked=true;
   }
}

function PresentScript(scriptfile,arg)
{
   fadeout(0.5);
   root.Viewports.main.fadecolor=color(0,0,0,0);
   ExecuteScript(scriptfile,arg);
   fadeout(0.5);
   vp=root.Viewports.main;
   vp.setscene(root.PSC);
   SetViewportPresentParams();
   fadein(0.5);
}

function RemoveLastItem()
{
   lst=root.PSC.Universe.Slide.getmembers;
   lst(lst.size-1).dispose;
}
