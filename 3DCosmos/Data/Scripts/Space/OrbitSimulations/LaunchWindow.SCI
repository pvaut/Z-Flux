codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

sf=T_scene_create;
sc=T_getscene;
vp=T_getviewport;
vp.nearclipplane=0.1;
vp.farclipplane=50;

vp.camerapos=point(-6,2,6);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);

sc.Light0Pos=point(-100,300,100);

sc.ambientlightcolor=color(0.1,0.1,0.1);

vp.transf.origin=point(0,0,-3);

textcl=color(0.4,0.4,0.4);

sf.add("Arrow","Color":textcl,"EnableLight":false,
      "Position":point(0,-2.2,0),"Axis":vector(0,0,3),"NormAxis":vector(0,1,0),"Width":0.3,"Thickness":0);
sf.add("Text3D","Color":textcl,"EnableLight":false,"Content":'Time',
      "Position":point(-0.2,-2.2,3.1),"UnitX":vector(0,0,0.6),"UnitY":vector(0.6,0,0),"Depth":0);



#####################################################################################
# FRAME WITH 2D "NOW" WORLD
#####################################################################################

centerframe=sf.addsubframe("Center");
centerframe.velocity=vector(0,0,1/30000000);

centerframe.add("Arrow","Color":textcl,"EnableLight":false,
      "Position":point(0,2.2,0),"Axis":vector(0,-0.6,0),"NormAxis":vector(0,0,1),"Width":0.3,"Thickness":0);
txt=centerframe.add("Text3D","Color":textcl,"EnableLight":false,"Content":'The present',
      "UnitX":vector(0.4,0,0),"UnitY":vector(0,0.4,0),"Depth":0);
txt.position=point(0-0.4/2*txt.GetSizeX,2.3,0);


root.time=time(2008,1,1,0,0,0);


############ PARAMETERS ##########################################""

Aearth=0.7;Amars=1.523;

Tearth=360.0/(0.9856076686/(Aearth^1.5));
Tmars=360.0/(0.9856076686/(Amars^1.5));

Acraft=(Aearth+Amars)/2;
Tcraft=360.0/(0.9856076686/(Acraft^1.5));
Ecraft=(Amars-Aearth)/(Amars+Aearth);



########################################################################################
# EARTH
########################################################################################

