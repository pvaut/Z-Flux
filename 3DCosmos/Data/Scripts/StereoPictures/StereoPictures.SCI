
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


###############################################################################################
# GENERAL SETTINGS
###############################################################################################

ycount=ReadSetting("StereoBrowserRows",3);
currentfolder=DataDir+"\StereoPictures";





###############################################################################################
# SHOW ZOOMED IMAGE FUNCTION AND RENDER LOOP
###############################################################################################


function loadimage(picname)
{
   bmp=loadbitmap(currentfolder+"\"+picname+".jpg");
   xr=bmp.xres/2;
   yr=bmp.yres;
   bmp1=bmp.crop(0,0,xr,yr);
   bmp2=bmp.crop(xr,0,xr,yr);
   
   frx=vp.aspectratio/xr;
   fry=1/yr;
   fr=min(frx,fry);
   fr=fr*2*tan(vp.Aperture/2);
      
   imageframe=sss.Universe.addsubframe("ImageFrame");
   tx1=imageframe.CreateBitmapTexture("tx1",bmp1);
   tx2=imageframe.CreateBitmapTexture("tx2",bmp2);
   
   sizex=fr*xr;
   sizey=fr*yr;
   
   b=imageframe.add("Rectangle","Axis1":vector(sizex,0,0),"Axis2":vector(0,sizey,0),
      "position":point(-0.5*sizex,-0.5*sizey,-0.0001),"EnableLight":false,
      "Texture":tx1.name,
      "TextureRight":tx2.name,
      "color":color(1,1,1));
   return(b);
}

function showimage(picnr)
{
   #init
   backup_camerapos=vp.camerapos;
   thumbnailsframe.visible=false;
   vp.CameraPos=point(0,0,1);
   vp.EnableUserStop=false;
   
   b=loadimage(thumbframes(picnr).name);
   b.DepthMask=DepthMaskDisable;
   
   fadein(0.5);
   
   
   finished=false;
   while not(finished) do {
      render;
      
      navmove=min(NavigationMove.x,2*NavigationRotate.y);
      navmove=min(navmove,-1*NavigationMove.y);
      navmove=min(navmove,2*NavigationRotate.x);
      
      if (UIIsKeyDown("left")) or (UIIsKeyDown("up")) or (JoystickRockerPos(0)==270) or (navmove<-0.005) then 
         if picnr>0 then {#show previous image
            picnr=picnr-1;
            fadeout(0.15);
            sss.Universe.imageframe.dispose;
            b=loadimage(thumbframes(picnr).name);
            b.DepthMask=DepthMaskDisable;
            fadein(0.25);
         }
         
      navmove=max(NavigationMove.x,2*NavigationRotate.y);
      navmove=max(navmove,-1*NavigationMove.y);
      navmove=max(navmove,2*NavigationRotate.x);
         
      if (UIIsKeyDown("right")) or (UIIsKeyDown("down")) or (JoystickRockerPos(0)==90) or 
          UIIsKeyDown("return") or (LeftMouseClicked) or NavigatorPressing or (navmove>0.005) then {
         if picnr<thumbframes.size-1 then {#show next image
            picnr=picnr+1;
            fadeout(0.15);
            sss.Universe.imageframe.dispose;
            b=loadimage(thumbframes(picnr).name);
            b.DepthMask=DepthMaskDisable;
            fadein(0.25);
         }
         else finished=true;
      }
      if UIIsKeyDown("escape") or RightMouseClicked or NavigatorPulling
           or JoystickButtonClicked(0,4) or JoystickButtonClicked(0,6) then finished=true;
   }

   
   #finalise
   fadeout(0.5);
   sss.Universe.imageframe.dispose;
   vp.camerapos=backup_camerapos;
   thumbnailsframe.visible=true;
   vp.EnableUserStop=true;
   return(picnr);
}




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
vp.enableusernavigation=true;
vp.EnableUserStop=true;


###############################################################################################
# Create directory browse menu
###############################################################################################

currentfolder="";
if isdefined(ScriptArgument) then 
   if ScriptArgument.Length>0 then
      currentfolder=ScriptArgument;
if currentfolder.length<=0 then {
   
   menu=T_createmenu("Select stereo pictures folder",true);
   menu.sizex=0.35;
   menu.Color=color(1,1,1,1);
   root.SC.Universe.MenuFrame.Color=color(1,1,1,1);
   root.mousedampingfactor=0.25;
   root.SC.Universe.MenuFrame.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,1),0.03);

   
   
   
   vp.ShowControls=true;
   startfolderinfo=map;
   startfolderinfo.Folder=ReadSetting("StereoFolder","");
   if startfolderinfo.Folder=="" then startfolderinfo.Folder=DataDir+"\StereoPictures";
   startfolderinfo.Name="Stereo pictures";
   startfolderinfo.Parent="";
   startfolderinfo.Level=-1;
   todofolderlist=list(startfolderinfo);
   while todofolderlist.size>0 do {
      currentfolderinfo=todofolderlist(todofolderlist.size-1);
      todofolderlist.del(todofolderlist.size-1);
      #put subfolders in the todo stack
      folderlist=GetFileList(currentfolderinfo.Folder+"\*.*",true);
      folderlist.Invert;
      foreach subfolder in folderlist do 
         if (subfolder!=".") and (subfolder!="..") then {
            subfolderinfo=map;
            subfolderinfo.Folder=currentfolderinfo.Folder+"\"+subfolder;
            subfolderinfo.Name=Deref(subfolder);
            subfolderinfo.Parent=currentfolderinfo.Folder;
            subfolderinfo.Level=currentfolderinfo.Level+1;
            todofolderlist.add(subfolderinfo);
         }
      if currentfolderinfo.Level>=0 then {
         pindex="";
         if currentfolderinfo.Level>0 then pindex=currentfolderinfo.Parent;
         menu.Add(pindex,currentfolderinfo.Name,currentfolderinfo.folder);
      }
         
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
   if menu.SelectID=="[Close]" then stop;
   T_delmenu;
}

