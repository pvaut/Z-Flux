#   resetallscenes;
#   resetallviewports;
#   resetallvideos;
   
   resetall;
   
   displayname=root.displays.getmembers.get(0).name;
   
   vp=addviewport(0,0,1,1,displayname,displayname);
   
   
vp.name="main";
disp1=root.displays.get(displayname);
disp1.start;
vp.usestereo=true;


vp.start;
   

   
   vp.camerapos=point(0,6,10);
   vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
   vp.cameraupdir=vector(0,1,0);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.NearClipPlane=0.05;
   vp.FarClipPlane=30;
   vp.FocalDistance=10;
   vp.EyeSeparation=0.5;
   sc=addscene;sc.name="SC";
   sc.light0pos=point(50,50,100);
   vp.setscene(sc);
   scene.ambientlightcolor=color(0.1,0.1,0.1);
   scene.start;
   universe=sc.addsubframe("Universe");
   
   root.time=time(2008,1,1,0,0,0);
   root.TimeSpeed=1;


while true do {
   render;
}