earthframe=centerframe.addsubframe("EarthFrame");
earthframe.add("sphere","radius":0.05,"color":color(0.5,0.5,1));
earthframe.motion=MotionKepler.create(earthframe);
earthframe.motion.SemiMajorAxis=Aearth;
earthframe.motion.Eccentricity=0;
earthframe.motion.Period=Tearth;
earthframe.motion.PeriTime=root.time;
#Tracking
earthtrack=sf.add("Curve","color":color(0.4,0.5,1),"size":2);
earthtrack.Track(earthframe,0.05);
#Line
earthline=centerframe.add("Curve","size":1,"Color":color(0.5,0.5,0.5,0.5),"CurveRenderType":CurveRenderDash);
#Orbit
orbit=centerframe.add("Curve","size":1,"Color":color(0.3,0.3,0.8,0.5),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
orbit.makecircle(point(0,0,0),vector(0,0,1),Aearth,80);



########################################################################################
# MARS
########################################################################################

marsframe=centerframe.addsubframe("MarsFrame");
marsframe.add("sphere","radius":0.05,"color":color(1,0.6,0.3));
marsframe.motion=MotionKepler.create(marsframe);
marsframe.motion.SemiMajorAxis=Amars;
marsframe.motion.Eccentricity=0;
marsframe.motion.Period=Tmars;
marsframe.motion.PeriTime=root.time;
#Tracking
marstrack=sf.add("Curve","color":color(1,0.6,0.3),"size":2);
marstrack.Track(marsframe,0.05);
#Line
marsline=centerframe.add("Curve","size":1,"Color":color(0.5,0.5,0.5,0.5),"CurveRenderType":CurveRenderDash);
#Orbit
orbit=centerframe.add("Curve","size":1,"Color":color(1,0.6,0.3,0.3),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
orbit.makecircle(point(0,0,0),vector(0,0,1),Amars,80);

########################################################################################
# CRAFT
########################################################################################

craftframe=centerframe.addsubframe("CraftFrame");
craftframe.add("sphere","radius":0.05,"color":color(0.3,1,0.3));
craftframe.motion=MotionKepler.create(craftframe);
craftframe.motion.SemiMajorAxis=Acraft;
craftframe.motion.Eccentricity=Ecraft;
craftframe.motion.Period=Tcraft;
craftframe.motion.PeriTime=root.time;
#Tracking
crafttrack=sf.add("Curve","color":color(0.3,1,0.3),"size":2);
crafttrack.Track(craftframe,0.05);
#Line
craftline=centerframe.add("Curve","size":1,"Color":color(0.5,0.5,0.5,0.5),"CurveRenderType":CurveRenderDash);
#Orbit
orbit=centerframe.add("Curve","size":1,"Color":color(0.3,1,0.3,0.3),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
e_a=Acraft;e_b=sqrt(1-sqr(Ecraft))*Acraft;
orbit.Generate(functor("point(e_a*cos(a)-Acraft+Aearth,e_b*sin(a),0)","a"),0,2*Pi,80);


########################################################################################
# SUN
########################################################################################

#Sun object
centerframe.add("sphere","radius":0.05,"color":color(0.6,0.6,0.0));

#Sun tracking
suntrack=sf.add("Curve","color":color(0.6,0.6,0.0),"size":2);
suntrack.Track(centerframe,0.05);


tubelen=6;
tuberesol=60;
tubetransp=0.15;
tubefr=1;

tubef=sf.addsubframe("TubesFrame");
tubef.AutoSort=true;

tubes=list;

earthtubeframe1=tubef.addsubframe("EarthTubeFrame1");
earthtubeframe1.transf.yaxis=vector(0,sqrt(1-sqr(Ecraft)),0);
tb=earthtubeframe1.add("Cylinder","color":color(0.3,0.3,0.8,tubetransp),
                   "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":true,"RenderBack":false,
                   "Position":point(0,0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Aearth),"IsClosed":false,"Resolution":tuberesol);
earthtubeframe1.center=point(-1*Aearth,0,0);
tubes.add(tb);

earthtubeframe2=tubef.addsubframe("EarthTubeFrame2");
earthtubeframe2.transf.yaxis=vector(0,sqrt(1-sqr(Ecraft)),0);
tb=earthtubeframe2.add("Cylinder","color":color(0.3,0.3,0.8,tubetransp),
                   "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":false,"RenderBack":true,
                   "Position":point(0,0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Aearth),"IsClosed":false,"Resolution":tuberesol);
earthtubeframe2.center=point(1*Aearth,0,0);
tubes.add(tb);


marstubeframe1=tubef.addsubframe("MarsTubeFrame1");
tb=marstubeframe1.add("Cylinder","color":color(0.8,0.3,0.3,tubetransp),
                   "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":true,"RenderBack":false,
                   "Position":point(0,0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Amars),"IsClosed":false,"Resolution":tuberesol);
marstubeframe1.center=point(-1*Amars,0,0);
tubes.add(tb);

marstubeframe2=tubef.addsubframe("MarsTubeFrame2");
tb=marstubeframe2.add("Cylinder","color":color(0.8,0.3,0.3,tubetransp),
                   "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":false,"RenderBack":true,
                   "Position":point(0,0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Amars),"IsClosed":false,"Resolution":tuberesol);
marstubeframe2.center=point(1*Amars,0,0);
tubes.add(tb);


crafttubeframe1=tubef.addsubframe("CraftTubeFrame1");
crafttubeframe1.transf.yaxis=vector(0,sqrt(1-sqr(Ecraft)),0);
tb=crafttubeframe1.add("Cylinder","color":color(0.3,0.8,0.3,tubetransp),
                   "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":true,"RenderBack":false,
                   "Position":point(-1*(Acraft-Aearth),0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Acraft),"IsClosed":false,"Resolution":tuberesol);
crafttubeframe1.center=point(-1*Acraft,0,0);
tubes.add(tb);

crafttubeframe2=tubef.addsubframe("CraftTubeFrame1");
crafttubeframe2.transf.yaxis=vector(0,sqrt(1-sqr(Ecraft)),0);
tb=crafttubeframe2.add("Cylinder","color":color(0.3,0.8,0.3,tubetransp),
                    "BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable,"CanBuffer":true,
                   "RenderFront":false,"RenderBack":true,
                   "Position":point(-1*(Acraft-Aearth),0,0),"Axis":vector(0,0,tubelen),"Radius":(tubefr*Acraft),"IsClosed":false,"Resolution":tuberesol);
crafttubeframe2.center=point(1*Acraft,0,0);
tubes.add(tb);

foreach tube in tubes do {
   tube.SpecularColor=color(0.5,0.5,0.5);
   tube.SpecularValue=40;
}

clockframe=sf.addscreenframe("ClockFrame");
info_clock=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.01,0.01,0),"Size":0.04,
   "Color":color(0.7,0.7,0.7),"EnableLight":false);

craftylast=-1;
marsylast=-1;

root.TimeSpeedFactor=1000000;
while true do {
   incrtime;
   earthline.makeline(point(0,0,0),earthframe.transf.origin);
   craftline.makeline(point(0,0,0),craftframe.transf.origin);
   marsline.makeline(point(0,0,0),marsframe.transf.origin);
   
   if craftframe.transf.origin.y<0 and craftylast>0 then {
      ps=craftframe.totaltransformation.origin;
      sf.add("Curve","color":color(0.3,0.8,0.3)).makeline(ps-vector(0,0,0.05),ps+vector(0,0,0.05));
   }
   craftylast=craftframe.transf.origin.y;

   if marsframe.transf.origin.y<0 and marsylast>0 then {
      ps=marsframe.totaltransformation.origin;
      sf.add("Curve","color":color(0.8,0.5,0.3)).makeline(ps-vector(0,0,0.05),ps+vector(0,0,0.05));
   }
   marsylast=marsframe.transf.origin.y;
   
   render;
}