vp.enableusernavigation=false;
root.mousedampingfactor=0.85;


###############################################################################################
# LOAD THUMBNAILS
###############################################################################################

rendermessage(translate("Loading thumbnails"));

thbsize=0.99*2*tan(vp.Aperture/2)/(ycount+0);
thbsize2=0.9*thbsize;

thumbnailsframe=sss.Universe.addsubframe("ThumbnailsFrame");
columnframes=list;
thumbframes=list;


piclist=GetFileList(currentfolder+"\*.jpg");
xps=-1;
yps=9999;
maxposx=0;
createvar(columnframe);
foreach picfile in piclist do {
   yps=yps+1;
   if yps>=ycount then {
      xps=xps+1;
      yps=0;
      columnframe=thumbnailsframe.addsubframe(str(xps));columnframes.add(columnframe);
      columnframe.transf.origin=point(xps*thbsize,0);
      maxposx=max(maxposx,columnframe.transf.origin.x);
   }
   picname=picfile;picname.invert;picname.split(".");picname.invert;
   thumbframe=columnframe.addsubframe(picname);thumbframes.add(thumbframe);
   thumbframe.transf.origin=point(0,-1*(yps-ycount/2+0.5)*thbsize);
   thumbfilename=currentfolder+"\"+picname+".THB";
   if not(fileispresent(thumbfilename)) then thumbfilename=datadir+"\textures\questionstereo.jpg";
   bmpthumbnail=LoadBitmap(thumbfilename);
   xr=bmpthumbnail.xres/2;
   yr=bmpthumbnail.yres;
   bmp1=bmpthumbnail.crop(0,0,xr,yr);
   bmp2=bmpthumbnail.crop(xr,0,xr,yr);
   tx1=thumbframe.CreateBitmapTexture("tx1",bmp1);
   tx2=thumbframe.CreateBitmapTexture("tx2",bmp2);
   fr=thbsize2/max(xr,yr*1.1);
   thbsizex=xr*fr;
   thbsizey=yr*fr;
   thumbframe.add("Rectangle","Axis1":vector(thbsizex,0,0),"Axis2":vector(0,thbsizey,0),
                  "position":point(-0.5*thbsizex,-0.5*thbsizey+0.02*thbsize,-0.0001),
                  "Texture":tx1.name,"TextureRight":tx2.name,"EnableLight":false
                  );
   txt=thumbframe.add("Text3D","Content":picname,"Color":color(0.5,0.5,0.5),"EnableLight":false,
                    "UnitX":vector(thbsize/15,0,0),"UnitY":vector(0,thbsize/15,0));
   txt.position=point(-0.5*txt.GetSizeX*thbsize/15,-0.45*thbsize,0);
}


