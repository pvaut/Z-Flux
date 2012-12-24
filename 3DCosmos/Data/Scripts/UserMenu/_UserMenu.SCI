codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

timeout=60;
idletimer=Chrono;


thbsize=0.2;
selpsy=0;
selnr=0;
createvar(vp);createvar(selframe);createvar(isinsub);

function CheckStop()
{
   stoptimer=Chrono;
   while UIIsLeftMouseButtonDown and UIIsRightMouseButtonDown do {
      render;
      if stoptimer.Elapsed>5 then ShutdownComputer;
   }
}


# INIT SCENE AND VIEWPORT
function initscene()
{
   sf=T_scene_create;
   sss=T_getscene;
   
   vp=T_getviewport;
   vp.CameraPos=point(0,0,1);
   vp.CameraDir=vector(0,0,-1);
   vp.FocalDistance=1;
   vp.NearClipPlane=0.1;
   vp.FarClipPlane=20;
   vp.enableusernavigation=false;
   vp.EnableUserStop=false;
#   vp.UserNavigationAxisLevelRotate=2;
#   vp.UserNavigationAxisLevelTime=0;
   root.MouseDampingFactor=0.9;
   
   #build scene
   thumbnailsframe=sss.Universe.addsubframe("ThumbnailsFrame");
   thumbnailsframe.transf.origin=point(-2*thbsize,0);
   y0=0;
   foreach item in folders do {
      frame=thumbnailsframe.addsubframe("");
      frame.transf.origin=point(0,y0);
   
      if item.thumbnailfile.Length>0 then {
         thethumbnailfile=item.thumbnailfile;
         thethumbnailfile.Replace("[CurrentFolder]",item.folder);
         bmpthumbnail=LoadBitmap(thethumbnailfile);
         xr=bmpthumbnail.xres/2;
         yr=bmpthumbnail.yres;
         bmp1=bmpthumbnail.crop(0,0,xr,yr);
         bmp2=bmpthumbnail.crop(xr,0,xr,yr);
         tx1=frame.CreateBitmapTexture("tx1",bmp1);
         tx2=frame.CreateBitmapTexture("tx2",bmp2);
         fr=thbsize/max(xr,yr*1.1);
         thbsizex=xr*fr;
         thbsizey=yr*fr;
         frame.add("Rectangle","Axis1":vector(thbsizex,0,0),"Axis2":vector(0,thbsizey,0),
                        "position":point(-0.5*thbsizex,-0.5*thbsizey+0.02*thbsize,-0.0001),
                        "Texture":tx1.name,"TextureRight":tx2.name,"EnableLight":false
                        );
      }
      txt=frame.add("FormattedText","Position":point(1.1*thbsize/2,0.6*thbsize/2,0));
      txt.EnableLight=false;
      txt.Content="\textbf{"+item.title+"}";
      txt.MaxLenX=0.75;
      txt.Color=color(0.9,0.9,0.9);
      txt.Size=0.18*thbsize;
      
      
      if isinsub and (item.action=="up") then {
         txt.Color=color(0.5,0.6,0.7);
         txt.position=txt.position+vector(-0.8*thbsize,-0.2*thbsize);
      }

      txt=frame.add("FormattedText","Position":point(1.1*thbsize/2,0.1*thbsize/2,0));
      txt.EnableLight=false;
      txt.Content=item.description;
      txt.MaxLenX=0.75;
      txt.Size=0.14*thbsize;
      txt.Color=color(0.6,0.6,0.6);
   
      y0=y0-thbsize;
   }
   
   if isinsub and (selnr==0) then {
      selnr=1;
      selpsy=thbsize;
   }
   
   #create selector frame
   selframe=thumbnailsframe.addsubframe("selframe");
   selframe.transf.origin=point(0,0);
   cv=selframe.add("Curve","Color":color(0.75,0.25,0.0),"Size":3);
   cv.addpoint(point(-0.6*thbsize,-0.5*thbsize));
   cv.addpoint(point( 0.9,-0.5*thbsize));
   cv.addpoint(point( 0.9, 0.5*thbsize));
   cv.addpoint(point(-0.6*thbsize, 0.5*thbsize));
   cv.addpoint(point(-0.6*thbsize,-0.5*thbsize));
}



function handle_enter(folder)
{
   myaction=folder.action;
   processed=false;
   actarg=folder.actionargument;
   actarg.Replace("[CurrentFolder]",folder.folder);
   actarg.Replace("[DATADIR]",DataDir);
   
   if myaction=="BrowsePictures" then {
      returnstring=ExecuteScript(datadir+"\scripts\Stereo Pictures\_ShowStereoPictures.SCI",actarg);
      if (returnstring=="timeout") and isinsub then stop("timeout");
      processed=true;
   }

   if myaction=="BrowseThumbnails" then {
      returnstring=ExecuteScript(datadir+"\scripts\Stereo Pictures\01_Browse stereo pictures.SCI",actarg);
      if (returnstring=="timeout") and isinsub then stop("timeout");
      processed=true;
   }

   if myaction=="ShowStereoVideo" then {
      ExecuteScript(datadir+"\scripts\Stereo Pictures\_ShowStereoVideo.SCI",actarg);
      processed=true;
   }

   if myaction=="RunScript" then {
      ExecuteScript(actarg);
      processed=true;
   }
   
   if myaction=="up" then {
      stop;
      processed=true;
   }
   
   if myaction=="" then {
      output(folder.folder);
      ExecuteScript(ScriptFilePath+"\_UserMenu.SCi",folder.folder);
      processed=true;
   }
   
   if not(processed) then ExecuteScript(datadir+"\scripts\test\sphere.SCi","");
   initscene;
}





