

codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


###############################################################################################
# INIT SCENE AND VIEWPORT
###############################################################################################

sf=T_scene_create;
sss=T_getscene;

vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;
vp.enableusernavigation=false;
vp.EnableUserStop=true;


###############################################################################################
# PROMPT USER
###############################################################################################


slideshowfilename="";
if isdefined(ScriptArgument) then 
   if ScriptArgument.Length>0 then
      slideshowfilename=ScriptArgument;
if slideshowfilename.length<=0 then {
   
   slideshowlist=GetFileList(datadir+"\SlideShows\*.sci");
   
   menu=T_createmenu("Select slide show",true);
   menu.sizex=0.35;
   root.SC.Universe.MenuFrame.Color=color(1,1,1,1);
   
   vp.ShowControls=true;
   
   foreach slideshowfilename in slideshowlist do {
      slideshowname=slideshowfilename;slideshowname.Invert;slideshowname.split(".");slideshowname.Invert;
      menu.Add("",slideshowname,slideshowfilename);
   }
   T_menu_appendclose;
   
   
   menufinished=false;
   while not(menufinished) do {
      if menu.WasModified then {
         menufinished=true;
         currentfolder=menu.SelectID;
      }
      render;
   }
   slideshowfilename=datadir+"\SlideShows\"+menu.SelectID;
   if menu.SelectID=="[Close]" then stop;
   T_delmenu;
}







###########################################################################
# action functions definitions
###########################################################################

actions=list;
attime=0;

function picturefolder(foldername)
{
   act=map;
   act.AtTime=attime;
   act.ID="PictureFolder";
   act.FolderName=foldername;
   actions.add(act);
}

function videofolder(foldername)
{
   act=map;
   act.AtTime=attime;
   act.ID="VideoFolder";
   act.FolderName=foldername;
   actions.add(act);
}


function transition(tpe,delay)
{
   act=map;
   act.AtTime=attime;
   act.ID="Transition";
   act.Tpe=tpe;
   act.Delay=delay;
   actions.add(act);
}

function showstereopic(imagename,exarg1,exarg2,exarg3,exarg4,exarg5)
{
   act=map;
   act.AtTime=attime;
   act.ID="ShowStereoPic";
   act.ImageName=imagename;
   exargs=map;
   if isvardefined("exarg1") then exargs.additem(exarg1);
   if isvardefined("exarg2") then exargs.additem(exarg2);
   if isvardefined("exarg3") then exargs.additem(exarg3);
   if isvardefined("exarg4") then exargs.additem(exarg4);
   if isvardefined("exarg5") then exargs.additem(exarg5);
   act.exargs=exargs;
   actions.add(act);
}

function panstereopic(imagename,exarg1,exarg2,exarg3,exarg4,exarg5)
{
   act=map;
   act.AtTime=attime;
   act.ID="PanStereoPic";
   act.ImageName=imagename;
   exargs=map;
   if isvardefined("exarg1") then exargs.additem(exarg1);
   if isvardefined("exarg2") then exargs.additem(exarg2);
   act.exargs=exargs;
   actions.add(act);
}


function hidestereopic(imagename)
{
   act=map;
   act.AtTime=attime;
   act.ID="HideStereoPic";
   act.ImageName=imagename;
   actions.add(act);
}

function AutoHidePrevious(newstatus)
{
   act=map;
   act.AtTime=attime;
   act.ID="AutoHidePrevious";
   act.NewStatus=newstatus;
   actions.add(act);
}

function showstereovideo(filename)
{
   act=map;
   act.AtTime=attime;
   act.ID="ShowStereoVideo";
   act.FileName=filename;
   actions.add(act);
}


function showtext(content,posit,exarg1,exarg2,exarg3,exarg4,exarg5)
{
   act=map;
   act.AtTime=attime;
   act.ID="ShowText";
   act.Content=content;
   act.Posit=posit;
   exargs=map;
   if isvardefined("exarg1") then exargs.additem(exarg1);
   if isvardefined("exarg2") then exargs.additem(exarg2);
   if isvardefined("exarg3") then exargs.additem(exarg3);
   if isvardefined("exarg4") then exargs.additem(exarg4);
   if isvardefined("exarg5") then exargs.additem(exarg5);
   act.exargs=exargs;
   actions.add(act);
}