#create selector frame
selframe=thumbnailsframe.addsubframe("selframe");
selframe.transf.origin=point(columnframes(0).transf.origin.x,thumbframes(0).transf.origin.y,0);
cv=selframe.add("Curve","Color":color(0.75,0.25,0.0),"Size":2);
cv.addpoint(point(-0.5*thbsize,-0.5*thbsize));
cv.addpoint(point( 0.5*thbsize,-0.5*thbsize));
cv.addpoint(point( 0.5*thbsize, 0.5*thbsize));
cv.addpoint(point(-0.5*thbsize, 0.5*thbsize));
cv.addpoint(point(-0.5*thbsize,-0.5*thbsize));

hiderendermessage;


###############################################################################################
# RENDER LOOP FOR THUMBNAIL BROWSING
###############################################################################################

function applydistwarping()
{
   foreach column in columnframes do {
      xx=column.transf.origin.x;
      zz=-0.37*sqr(xx-vp.camerapos.x);
      column.transf.origin=point(xx,column.transf.origin.y,zz);
      column.visible=zz>-2;
   }
}

selx=0;
sely=0;
selpsx=0;
selpsy=0;

firstrender=true;

while true do {

  #Horizontal movement check
  vl=0;
  vl=40*(NavigationMove.x+2*NavigationRotate.y);
   if UIGetAxisActive(UIAxisX,UIAxisLevel2) then vl=UIGetAxisPos(UIAxisX,UIAxisLevel2);
   if UIGetAxisActive(UIAxisX,UIAxisLevel0) then vl=UIGetAxisPos(UIAxisX,UIAxisLevel0);
   if abs(vl)>0.0001 then {
      selpsx=selpsx+0.02*vl;
      if selpsx<0 then selpsx=0;
      if selpsx>maxposx then selpsx=maxposx;
      vp.camerapos.x=selpsx;
      newselx=round(vp.camerapos.x/thbsize);
      if newselx!=selx then {
         selpicnr=sely+ycount*newselx;
         if (selpicnr>=0) and (selpicnr<thumbframes.size) then {
            selframe.transf.origin=point(columnframes(newselx).transf.origin.x,selframe.transf.origin.y,selframe.transf.origin.z);
            selx=newselx;
         }
      }
      
   }

  #Vertical movement check
  vl=0;
  vl=40*(NavigationMove.y-2*NavigationRotate.x);
   if UIGetAxisActive(UIAxisY,UIAxisLevel2) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel2);
   if UIGetAxisActive(UIAxisY,UIAxisLevel0) then vl=UIGetAxisPos(UIAxisY,UIAxisLevel0);
   if abs(vl)>0.0001 then {
      selpsy=selpsy-0.03*vl;
      if selpsy<0 then selpsy=0;
      if selpsy>(ycount-1)*thbsize then selpsy=(ycount-1)*thbsize;
      newsely=round(selpsy/thbsize);
      if newsely!=sely then {
         selpicnr=newsely+ycount*selx;
         if (selpicnr>=0) and (selpicnr<thumbframes.size) then {
            selframe.transf.origin=point(selframe.transf.origin.x,thumbframes(selpicnr).transf.origin.y,selframe.transf.origin.z);
            sely=newsely;
         }
      }
   }

   applydistwarping;
   
   if UIIsKeyDown("return") or LeftMouseClicked or NavigatorPressing or JoystickButtonClicked(0,4) then {
      if not(firstrender) then {#zoom in on image
         fadeout(0.5);
         selpicnr=sely+ycount*selx;
         selpicnr=showimage(selpicnr);
         selx=floor(selpicnr/ycount);
         sely=selpicnr-selx*ycount;
         selframe.transf.origin=point(columnframes(selx).transf.origin.x,thumbframes(selpicnr).transf.origin.y,selframe.transf.origin.z);
         vp.camerapos.x=selframe.transf.origin.x;
         applydistwarping;
         fadein(0.5);
         selpsx=vp.camerapos.x;
         selpsy=sely*thbsize;
         firstrender=true;
      }
   }
   
   if JoystickButtonClicked(0,6) or RightMouseClicked or NavigatorPulling then {
      if not(firstrender) then stop;
   }

   render;
   if firstrender then WaitReleaseAll;
   firstrender=false;
}