###############################################################################################
# LOAD INFORMATION
###############################################################################################

#break;
isinsub=false;
#rootfolder=ReadSetting("StereoFolder","");
rootfolder=DataDir+"\UserMenu_Standard";
#if rootfolder=="" then rootfolder=DataDir+"\UserMenu\01_Meteorology";
if isdefined(ScriptArgument) then 
   if ScriptArgument.Length>0 then {
   if ScriptArgument.find("~t")>=0 then {
      rootfolder=ScriptArgument;
      rootfolder.split("~t");
      isinsub=false;
   } else {
      rootfolder=ScriptArgument;
      isinsub=true;
   }
}

folders=list;


if isinsub then {
   newfolder=map;
   newfolder.title="Back to previous menu";
   newfolder.folder=rootfolder;
   newfolder.action="";
   newfolder.thumbnailfile="";
   newfolder.description="";
   newfolder.action="up";
   newfolder.actionargument="";
   folders.add(newfolder);
}

folderlist=GetFileList(rootfolder+"\*.*",true);

foreach subfolder in folderlist do {
   if (subfolder!='.') and (subfolder!='..') then {
      name=subfolder;
      subfolder=rootfolder+'\'+subfolder;
      if FileIsPresent(subfolder+"\_description.txt") then {
         newfolder=map;
         newfolder.name=name;
         newfolder.folder=subfolder;
         newfolder.action="";
         newfolder.actionargument="";
         content=readtextfile(subfolder+"\_description.txt");
         while content!="" do {
            line=content.split("~n");
            keyword=line.split("=");
            if keyword=="ACTION" then {
               newfolder.action=line.split(";");
               newfolder.actionargument=line;
            }
            if keyword=="THUMBNAIL" then {
               line.Replace("[DATADIR]",DataDir);
               newfolder.thumbnailfile=line;
            }
            if keyword=="TITLE" then newfolder.title=line;
            if keyword=="DESCRIPTION" then newfolder.description=line;
         }
         newfolder.content="\textbf{"+newfolder.title+"} \\ "+newfolder.description;
         folders.add(newfolder);
      }
   }
}

initscene;
firstrender=true;
fadechrono=Chrono;
fadetime=0.4;

changed=true;
monitoractive=true;

idletimer.start;
while true do {


  #Respond to browsing movement
  vl=0;
  vl=40*(NavigationMove.y-2*NavigationRotate.x);
   if UIGetAxisActive(UIAxisY,UIAxisLevel2) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel2);
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
   if abs(vl)>0.0001 then {
      selpsy=selpsy-0.01*vl;
      if selpsy<0 then selpsy=0;
      if selpsy>(folders.size-1)*thbsize then selpsy=(folders.size-1)*thbsize;
      newselnr=round(selpsy/thbsize);
      if newselnr!=selnr then {
         selnr=newselnr;
      }
      changed=true;
      idletimer.Start;
   }
   

   
   if UIIsKeyDown("return") or UIIsLeftMouseButtonDown or JoystickButtonClicked(0,4) or NavigatorPressing then {
      while UIIsKeyDown("return") or UIIsLeftMouseButtonDown or JoystickButtonClicked(0,4) do {
         if UIIsRightMouseButtonDown then CheckStop;
         render;
      }
      if not(firstrender) then {
         fadeout(0.4);
         handle_enter(folders(selnr));
         firstrender=true;
         fadechrono.set(0);
      }
      changed=true;
      idletimer.Start;
   }
   
   vp.camerapos.y=-1*selpsy;
   if (selnr>=0) and (selnr<folders.size) then {
      selframe.transf.origin=point(0,-1*selnr*thbsize);
   }


   if (UIIsKeyDown("escape") or RightMouseClicked or NavigatorPulling or JoystickButtonClicked(0,6) or (idletimer.Elapsed>timeout)) 
         and isinsub and not(firstrender) then {
      fadeout(fadetime);
      if idletimer.elapsed>timeout then stop("timeout");
      stop;
   }

   #this line allows exiting the user menu with the joystick
   if JoystickButtonClicked(0,6) and not(firstrender) then {
      fadeout(fadetime);
      stop;
   }
   
   if not(isinsub) and (idletimer.Elapsed>5*60) and monitoractive then {
      SetMonitorStatus(false);
      monitoractive=false;
   }
   if changed and not(monitoractive) then {
      SetMonitorStatus(true);
      monitoractive=true;
   }

   root.Viewports.main.fadecolor=color(0,0,0,1-min(1,fadechrono.elapsed/fadetime));
   
   if fadechrono.elapsed<=fadetime then changed=true;

   renderskip(not(changed));
   if not(changed) then delay(1/60);
   
   if firstrender then WaitReleaseAll;
   else changed=false;
   firstrender=false;
   
   

}