function SoundFolder(folder)
{
   act=map;
   act.AtTime=attime;
   act.ID="SoundFolder";
   act.FolderName=folder;
   actions.add(act);
}

function PlaySound(filename,volume)
{
   act=map;
   act.AtTime=attime;
   act.ID="PlaySound";
   act.FileName=filename;
   act.Volume=500;
   if isvardefined("volume") then act.Volume=volume;
   actions.add(act);
}

function StopSound(filename)
{
   act=map;
   act.AtTime=attime;
   act.ID="StopSound";
   act.FileName=filename;
   actions.add(act);
}

function FadeSound(filename,newvol,durat)
{
   act=map;
   act.AtTime=attime;
   act.ID="FadeSound";
   act.FileName=filename;
   act.NewVol=newvol;
   act.Durat=durat;
   actions.add(act);
}


function delay(seconds)
{
   act=map;
   act.AtTime=attime;
   act.ID="Delay";
   act.Seconds=seconds;
   actions.add(act);
   attime=attime+seconds;
}

function waituser()
{
   act=map;
   act.AtTime=attime;
   act.ID="WaitUser";
   actions.add(act);
}

function loop()
{
   act=map;
   act.AtTime=attime;
   act.ID="Loop";
   act.Durat=attime;
   actions.add(act);
}

function end()
{
   act=map;
   act.AtTime=attime;
   act.ID="End";
   act.Durat=attime;
   actions.add(act);
}


###########################################################################"
# Here comes the scenario writeout

codeblock readtextfile(slideshowfilename);

###########################################################################
# RENDER VARIABLE DEFINITIONS
###########################################################################

PictureFolder=slideshowfilename;
PictureFolder.invert;PictureFolder.split(".");PictureFolder.invert;
SoundFolder=PictureFolder;
VideoFolder=PictureFolder;

extension="jpg";
activetransition="fade";
transitionspeed=1.5;
autohideprevious=true;

activepics=list;
rootframe=T_getrootframe;
screensizey=2*tan(vp.aperture/2);
screensizex=screensizey*vp.aspectratio;

activesounds=list;

curpicfactor=1;

###########################################################################
# ACTION FUNCTION IMPLEMENTATIONS
###########################################################################

function Exec_ShowStereoPic(args)
{
   actpic=map;
   actpic.ImageName=args.ImageName;
   actpic.FileName=PictureFolder+"\"+actpic.ImageName+"."+extension;
   actpic.Frame=rootframe.addsubframe(actpic.ImageName);
   actpic.StopAt=9999999999;
   
   sizefactor=1;
   if args.exargs.IsPresent("Size") then sizefactor=args.exargs.Size;
   pos=point(0,0,0);
   if args.exargs.IsPresent("Position") then pos=args.exargs.Position;
   
   bmp=loadbitmap(actpic.FileName);
   xr=bmp.xres/2;
   yr=bmp.yres;
   if (xr==0) or (yr==0) then throw("Invalid or absent file "+actpic.FileName);
   bmp1=bmp.crop(0,0,xr,yr);
   bmp2=bmp.crop(xr,0,xr,yr);
   
   frx=vp.aspectratio/xr;
   fry=1/yr;
   fr=min(frx,fry);
   fr=fr*screensizey;
      
   tx1=actpic.Frame.CreateBitmapTexture("tx1",bmp1);
   tx2=actpic.Frame.CreateBitmapTexture("tx2",bmp2);
   
   actpic.posit=pos;
   actpic.sizex=fr*xr*sizefactor;
   actpic.sizey=fr*yr*sizefactor;
   
   actpic.Frame.add("Rectangle","Name":"REC","Axis1":vector(actpic.sizex,0,0),"Axis2":vector(0,actpic.sizey,0),
      "position":point(pos.x-0.5*actpic.sizex,pos.y-0.5*actpic.sizey,-0.0001-pos.z),"EnableLight":false,
      "Texture":tx1.name,
      "TextureRight":tx2.name,
      "color":color(1,1,1),
      "BlendType":BlendTransLucent,
      "DepthMask":DepthMaskDisable);

   actpic.StartAt=timer.elapsed;
   actpic.dopanning=false;

   if autohideprevious then
      foreach pic in activepics do pic.StopAt=timer.elapsed;
   
   activepics.add(actpic);
   curpicframe=actpic.frame;
   curpicfactor=actpic.sizex;
}



