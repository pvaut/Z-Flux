
#mean solar time at Mars's prime meridian (i.e., at the centre of the crater Airy-0).
function G_MTC()
{
   MTC=24*(((root.time.JD-2451549.5)/1.027491252) + 44796.0 - 0.00096);
   return(MTC-24*floor(MTC/24));
}

function SUB_G_MarsPositFrame()
{
   return(root.SC.Universe.SolarSystem.Mars.Inclin.Globe.GlobeRendering.LocalPosit);
}


function SUB_MarsSolarMovementInit(infoset)
{

   infoset.scenescale=100000;
   infoset.globeradius=7*infoset.scenescale;
   infoset.marsradius=4000;

   sss=T_getscene;
   sss.VolumeShadowReset;
   sss.VolumeShadowAdd(0,color(0,0,0,0.25),0.01*infoset.scenescale,16*infoset.scenescale);

   myviewport=T_getviewport;
   myviewport.focaldistance=1.05*infoset.scenescale;
   myviewport.nearclipplane=infoset.scenescale;
   myviewport.farclipplane=200*infoset.scenescale;
   myviewport.cameradir=vecnorm(vector(0.1,-1,0));
   myviewport.cameraupdir=vector(0,0,1);
   cpos0=point(0,0,6*infoset.scenescale)-25*infoset.scenescale*myviewport.cameradir;
   myviewport.camerapos=cpos0;
   myviewport.enableusernavigation=false;
   
   #sun reference
   ssysframe.add("Sphere","color":color(1,1,0.5),"Radius":autokm(0.02),"EnableLight":false);
   
   marsframe=root.SC.Universe.SolarSystem.Mars;
   
   GetPlanetOrbit("Mars").visible=false;
   
   #Delete Mars object
   marsframe.Inclin.Globe.GlobeRendering.Dispose;
   marsglobeframe=marsframe.Inclin.Globe.addsubframe("GlobeRendering");
   
   #Create local viewer frame   
   marspositframe=marsglobeframe.addsubframe("LocalPosit");
   marspositframe.transf.origin=radial2point(infoset.marsradius,infoset.longit,infoset.lattit);
   marspositframe.transf.Zaxis=@vector(radial2point(1,infoset.longit,infoset.lattit));
   marspositframe.transf.Xaxis=vector(-1*sin(infoset.longit),cos(infoset.longit),0);
   marspositframe.transf.Yaxis=vecnorm(marspositframe.transf.Zaxis*marspositframe.transf.Xaxis);
   
   #Mars surface
   surfaceframe=marspositframe.addsubframe("Surface");
   surfaceframe.CastVolumeShadow=false;
   surfaceframe.add("Cylinder",
      "Position":point(0,0,-0.002*infoset.scenescale),
      "Radius":(20*infoset.scenescale),
      "Axis":vector(0,0,-0.00001*infoset.scenescale),
      "Color":color(202/255,139/255,62/255),
      "Resolution":80);
   surfaceframe.add("Cylinder",
      "Radius":(20*infoset.scenescale),
      "Position":point(0,0,-0.005*infoset.scenescale),
      "Axis":vector(0,0,-7.5*infoset.scenescale),
      "Color":color(0,0,0),
      "Resolution":80);
      
   #create viewer figure
   bl=Blob;
   r2=0.3;
   bl.AddSphere(point(0,0,0.95),0.2,0.3);
   bl.AddSegment(point(0,0,0),point(0,0,1),0.23,r2+0.03);
   bl.AddSegment(point(0.22,0,-1),point(0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(-0.22,0,-1),point(-0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(0.26,0,0.55),point(0.66,-0.4,0.94),0.185,r2);
   bl.AddSegment(point(-0.26,0,0.55),point(-0.66,-0.4,0.94),0.185,r2);
   sh=bl.CreateSolidShape(0.06);
   ttf=transformation;ttf.translate(vector(0,0,0.6*infoset.scenescale));ttf.scale(0.6*infoset.scenescale);
   sh.transform(ttf);
   ob=marspositframe.add("SolidObject");
   ob.CreateShape(sh);
   ob.color=color(0.7,0.7,0.7);
      
      
   #create small sun
   smallsun=surfaceframe.add("sphere","Name":"SmallSun","radius":(0.13*infoset.scenescale),"color":color(1,1,0.5),"EnableLight":false);
   
   #create halo
   st_globeradius=str(infoset.globeradius);
   halfcircle=FlatContourSet;
   haloframe=marspositframe.addviewdirframe(point(0,0,0),"haloframe");
   haloframe.CastVolumeShadow=false;
   halfcircle.generate(functor("point("+st_globeradius+"*sin(a),"+st_globeradius+"*cos(a),0)","a"),Pi,0,40);
   halo=haloframe.add("SolidObject","Name":"Halo");
   halo.Revolve(halfcircle,40);
   halo.BlendType=BlendTransparent;
   halo.RenderBack=true;
   halo.DepthMask=DepthMaskDisable;
   halo.EnableLight=false;
   halo.GenerateVertexProperty(functor("color(0,0,1/(1+15*sqr(p.z/"+st_globeradius+")),0.25)","p"),VertexPropertyColor);
   halo.canbuffer=true;
      
   #Create wind directions
   signcolor=color(0.15,0.15,0.15);
   rd=8*infoset.scenescale;
   dirnames=list("O","N","W","Z");
   for i=0 to 3 do {
      frame=surfaceframe.addviewdirframe(radial2point(rd,i*Pi/2,0),"DF_"+dirnames(i));
      frame.add("Text3D","Position":point(-0.28*infoset.scenescale,0.6*infoset.scenescale,0),
         "UnitX":vector(infoset.scenescale,0,0),"UnitY":vector(0,infoset.scenescale),"EnableLight":false,"Color":signcolor,"Content":dirnames(i));
      frame.add("Arrow","Position":point(0,0.2*infoset.scenescale,0),
         "Axis":vector(0,-0.4*infoset.scenescale,0),"NormAxis":vector(0,0,1),"Width":(0.2*infoset.scenescale),"EnableLight":false,"Color":signcolor);
   }
   surfaceframe.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,0,0),radial2point(rd,Pi,0));
   surfaceframe.add("Curve",
      "EnableLight":false,"Color":signcolor,"Size":2,"CurveRenderType":CurveRenderDot
      ).makeline(radial2point(rd,-0.5*Pi,0),radial2point(rd,0.5*Pi,0));
      
   surfaceframe.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*infoset.scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(1,0,0),1.001*infoset.globeradius,80);
   surfaceframe.add("Curve",
      "Color":color(0,0.5,0.5,0.25),"Size":(0.1*infoset.scenescale),"CurveRenderType":CurveRenderRibbon,"BlendType":BlendTransparent
      ).makecircle(point(0,0,0),vector(0,1,0),1.001*infoset.globeradius,80);
   
   
   crv_sundir=surfaceframe.add("Curve","Name":"crv_sundir","EnableLight":false,"Color":color(1,1,0),"Size":2);
   crv_suntrace=surfaceframe.add("Curve","Name":"crv_suntrace","EnableLight":false,"Color":color(1,0.25,0),"Size":3);
   
   #create side panel
   PanelContent=SUB_G_leftpanel.addscreenframe("MarsSolarMovementInfo");
   offsetx=SUB_G_leftpaneloffsett+0.01;
   cl=SUB_G_sidepanelcolor;
   PanelContent.add("Text3D","Content":"Tijd op Aarde (UT):","EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.97,0),"Color":cl);
   PanelContent.add("Clock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.88,0),"Color":cl);
   PanelContent.add("Clock","Type":ClockTypeDate,"Size":0.02,"EnableLight":false,"Position":point(offsetx+0.045,0.78,0),"Color":cl);
   PanelContent.add("Text3D","Content":"Locale tijd op Mars:","EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),"Position":point(offsetx,0.18,0),"Color":cl);
   marsclock=PanelContent.add("Clock","Name":"MarsClock","Type":ClockTypeAnalog,"Size":0.07,"EnableLight":false,"Position":point(offsetx+0.1,0.09,0),"Color":cl,"HasOwnTime":true);
   PanelLines_init(PanelContent,0.7);
   PanelLines_add("Plaats op Mars:");
   PanelLines_add(longit2string(infoset.longit));
   PanelLines_add(lattit2string(infoset.lattit));
   PanelLines_skip;
   PanelLines_add("Positie van de zon:");
   sunpos_az=PanelLines_add("");
   sunpos_h=PanelLines_add("");
   infoset.panelcontent=panelcontent;
   infoset.marsclock=marsclock;
   infoset.sunpos_az=sunpos_az;
   infoset.sunpos_h=sunpos_h;
}


function SUB_MarsSolarMovement_FindSunRise(infoset)
{
   infoset.foundsunrise=false;
   root.time=CurrentTimeUT;
   tmstart=root.time;
   root.time.hour=0;root.time.min=0;root.time.sec=0;
   root.TimeSpeed=4*60;
   sundirprev=vector(0,0,1);
   foundsunrise=false;
   root.TimeSpeed=24*60*60/5000;
   finished=false;
   marspositframe=SUB_G_MarsPositFrame;
   surfaceframe=marspositframe.surface;
   while not(finished) do {
      incrtime;
      root.SC.Universe.transf.reset;
      tf1=marspositframe.totaltransformation;
      tf2=tf1;tf2.invert;
   #calculate position of sun in the local reference system
      sunpoint=tf2*point(0,0,0);
      sundir=vecnorm(sunpoint-point(0,0,0));
      if (sundir^vector(0,0,1)>0) and (sundirprev^vector(0,0,1)<0) then {
         finished=true;
         infoset.foundsunrise=true;
      }
      if root.time.day!=tmstart.day then finished=true;
      sundirprev=sundir;
   }
}


function SUB_MarsSolarMovementAnimate(ilongit,ilattit)
{
   SUB_showloading(true);

   infoset=map;
   infoset.longit=ilongit;
   infoset.lattit=ilattit;
   
   SUB_MarsSolarMovementInit(Ref(infoset));

   distaxislast=-10;
   root.time=time(2008,9,6,0,0,0);
   
   SUB_MarsSolarMovement_FindSunRise(Ref(infoset));
   
   marspositframe=SUB_G_MarsPositFrame;
   surfaceframe=marspositframe.surface;
   minlast=floor(root.time.min/30);
   
   SUB_showloading(false);
   
   root.TimeSpeed=30;
   sundirprev=vector(0,0,-1);
   
   maxangreached=false;
   if not(infoset.foundsunrise) then maxangreached=true;#do not look for max position if sun does not rise or set
   
   showconst=false;
   
   firstloop=false;sunsethappened=false;
   finished=false;
   while not(finished) do {
      if JoystickButtonClicked(JID,6) then finished=true;
      
      if JoystickButtonClicked(JID,9) then {
         showconst=not(showconst);
         ssys_showconstellations(showconst);
         root.SC.Universe.StarBackFrame.ConstellationNames.Visible=showconst;
         root.SC.Universe.StarBackFrame.StarNames.Visible=showconst;
      }
      
      incrspeed=(JoystickGetAxis(JID,4)+1)/2;
      root.TimeSpeed=sqr(incrspeed)*120;
      
      incrtime;
      
      tm=G_MTC+infoset.longit/Pi*12;
      while tm<0 do tm=tm+24;
      while tm>24 do tm=tm-24;
      infoset.marsclock.time.hour=floor(tm);
      infoset.marsclock.time.min=(tm-floor(tm))*60;
      
      #respond to distance throttle   
      distaxis=JoystickGetAxis(JID,3);
      if abs(distaxislast-distaxis)>0.025 then distaxislast=distaxis;
   #   myviewport.camerapos=(1+distaxislast)*cpos0+(1-distaxislast)*cpos1;
   
   #joystick movement   
      a=JoystickGetAxis(JID,0);if abs(a)<0.02 then a=0;
      a=a/30;
      rtdir=myviewport.cameraupdir;
      myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,a);
      myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,a);
      
      a=JoystickGetAxis(JID,1);if abs(a)<0.02 then a=0;
      a=-1*a/30;
      rtdir=vecnorm(myviewport.cameradir*myviewport.cameraupdir);
      ndir=vecrotate(myviewport.cameradir,rtdir,a);
      if (ndir^vector(0,0,1)<0.1) and (ndir^vector(0,0,1)>-0.99) then {
         myviewport.camerapos=point(0,0,0)+vecrotate(myviewport.camerapos-point(0,0,0),rtdir,a);
         myviewport.cameradir=ndir;
      }
      
      
      
   #mechanism to co-move with position on Mars
      root.SC.Universe.transf.reset;
      tf1=marspositframe.totaltransformation;
      tf2=tf1;tf2.invert;
      root.viewports.main.transf=tf2;#apply the inverse transformation to the viewport-> object becomes positioned in the origin
   
   #calculate position of sun in the local reference system
      sunpoint=tf2*point(0,0,0);
      sundir=vecnorm(sunpoint-point(0,0,0));
      surfaceframe.smallsun.position=point(0,0,0)+infoset.globeradius*sundir;
      surfaceframe.crv_sundir.makeline(point(0,0,0)+0.2*infoset.globeradius*sundir,sunpoint);
      sunaz=-0.5*Pi-angle(sundir.x,sundir.y);
      if sunaz<-1*Pi then sunaz=sunaz+2*Pi;
      if sunaz>Pi then sunaz=sunaz-2*Pi;
      sunang=Pi/2-vecangle(sundir,vector(0,0,1));
      infoset.sunpos_az.content="Azimut: "+str(round(sunaz/Pi*180,1))+"°";
      infoset.sunpos_h.content="Hoogte: "+str(round(sunang/Pi*180,1))+"°";
      sunc=sunang;
      if sunc>1 then sunc=1;
      if sunc<0 then sunc=0;
      sss.light0color=color(sunc,sunc,sunc,1);
      sss.VolumeShadowReset;
      sss.VolumeShadowAdd(0,color(0,0,0,0.5*sunc),0.01*infoset.scenescale,16*infoset.scenescale);
      if sundir^vector(0,0,1)>0 then {
         if not(sunsethappened) then firstloop=true;
         if firstloop then surfaceframe.crv_suntrace.addpoint(point(0,0,0)+infoset.globeradius*sundir);
         if ( (sundir^vector(0,0,1)) < (sundirprev^vector(0,0,1)) ) and not(maxangreached) then {#maximum was reached
            angmax=sunang;
            angmaxstr=Str(round(angmax/Pi*180,1))+"°";
            sundirhor=vecnorm(vector(sundirprev.x,sundirprev.y,0));
            crv_maxang=surfaceframe.add("Curve","EnableLight":false,"Color":color(1,0,0),"Size":5,"Arrow2Size":(infoset.globeradius*0.1));
            makepartialcircle(crv_maxang,infoset.globeradius*sundirhor,infoset.globeradius*sundirprev);
            maxangreached=true;
            surfaceframe.add("TextFlat","Position":@point(0.53*infoset.globeradius*vecnorm(sundirprev+sundirhor)),"EnableLight":false,
               "Size":(0.1*infoset.globeradius),"Content":angmaxstr);
            surfaceframe.add("Curve","EnableLight":false,"Color":color(0.8,0,0),"Size":3).
               makeline(point(0,0,0)+0.2*infoset.globeradius*sundirprev,point(0,0,0)+infoset.globeradius*sundirprev);
         }
      }
      if sundir^vector(0,0,1)<0 then {
         sunsethappened=true;
         firstloop=false;
      }
      
      #show trace
      if firstloop then {
         if floor(root.time.min/30)!=minlast then {
            surfaceframe.add("Curve","EnableLight":false,"Color":color(0.5,0,0),"Size":1).
               makeline(point(0,0,0)+0.2*infoset.globeradius*sundir,point(0,0,0)+infoset.globeradius*sundir);
         }
      }
      minlast=floor(root.time.min/30);
      
      sundirprev=sundir;
   
      SUB_testcmd();
      render;
   }
   root.SC.Universe.transf.reset;
   root.viewports.main.transf.reset;
   infoset.panelcontent.dispose;
   marspositframe.dispose;
   ssys_showconstellations(false);
   root.SC.Universe.StarBackFrame.ConstellationNames.Visible=false;
   root.SC.Universe.StarBackFrame.StarNames.Visible=false;
   sss=T_getscene;
   sss.VolumeShadowReset;
   sss.light0color=color(1,1,1,1);
}