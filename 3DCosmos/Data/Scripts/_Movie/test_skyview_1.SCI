codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptFilePath+"\_SkyViewTools.sci");


mydata=map;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
InitSkyView(ref(mydata));
SetViewPosition(0,deg2rad(51));

root.time=time(2011,6,27,0,0,0);

root.FrameRate=30;
root.TimeSpeedFactor=1000;

###############################################################################"

mydata.zoomfrac=1.0;
SkyFrameVisible(true);

animate(1);
Cam_RotateHor(root.Viewports.Main,Pi/2,3);
animate(3);
Cam_RotateVert(root.Viewports.Main,Pi/4,3);
Cam_RotateHor(root.Viewports.Sky,-1*Pi,10);


animate(20);

stop;