function Exec_PanStereoPic(args)
{
   durat=1;
   if args.exargs.IsPresent("Duration") then durat=args.exargs.Duration;
   destpos=point(0,0,0);
   if args.exargs.IsPresent("Position") then destpos=args.exargs.Position;
   
   foreach pic in activepics do 
      if pic.ImageName==args.ImageName then {
         pic.dopanning=true;
         pic.panningstarted=timer.elapsed;
         pic.panningduration=durat;
         pic.panningpositfrom=pic.posit;
         pic.panningpositto=destpos;
      }
}





function Exec_HideStereoPic(args)
{
   foreach pic in activepics do 
      if pic.ImageName==args.ImageName then pic.StopAt=timer.elapsed;
}

function Exec_AutoHidePrevious(args)
{
   autohideprevious=args.NewStatus;
}

function Exec_loop(args)
{
   nextaction=0;
   foreach action in actions do 
      action.AtTime=action.AtTime+args.durat;
}



function Exec_ShowText(args)
{
#   curpicfactor=1;
   output("exec_showtext "+str(curpicfactor));
   textsize=curpicfactor;
   if args.exargs.IsPresent("Size") then textsize=curpicfactor*args.exargs.Size;
   textcolor=color(1,1,1);
   if args.exargs.IsPresent("Color") then textcolor=args.exargs.Color;
   
   psit=point(curpicfactor*args.posit.x,curpicfactor*args.posit.y,args.posit.z);
   
   curpicframe.add("Text3D","Content":args.content,"Position":(psit+vector(0.05*textsize,-0.05*textsize,0.001)),
        "UnitX":vector(textsize,0,0),"UnitY":vector(0,textsize,0),"EnableLight":false,
        "BlendType":BlendTransLucent,
        "DepthMask":DepthMaskDisable,
        "Color":color(0,0,0));
   curpicframe.add("Text3D","Content":args.content,"Position":(psit+vector(0,0,0.002)),
        "UnitX":vector(textsize,0,0),"UnitY":vector(0,textsize,0),"EnableLight":false,
        "BlendType":BlendTransLucent,
        "DepthMask":DepthMaskDisable,
        "Color":textcolor);
}


