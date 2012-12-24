
#dframe.add(TextControl, abc );
#dframe.add( P("a",b) );
#c="ab";

#a="Size"@color(0.5,0.6,0.7);
#b="Size"@0.3;
#stop;

codeblock readtextfile(ScriptDir+"\TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.25,0.25,0.25);

#create the following as global variables
umin=-1;umax=+1;vmin=-1;vmax=+1;usteps=60;vsteps=60;

rootframe=sss.Universe;


###################### CREATE PLOT FRAME #########################

plotframe=rootframe.addsubframe("PlotFrame");
plotframe.transf.rotate(vector(1,0,0),-0.5*Pi);

##################### END CREATE PLOT FRAME ######################



###################### CREATE PLOT CONTROLS #########################

plotcontrolframe=rootframe.addscreenframe("PlotControlFrame");
plotcontrolframe.enablelight=false;
plotcontrolframe.BlendType=BlendTranslucent;
plotcontrolframe.DepthMask=DepthMaskDisable;
plotcontrolframe.color=color(1,1,1,0.5);
dyunit=0.035;
bt_surfaceedit=plotcontrolframe.add("ButtonControl","content":"Edit","position":point(0.02,0.95),"size":dyunit,"sizex":0.1);
bt_surfaceadd=plotcontrolframe.add("ButtonControl","content":"Add","position":point(0.15,0.95),"size":dyunit,"sizex":0.1);
chk_transparent=plotcontrolframe.add("CheckControl","position":point(0.02,0.85),"size":dyunit);
chk_transparent.checked=false;

###################### END CREATE PLOT CONTROLS #########################


########################### Create the dialog frame ################################
dframe=rootframe.addscreenframe("DialogFrame");
dframe.enablelight=false;
dframe.BlendType=BlendTranslucent;
dframe.DepthMask=DepthMaskDisable;
dframe.color=color(1,1,1,0.5);

dx0=0.02;
dy0=0.95;
dxl=0.7;
dyl=0.7;
dyunit=0.035;
dyunit2=dyunit*1.5;

rc1=dframe.add("FrameControl");
rc1.position=point(dx0,dy0-dyl);
rc1.SizeX=dxl;
rc1.SizeY=dyl;
rc1.color=color(0.3,0.3,0.3);

dx0=dx0+0.02;

dyp=dy0-dyunit2;
dframe.add("TextControl","Size":dyunit,"Position":point(dx0,dyp),"Content":"Plot surface");

dyp=dyp-dyunit2;
txt1=dframe.add("TextControl","Size":dyunit,"Position":point(dx0,dyp),"Content":"X=");
edx=dframe.add("EditControl","Name":"edX","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.4);
edx.content="u";

dyp=dyp-dyunit2;
txt1=dframe.add("TextControl","Size":dyunit,"Position":point(dx0,dyp),"Content":"Y=");
edy=dframe.add("EditControl","Name":"edY","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.4);
edy.content="v";

dyp=dyp-dyunit2;
txt1=dframe.add("TextControl","Size":dyunit,"Position":point(dx0,dyp),"Content":"Z=");
edz=dframe.add("EditControl","Name":"edZ","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.4);
edz.content="sin(u*u+v*v)";

#U range controls
dyp=dyp-dyunit2;
dframe.add("TextControl","Content":"U from","Size":dyunit,"Position":point(dx0,dyp));
dumin=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.12);
dumin.content="-1";
dframe.add("TextControl","Content":"to","Size":dyunit,"Position":point(dx0+0.23,dyp));
dumax=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.27,dyp),"SizeX":0.12);
dumax.content="1";
dframe.add("TextControl","Content":"steps","Size":dyunit,"Position":point(dx0+0.40,dyp));
dusteps=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.47,dyp),"SizeX":0.08);
dusteps.content="30";


#V range controls
dyp=dyp-dyunit2;
dframe.add("TextControl","Content":"V from","Size":dyunit,"Position":point(dx0,dyp));
dvmin=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.12);
dvmin.content="-1";
dframe.add("TextControl","Content":"to","Size":dyunit,"Position":point(dx0+0.23,dyp));
dvmax=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.27,dyp),"SizeX":0.12);
dvmax.content="1";
dframe.add("TextControl","Content":"steps","Size":dyunit,"Position":point(dx0+0.40,dyp));
dvsteps=dframe.add("EditControl","Size":dyunit,"position":point(dx0+0.47,dyp),"SizeX":0.08);
dvsteps.content="30";

#color control
dyp=dyp-dyunit2;
dframe.add("TextControl","Content":"Color:","Size":dyunit,"Position":point(dx0,dyp));
dcolor=dframe.add("ListControl","Size":dyunit,"position":point(dx0+0.1,dyp),"SizeX":0.2,"CountY":1);
dcolor.list.add("White");dcolor.list.add("Red");dcolor.list.add("Green");dcolor.list.add("Blue");

#error control
dyp=dyp-dyunit2*1;
err=dframe.add("TextControl","Size":dyunit,"Position":point(dx0,dyp));
err.color=color(1,0,0);

dyp=dyp-dyunit2*2;
bt1=dframe.add("ButtonControl","content":"Plot","position":point(dx0,dyp),"size":dyunit,"sizex":0.3);

######################### END CREATE DIALOG FRAME ###########################


