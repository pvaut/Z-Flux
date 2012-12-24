
codeblock readtextfile(ScriptDir+"\TOOLS.sci");
codeblock readtextfile(ScriptDir+"\SSYS.sci");

ssysframe=ssys_create;

satframe=ssys_createplanet("Saturnus",10);


filecontent=readtextfile(ScriptDir+"\Saturnus\saturnus_moons.txt");
filecontent.split("~n");

#Add Saturnus disc
satframe.Inclin.Globe.GlobeRendering.add("Cylinder","Radius":120000,"Axis":vector(0,0,1));

moonsframe=satframe.Inclin.AddSubFrame("MoonsFrame");
moonorbitsframe=satframe.Inclin.AddSubFrame("MoonOrbitsFrame");

while filecontent.length>0 do {
   line=filecontent.split("~n");
   st=line.split("~t");
   epochtm=time(ToScalar(st.split(",")),ToScalar(st.split(",")),ToScalar(st.split(",")),
                ToScalar(st.split(",")),ToScalar(st.split(",")),ToScalar(st.split(",")));
   moonname=line.split("~t");
   
   moon_a=ToScalar(line.split("~t"));
   moon_e=ToScalar(line.split("~t"));
   moon_w=ToScalar(line.split("~t"));
   moon_mean=ToScalar(line.split("~t"));
   moon_i=ToScalar(line.split("~t"));
   moon_O=ToScalar(line.split("~t"));
   line.split("~t");
   moon_period=ToScalar(line.split("~t"));
   
   moon_peritm=epochtm;
   moon_peritm.AddDays(0-moon_mean/360.0*moon_period);
   
   
   moonframe=moonsframe.addsubframe(moonname);
   mt=MotionKepler.Create(moonframe,"Motion");
   mt.SemiMajorAxis=moon_a;
   mt.Eccentricity=moon_e;
   mt.Inclination=deg2rad(moon_i);
   mt.AscendingNode=deg2rad(moon_O);
   mt.ArgPerigee=deg2rad(moon_w);
   mt.period=moon_period;
   mt.PeriTime=moon_peritm;
   moonframe.motion=mt;
   
   moonframe.add("LightPoint","Size":50000);
   
   orbit=moonorbitsframe.add("Curve","Name":moonname,"Color":color(0.5,0,0.5,1));
   mt.CreateCurve(orbit,200);
}

vp=T_getviewport;

vp.cameradir=vecnorm(vector(0,-1,-0.5));
vp.camerapos=point(0,0,0)-15000000*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;
vp.EnableUserTimeControl=true;
vp.NearClipPlane=0.1*15000000;
vp.FarClipPlane=30*15000000;
vp.FocalDistance=15000000;
vp.EyeSeparation=0.1*vp.FocalDistance;

root.time=time(2008,3,21,0,0,0);
root.TimeSpeed=10;
while true do {
   incrtime;
   
   #mechanism to co-move camera with planet
   if true then {
      root.SC.Universe.transf.reset;
      ttf=satframe.totaltransformation;
      ttf.invert;
      root.viewports.main.transf=ttf;
   }
   
   render;
}
