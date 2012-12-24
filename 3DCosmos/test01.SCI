resetallscenes;

vp=all.viewports.main;
vp.cameradir=vecnorm(vector(0,-100,-1));
vp.camerapos=point(0,0,0)-4*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;


scene=addscene;scene.name="Universe";
scene.light0pos=point(10,10,0);
vp.setscene(scene);
scene.ambientlightcolor=color(0.1,0.1,0.1);
scene.start;

ssys=scene.addsubframe("s");


s=ssys.addobject("sphere","myshere");

s.radius=1;


render;