function showplotcontrols(status)
{
   jjj=0;
   plotcontrolframe.visible=status;
   children=plotcontrolframe.getmembers;
   for i=0 to children.size-1 do
      children(i).visible=status;
}

function showdialogcontrols(status)
{
   dframe.visible=status;
   children=dframe.getmembers;
   for i=0 to children.size-1 do
      children(i).visible=status;
}

######################## GLOBAL LOOP ########################
while true do {
  
   #run the dialog frame
   showplotcontrols(false);
   showdialogcontrols(true);
   plotframe.visible=false;
   dialogcompleted=false;
   root.showcontrols=true;
   while not(dialogcompleted) do {
      incrtime;
      if bt1.WasModified then {
         dialogcompleted=true;
         try { 
            fnc=functor(edx.content,"u","v"); fnc.eval(0.55,0.55); 
            fnc=functor(edy.content,"u","v"); fnc.eval(0.55,0.55);
            fnc=functor(edz.content,"u","v"); fnc.eval(0.55,0.55);
            umin=toscalar(dumin.content);
            umax=toscalar(dumax.content);
            vmin=toscalar(dvmin.content);
            vmax=toscalar(dvmax.content);
            usteps=toscalar(dusteps.content);
            vsteps=toscalar(dvsteps.content);
         }
         if catchederror.Length>0 then { err.content=catchederror; dialogcompleted=false; }
      }
      render;
   }
   
   #start the plotting
   chk_transparent.checked=false;
   showdialogcontrols(false);
   
   
   render;
   
   plotframe.visible=true;
   showplotcontrols(true);
   
   fnc=functor("point("+edx.content+","+edy.content+","+edz.content+")","u","v");
   
   unitsize=2;
   
   #create axes
   axx=plotframe.add("Curve","Color":color(0.7,0.7,0.7));
   axx.makeline(point(0,0,0),point(unitsize,0,0));
   axx.Size=2;axx.Arrow2Size=0.1*unitsize;
   axnamex=plotframe.addviewdirframe(point(unitsize,0,0)).add("Text3D");
   axnamex.color=color(1,1,1,0.3);axnamex.BlendType=BlendTransparent;
   axnamex.enablelight=false;axnamex.UnitX=vector(0.2*unitsize,0,0);axnamex.UnitY=vector(0,0.2*unitsize,0);
   axnamex.content="X";
   
   axy=plotframe.add("Curve","Color":color(0.7,0.7,0.7));
   axy.makeline(point(0,0,0),point(0,unitsize,0));
   axy.Size=2;axy.Arrow2Size=0.1*unitsize;
   axnamey=plotframe.addviewdirframe(point(0,unitsize,0)).add("Text3D");
   axnamey.enablelight=false;axnamey.UnitX=vector(0.2*unitsize,0,0);axnamey.UnitY=vector(0,0.2*unitsize,0);
   axnamey.color=color(1,1,1,0.3);axnamey.BlendType=BlendTransparent;
   axnamey.content="Y";
   
   axz=plotframe.add("Curve","Color":color(0.7,0.7,0.7));
   axz.makeline(point(0,0,0),point(0,0,unitsize));
   axz.Size=2;axz.Arrow2Size=0.1*unitsize;
   axnamez=plotframe.addviewdirframe(point(0,0,unitsize)).add("Text3D");
   axnamez.enablelight=false;axnamez.UnitX=vector(0.2*unitsize,0,0);axnamez.UnitY=vector(0,0.2*unitsize,0);
   axnamez.color=color(1,1,1,0.3);axnamez.BlendType=BlendTransparent;
   axnamez.content="Z";
   
   
   c=plotframe.add("sphere");
   c.radius=0.1;
   c.color=color(1,0,0);
   
   surf=plotframe.add("Surface");
   surf.generate(fnc,umin,umax,usteps,vmin,vmax,vsteps);
   surf.RenderBack=true;
   surf.color=color(0.7,0.7,0.7);
   if dcolor.Selected=="Red" then surf.color=color(0.9,0.3,0.3);
   if dcolor.Selected=="Green" then surf.color=color(0.3,0.7,0.3);
   if dcolor.Selected=="Blue" then surf.color=color(0.3,0.3,0.9);
   surf.SpecularValue=30;
   surf.SpecularColor=color(0.35,0.35,0.35);
   surf.canbuffer=true;
      
      
   stopping=false;
   editcurrent=false;
   addnew=false;
   while not(stopping) do {
      incrtime;
      
      if chk_transparent.wasmodified then {
         if chk_transparent.checked then {
            surf.color.A=0.5;
            surf.BlendType=BlendTransparent;
            surf.DepthMask=DepthMaskDisable;
         } else {
            surf.color.A=1.0;
            surf.BlendType=BlendNormal;
            surf.DepthMask=DepthMaskEnable;
         }
      }
      
      if bt_surfaceedit.wasmodified then { stopping=true; editcurrent=true; }
      if bt_surfaceadd.wasmodified then { stopping=true; addnew=true; }
      #keep light position fixed
      viewdir_X=vecnorm(myviewport.CameraUpDir*myviewport.CameraDir);
      viewdir_Y=vecnorm(viewdir_X*myviewport.CameraDir);
      viewdir_Z=myviewport.CameraDir;
      sss.light0pos=@point(1000*viewdir_X-1000*viewdir_Y-500*viewdir_Z);
      render;
   }
   
   if editcurrent then surf.dispose;

}
######################## END GLOBAL LOOP ########################