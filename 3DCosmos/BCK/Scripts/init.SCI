
#stop;

store("test","42");


resetall;

#dsplist=root.displays.getmembers;
#dsp=dsplist(0);
#dsp.xres=1000;
#dsp.fullscreen=false;

displayname="\\.\DISPLAY1";

tpe=1;

createvar(vp);
if tpe==1 then {
   root.rendertype=RenderSingle;
   root.vsync=1;
   root.framerate=1400;
   vp=addviewport(0,0,1,1,displayname,displayname);
}

if tpe==2 then {
   root.rendertype=RenderDual;
   root.vsync=0;
   vp=addviewport(0.003,0.003,0.5997,0.5997,displayname,displayname);
}

if tpe==3 then {
   root.rendertype=RenderMultithreaded;
   root.vsync=1;
   vp=addviewport(0.003,0.003,0.997,0.997,displayname,displayname);
}

vp.name="main";
#vp.stretchfactor=1;
#vp.eyeseparation=5;
#vp.focaldistance=15;
vp.FrameSize=0;
vp.aperture=deg2rad(60.0);
disp1=root.displays.get(displayname);
disp1.start;
vp.usestereo=true;


vp.start;

   
   
#ExecuteScript(ScriptDir+"\test.sci");

stop;


