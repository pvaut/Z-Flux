
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


###############################################################################################
# GENERAL SETTINGS
###############################################################################################

#currentfolder=DataDir+"\StereoPictures";

idletimer=Chrono;
timeout=60;




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
      
      
   if FileIsPresent(currentfolder+"\"+picname+".txt") then {
      content=readtextfile(currentfolder+"\"+picname+".txt");
      textsize=0.025;jitter=0.002;frontshift=0.05;
      txtpx=-0.45*sizex;txtpy=0.45*sizey;
      for iang=0 to 26 do {
         ang=iang/27*2*Pi;
         txt1=imageframe.add("FormattedText","Position":point(txtpx+jitter*cos(ang),txtpy+jitter*sin(ang),frontshift+0.001),
            "EnableLight":false,"color":color(0.14,0.14,0.14),"Size":textsize);
         txt1.Content=content;
         txt1.MaxLenX=0.9*sizex;
      }
      txt1=imageframe.add("FormattedText","Position":point(txtpx,txtpy,frontshift+0.002),"EnableLight":false,"Size":textsize,"color":color(1,0.8,0.4));
      txt1.Content=content;
      txt1.MaxLenX=0.9*sizex;
   }
      
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
   changed=true;
   while not(finished) do {
      
      navmove=min(NavigationMove.x,2*NavigationRotate.y);
      navmove=min(navmove,-1*NavigationMove.y);
      navmove=min(navmove,2*NavigationRotate.x);
      
      if (UIIsKeyDown("left")) or (UIIsKeyDown("up")) or (JoystickRockerPos(0)==270) or (navmove<-0.005) then 
         if picnr>0 then {#show previous image
            idletimer.Start;
            picnr=picnr-1;
            fadeout(0.4);
            sss.Universe.imageframe.dispose;
            b=loadimage(thumbframes(picnr).name);
            b.DepthMask=DepthMaskDisable;
            fadein(0.4);
         }
         
      navmove=max(NavigationMove.x,2*NavigationRotate.y);
      navmove=max(navmove,-1*NavigationMove.y);
      navmove=max(navmove,2*NavigationRotate.x);
         
      if (UIIsKeyDown("right")) or (UIIsKeyDown("down")) or (JoystickRockerPos(0)==90) or 
          UIIsKeyDown("return") or (LeftMouseClicked) or NavigatorPressing or (navmove>0.005) then {
         if picnr<thumbframes.size-1 then {#show next image
            idletimer.Start;
            picnr=picnr+1;
            fadeout(0.4);
            sss.Universe.imageframe.dispose;
            b=loadimage(thumbframes(picnr).name);
            b.DepthMask=DepthMaskDisable;
            fadein(0.4);
         }
         else finished=true;
      }
      if UIIsKeyDown("escape") or RightMouseClicked or NavigatorPulling or (idletimer.elapsed>timeout)
           or JoystickButtonClicked(0,4) or JoystickButtonClicked(0,6) then finished=true;
           
      renderskip(not(changed));
      if not(changed) then delay(1/60);
      changed=false;
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
vp.enableusernavigation=false;
vp.EnableUserStop=true;


###############################################################################################
# Create directory browse menu
###############################################################################################

currentfolder=datadir+"\StereoPictures\Samples";
if isdefined(ScriptArgument) then 
   if ScriptArgument.Length>0 then
      currentfolder=ScriptArgument;

vp.enableusernavigation=false;


###############################################################################################
# LOAD THUMBNAILS
###############################################################################################

ycount=1;
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
}



hiderendermessage;


showimage(0);

if idletimer.elapsed>timeout then stop("timeout");

stop;
