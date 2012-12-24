

function LoadPlanetMoonsKepler(planframe,filename)
{
   filecontent=readtextfile(filename);
   filecontent.split("~n");
   
   moonsframe=planframe.Inclin.AddSubFrame("MoonsFrame");
   moonorbitsframe=planframe.Inclin.AddSubFrame("MoonOrbitsFrame");
   
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
      
#      moonframe.add("LightPoint","Size":50000,"Color":color(1,0.75,0));
      p=moonframe.add("SolidPoint","Size":6,"Color":color(0.5,0.3,0));
      p.BlendType=BlendTranslucent;p.DepthMask=DepthMaskDisable;
      p=moonframe.add("SolidPoint","Size":3,"Color":color(0.8,0.6,0));
      
      moonframe.add("TextFlat","Name":"Label","Content":(" "+moonname),"Size":50000,"Visible":false,
            "EnableLight":false,"Color":color(0.5,0.25,0));
      
      orbit=moonorbitsframe.add("Curve","Name":moonname,"Size":2,"Color":color(0.1,0.1,0.3));
#      orbit.BlendType=BlendTransparent;
#      orbit.DepthMask=DepthMaskDisable;
      mt.CreateCurve(orbit,200);
   }
}



function AnimatePlanetMoons(planetname,idist,isimpleversion)
{
   startdist=1500000;
   if isvardefined("idist") then
      if idist>0 then startdist=idist;
   
   simpleversion=false;
   if isvardefined("isimpleversion") then simpleversion=isimpleversion;

   ssysframe=ssys_create;
   
   sc=T_getscene;
   sc.VolumeShadowAdd(0,color(0,0,0,0.5),1000,1000*300);
   sc.ambientlightcolor=color(0.02,0.02,0.02);
   
   
   planetframe=ssys_createplanet(planetname,1);
   
   GetPlanetOrbit(planetname).dispose;
   
   LoadPlanetMoonsKepler(planetframe,datadir+"\AstroData\"+planetname+"_moons.txt");
   
   vp=T_getviewport;
   vp.cameradir=vecnorm(vector(0,-1,-0.5));
   vp.camerapos=point(0,0,0)-startdist*vp.cameradir;
   vp.cameraupdir=vector(0,0,1);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.NearClipPlane=0.1*startdist;
   vp.FarClipPlane=100*startdist;
   vp.FocalDistance=startdist;
   
   if simpleversion then {
      vp.UserNavigationAxisLevelRotate=2;
   }
   
   createvar(menu);
   if not(simpleversion) then {
      menu=T_createmenu;
      menu.SizeX=0.28;
      menu.add("",'Show moons',"ShowMoons",true);
      menu.add("",'Show moon orbits',"ShowMoonOrbits",true);
      menu.add("",'Show moon names',"ShowMoonNames",true);
      planetframe.Inclin.MoonsFrame.visible=false;
      planetframe.Inclin.MoonOrbitsFrame.visible=false;
   } else {
      planetframe.Inclin.MoonsFrame.visible=true;
      planetframe.Inclin.MoonOrbitsFrame.visible=true;   }
   
   #create clock
   clockframe=root.SC.Universe.addscreenframe("ClockFrame");
   clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
   info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":point(0.1,0.15,0),"Size":0.08);
   
   root.time=time(2011,3,21,0,0,0);
   root.TimeSpeedfactor=1000;
   while true do {
      incrtime;
   
      #mechanism to co-move camera with planet
      root.SC.Universe.transf.reset;
      ttf=planetframe.totaltransformation;
      ttf.invert;
      root.sc.universe.transf=ttf;
      root.sc.light0pos=ttf*point(0,0,0);
      
      #keep planet in focal plane
      vp.FocalDistance=1.1*distance(vp.camerapos,point(0,0,0));
      
      if not(simpleversion) then {
         if menu.WasModified and (menu.SelectID=="ShowMoons") then
            planetframe.Inclin.MoonsFrame.visible=menu.Checked("ShowMoons");
         if menu.WasModified and (menu.SelectID=="ShowMoonOrbits") then
            planetframe.Inclin.MoonOrbitsFrame.visible=menu.Checked("ShowMoonOrbits");
         if menu.WasModified and (menu.SelectID=="ShowMoonNames") then
         {
            menu.Checked("ShowMoons")=true;
            planetframe.Inclin.MoonsFrame.visible=true;
            foreach moon in planetframe.Inclin.MoonsFrame.getmembers do
               moon.label.visible=menu.Checked("ShowMoonNames");
         }
      }
      
      if RightMouseClicked then stop;
   
      render;
   }
}

#AnimatePlanetMoons("Saturnus",1100000);