function Exec_PlaySound(args)
{
   snd=addsound(SoundFolder+"\"+args.FileName);
   snd.custom.filename=args.FileName;
   activesounds.add(snd);
   snd.SetVolume(args.Volume);
   snd.Start;
}

function Exec_StopSound(args)
{
   snd=findactivesound(args.FileName);
   snd.dispose;
}

function Exec_FadeSound(args)
{
   snd=findactivesound(args.FileName);
   snd.FadeVolume(args.NewVol,args.Durat);
}


function Exec_ShowStereoVideo(args)
{
   actvid=map;
   actvid.ImageName=args.FileName;
   FileName=VideoFolder+"\"+args.FileName;
   filename.replace("/","\");
   actvid.Frame=rootframe.addsubframe(FileName);
   
   soundfilename=filename+".mp3";
   
   vdleft=addvideo(filename+"_L.avi");
   vdright=addvideo(filename+"_R.avi");
   
   vdxres=vdleft.GetXRes;
   vdyres=vdleft.GetYRes;
   vdframerate=round(vdleft.GetFrameRate);
   vdframecount=vdleft.GetFrameCount;
   
   frx=vp.aspectratio/vdxres;
   fry=1/vdyres;
   fr=min(frx,fry);
   fr=fr*screensizey;
   
   sizex=fr*vdxres;
   sizey=fr*vdyres;
   
   
   vdrc=actvid.frame.add("Rectangle","Name":"VidREC","Axis1":vector(sizex,0,0),"Axis2":vector(0,sizey,0),
         "position":point(0-0.5*sizex,0-0.5*sizey,0),"EnableLight":false);
   
   txl=actvid.frame.CreateVideoTexture("left",vdleft);
   txr=actvid.frame.CreateVideoTexture("right",vdright);
   vdrc.Texture="left";
   vdrc.TextureRight="right";
   
   oldframerate=root.FrameRate;
   root.FrameRate=vdframerate;
   
   #Fade in
   for i=0 to 30 do {
      vp.FadeColor=color(0,0,0,1-i/30);
      render;
   }
   
   #Start sound file
   createvar(snd);
   soundpresent=false;
   if FileIsPresent(soundfilename) then {
      snd=addsound(soundfilename);
      snd.Start;
      soundpresent=true;
   }
   
   #Play video
   frnr=0;
   while frnr<vdframecount do {
      incrtime;
      vdleft.CurrentFrame=frnr;
      vdright.CurrentFrame=frnr;
      frnr=frnr+1;
      render;
   }
   
   #fade out
   for i=0 to 30 do {
      vp.FadeColor=color(0,0,0,i/30);
      render;
   }
   
   root.FrameRate=oldframerate;
   
   actvid.frame.dispose;
   vdleft.dispose;
   vdright.dispose;
   if soundpresent then {
      snd.stop;
      snd.dispose;
   }
   vp.fadecolor=color(0,0,0,0);
}

function Exec_WaitUser()
{
   aaa=0;
}

############################################################################################
# SOME INTERNAL FUNCTIONS
############################################################################################

function findactivesound(filename)
{
   foreach snd in root.sounds.getmembers do
      if snd.custom.filename==filename then return(snd);
   throw("Invalid active sound "+filename);
}


function apply_appear(subframe,frac)
{
   frac=max(frac,0);
   frac=min(frac,1);
   
   if activetransition=="Curtain" then {
      subframe.transf.reset;
      subframe.transf.rotate(vector(0,1,0),(frac-1)*Pi/2);
      subframe.transf.origin=point((frac-1)*screensizex,0,0.5*sin((frac-1)*Pi/2));
      return(0);
   }
   
   if activetransition=="Pan" then {
      subframe.transf.reset;
      subframe.transf.origin=point((frac-1)*screensizex,0,0);
      return(0);
   }

   
   if activetransition=="Transient" then {
      subframe.Color=color(1,1,1,frac);
      return(0);
   }

   if activetransition=="Zoom" then {
      subframe.getparent.moveobject(subframe.name,-999);
      subframe.REC.Color=color(frac,frac,frac,frac);
      frac2=1-frac;frac2=sqr(frac2);
      subframe.transf.origin=point(0,0,-15*frac2);
      return(0);
   }
   
   
   frac=min(1,2*frac-1);
   subframe.Color=color(1,1,1,frac);
}


function apply_disappear(subframe,frac)
{
   frac=max(frac,0);
   frac=min(frac,1);
   frac=frac;
   
   if activetransition=="Curtain" then {
      subframe.transf.reset;
      subframe.transf.rotate(vector(0,1,0),frac*Pi/2);
      subframe.transf.origin=point(frac*screensizex,0,-0.5*sin(frac*Pi/2));
      return(0);
   }
   
   if activetransition=="Pan" then {
      subframe.transf.reset;
      subframe.transf.origin=point((frac)*screensizex,0,0);
      return(0);
   }

   if activetransition=="Transient" then {
      subframe.Color=color(1,1,1,1-frac);
      return(0);
   }

   if activetransition=="Zoom" then {
      subframe.REC.Color=color(1,1,1,1-frac);
      subframe.transf.origin=point(0,0,0.9*frac);
      return(0);
   }

   frac=min(1,2*frac); 
   subframe.Color=color(1,1,1,1-frac);
   
}


function sweep_appear()
{
   foreach pic in activepics do {
      if pic.StartAt+transitionSpeed>timer.elapsed-1/10 then {
         apply_appear(pic.Frame,(timer.elapsed-pic.StartAt)/transitionspeed);
      }
   }
}


function sweep_disappear()
{
   picnr=0;
   while picnr<activepics.size do {
      pic=activepics(picnr);
      if pic.StopAt<timer.elapsed then {
         apply_disappear(pic.Frame,(timer.elapsed-pic.StopAt)/transitionspeed);
         if pic.StopAt+transitionspeed<timer.elapsed then {
            pic.Frame.dispose;
            activepics.del(picnr);picnr=picnr-1;
         }
      }
      picnr=picnr+1;
   }
}


function sweep_pan()
{
   foreach pic in activepics do {
      if pic.dopanning then {
         fr=(timer.elapsed-pic.panningstarted)/pic.panningduration;
         if (fr>=0) and (fr<=1) then {
            fr=1-(1-fr)*(1-fr);
            fr=fr*fr;
            pic.posit=pic.panningpositfrom+fr*(pic.panningpositto-pic.panningpositfrom);
            pic.Frame.REC.position=point(pic.posit.x-0.5*pic.sizex,pic.posit.y-0.5*pic.sizey,-0.0001-pic.posit.z);
         } else {
            pic.dopanning=false;
         }
      }
   }
}



###########################################################################
# THE RENDER LOOP
###########################################################################

timer=chrono;
nextaction=0;
createvar(curpicframe);


while nextaction<actions.size do {
   if actions(nextaction).attime<timer.elapsed then {
      actID=actions(nextaction).ID;
      timer.pauze;
      
      if actID=="PictureFolder" then PictureFolder=actions(nextaction).Foldername;
      if actID=="SoundFolder" then SoundFolder=actions(nextaction).Foldername;
      if actID=="VideoFolder" then VideoFolder=actions(nextaction).Foldername;
      if actID=="Transition" then {
         activetransition=actions(nextaction).tpe;
         transitionspeed=actions(nextaction).delay;
      }
      if actID=="Loop" then Exec_loop(actions(nextaction));
      if actID=="ShowStereoPic" then Exec_ShowStereoPic(actions(nextaction));
      if actID=="PanStereoPic" then Exec_PanStereoPic(actions(nextaction));
      if actID=="HideStereoPic" then Exec_HideStereoPic(actions(nextaction));
      if actID=="AutoHidePrevious" then Exec_AutoHidePrevious(actions(nextaction));

      if actID=="ShowStereoVideo" then Exec_ShowStereoVideo(actions(nextaction));

      if actID=="ShowText" then Exec_ShowText(actions(nextaction));
      
      if actID=="PlaySound" then Exec_PlaySound(actions(nextaction));
      if actID=="StopSound" then Exec_StopSound(actions(nextaction));
      if actID=="FadeSound" then Exec_FadeSound(actions(nextaction));

      if actID=="WaitUser" then Exec_WaitUser;

      timer.Resume;
      nextaction=nextaction+1;
   }
   
   if UIIsKeyDown("return") or UIIsLeftMouseButtonDown or JoystickButtonClicked(0,4) or NavigatorPressing then {
      while UIIsKeyDown("return") or UIIsLeftMouseButtonDown or JoystickButtonClicked(0,4) do {
         render;
      }
      if nextaction<actions.size then {
         timer.set(actions(nextaction).attime);
      }
   }

   if UIIsKeyDown("escape") or RightMouseClicked or NavigatorPulling or JoystickButtonClicked(0,6) then {
      while UIIsKeyDown("escape") do {
         render;
      }
      stop;
   }

   
   sweep_appear;
   sweep_disappear;
   sweep_pan;
   
   render;
}

resetallsounds;
