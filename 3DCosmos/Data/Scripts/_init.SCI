
##################################################################@@@@@@@@###########
# WARNING
# all viewports (=with associated windows) should be created here, and remain alive
# reason: impossible to create a window in a thread!
##################################################################@@@@@@@@###########


resetall;


displayname=ReadSetting("DisplayName","");
if displayname=="" then displayname="\\.\DISPLAY1";

SetActiveLanguage(ReadSetting("Language","English"));

tpe=1;

createvar(vp);
if tpe==1 then {
   root.rendertype=RenderSingle;
   root.vsync=ReadSetting("SyncFactor",0);
   root.framerate=999999;st=ReadSetting("FrameRate","");if st!="" then root.framerate=toscalar(st);
#   root.framerate=30;
   vp=addviewport(0,0,1,1,displayname,displayname);
   vp.name="main";
   vp.Usestereo=ReadSetting("ShowStereo",false);
   vp.SwapStereo=ReadSetting("SwapLeftRight",false);
   vp.StretchFactor=ReadSetting("HorizontalStretch",1);
   vp.StretchFactorY=ReadSetting("VerticalStretch",1);
   vp.HMirrorLeft=ReadSetting("MirrorLeftH",false);
   vp.HMirrorRight=ReadSetting("MirrorRightH",false);
   vp.VMirrorLeft=ReadSetting("MirrorLeftV",false);
   vp.VMirrorRight=ReadSetting("MirrorRightV",false);

   vp.RightViewShiftX=ReadSetting("RightViewShiftX",0.5);
   vp.RightViewShiftY=ReadSetting("RightViewShiftY",0);

   vp.XMinFrac=ReadSetting("VPXMinFrac",0);
   vp.YMinFrac=ReadSetting("VPYMinFrac",0);
   vp.XMaxFrac=ReadSetting("VPXMaxFrac",1);
   vp.YMaxFrac=ReadSetting("VPYMaxFrac",1);
   
   vp.EyeSepFactor=ReadSetting("EyeSepFactor",0.025);
 
# FOR WARPED RENDERING:  
#   warpx=matrix(101,101);
#   warpy=matrix(101,101);
#   for ix=0 to 100 do
#      for iy=0 to 100 do {
#         x=ix/100;
#         y=iy/100;
#         warpx(ix,iy)=x/(1+y);
#         warpy(ix,iy)=y*(0.5+0.5*x);
#      }
#   vp.EnableWarped(2000,1500,warpx,warpy);

   disp1=root.displays.get(displayname);
   
   st=ReadSetting("WinOffsetX","");if st!="" then disp1.Xoffset=ToScalar(st);
   st=ReadSetting("WinOffsetY","");if st!="" then disp1.Yoffset=ToScalar(st);
   st=ReadSetting("WinSizeX","");if st!="" then disp1.Xres=ToScalar(st);
   st=ReadSetting("WinSizeY","");if st!="" then disp1.Yres=ToScalar(st);
   disp1.Fullscreen=ReadSetting("FullScreen",false);;
   disp1.start;
   vp.start;
}

if tpe==2 then {
   root.rendertype=RenderDual;
   root.vsync=0;
   vp=addviewport(0,0,1,1,displayname,displayname);
   vp.name="main";
   disp1=root.displays.get(displayname);
   disp1.start;
   vp.start;
}

if tpe==3 then {
   root.rendertype=RenderMultithreaded;
   root.vsync=1;
   vp=addviewport(0.003,0.003,0.997,0.997,displayname,displayname);
   vp.name="main";
   disp1=root.displays.get(displayname);
   disp1.start;
   vp.start;
}

for i1=0 to 2 do
   for i2=0 to 2 do
   {
      defval=0;
      if i1==i2 then defval=1;
      root.ColorMatrixLeft.get(i1,i2)=ReadSetting("ColorMatrixLeft"+str(i1)+str(i2),defval);
      root.ColorMatrixRight.get(i1,i2)=ReadSetting("ColorMatrixRight"+str(i1)+str(i2),defval);
   }
   

