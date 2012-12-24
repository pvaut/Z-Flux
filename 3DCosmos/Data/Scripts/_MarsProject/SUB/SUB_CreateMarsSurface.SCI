



function SUB_CreateMarsSurface(parentframe,longit,lattit,rs1,rs2)
{

   while longit<0 do longit=longit+2*Pi;

   surfaceframe=parentframe.addsubframe("SurfaceFrame");

   #MOLA DATA IS IN METER
   MarsRadius=3396.0;#MOLA average sphere radius, in km
   hfactor=2;#Height multiplication factor
   
   hcorr=1000*(3396.2-3376.2)-1000*(3396.2-3376.2)*sqr(cos(lattit));

   mapresol=32;
   
   loc_cent=radial2point(1,longit,lattit);
   loc_dirz=@vector(loc_cent);
   loc_dirx=vecnorm(vector(-1*sin(longit),cos(longit),0));
   loc_diry=vecnorm(loc_dirz*loc_dirx);
   
   normal=vecnorm(radial2point(1,longit,lattit)-point(0,0,0));
   
   terrainframe=surfaceframe.addsubframe("TerrainFrame");
   terrain=terrainframe.add("Surface","color":color(202/255,139/255,62/255));
   terrain.canbuffer=true;
   topo=matrix;
   pos_longit=longit;
   pos_lattit=lattit;
   idx_longit=floor(pos_longit/(2*Pi)*360*mapresol);
   idx_lattit=floor((Pi/2-pos_lattit)/Pi*180*mapresol);
   filename=DataDir+"\topo\Mars_Topo_"+str(mapresol)+".img";
   topo.LoadFile(filename,
      360*mapresol,idx_longit-rs1,idx_longit+rs1,
      180*mapresol,idx_lattit-rs2,idx_lattit+rs2,
      2);
   topo.addscalar(hcorr);
   terrain.Position=point(0,0,0);
   terrain.CreateTopoSphere(topo,
      longit-deg2rad(rs1/mapresol),longit+deg2rad(rs1/mapresol),
      lattit-deg2rad(rs2/mapresol),lattit+deg2rad(rs2/mapresol),
      hfactor/(1000*MarsRadius),1);
      
   h_central=topo(rs1,rs2);
   h_min=topo.GetMinVal;
   h_max=topo.GetMaxVal;
      
      
   #Create level at 0 meter
   level=terrainframe.add("Surface","Name":"Level0","Color":color(0,0.5,1,0.5));
   level.BlendType=BlendTranslucent;level.DepthMask=DepthMaskDisable;
   level.renderback=true;
   level.canbuffer=true;
   st_rd=str(1+0*hfactor/(1000*MarsRadius));
   fnc=functor("radial2point("+st_rd+",l,b)","l","b");
   level.Generate(fnc,
      longit-1.03*deg2rad(rs1/mapresol),longit+1.03*deg2rad(rs1/mapresol),60,
      lattit-1.03*deg2rad(rs2/mapresol),lattit+1.03*deg2rad(rs2/mapresol),60
      );
   level.visible=false;

   #Create level at 10000 meter
#   level=terrainframe.add("Surface","Name":"Level10000","Color":color(0,1,0,0.5));
#   level.BlendType=BlendTranslucent;level.DepthMask=DepthMaskDisable;
#   level.renderback=true;
#   level.canbuffer=true;
#   st_rd=str(1+10000*hfactor/(1000*MarsRadius));
#   fnc=functor("radial2point("+st_rd+",l,b)","l","b");
#   level.Generate(fnc,
#      longit-1.03*deg2rad(rs1/mapresol),longit+1.03*deg2rad(rs1/mapresol),60,
#      lattit-1.03*deg2rad(rs2/mapresol),lattit+1.03*deg2rad(rs2/mapresol),60
#      );
      
      
      
      
   sidecolor=2*color(0,0,0)+terrain.color;
   
   side1=surfaceframe.add("Surface","Name":"Side1");
   side1.renderback=true;
   side1.canbuffer=true;
   side1.color=sidecolor;
   side1.dim(topo.dim1,2);
   for i=0 to topo.dim1-1 do {
      pt=terrain.points(i,0);
      side1.points(i,0)=pt;
      ttt=0.95-normal^@vector(pt);
      side1.points(i,1)=pt+ttt*normal;
      side1.normals(i,0)=loc_diry;
      side1.normals(i,1)=loc_diry;
   }
   
   side3=surfaceframe.add("Surface","Name":"Side3");
   side3.renderback=true;
   side3.canbuffer=true;
   side3.color=sidecolor;
   side3.dim(topo.dim1,2);
   for i=0 to topo.dim1-1 do {
      pt=terrain.points(i,topo.dim2-1);
      side3.points(i,0)=pt;
      ttt=0.95-normal^@vector(pt);
      side3.points(i,1)=pt+ttt*normal;
      side3.normals(i,0)=loc_diry;
      side3.normals(i,1)=loc_diry;
   }
   
   
   side2=surfaceframe.add("Surface","Name":"Side2");
   side2.renderback=true;
   side2.canbuffer=true;
   side2.color=sidecolor;
   side2.dim(topo.dim2,2);
   for i=0 to topo.dim2-1 do {
      pt=terrain.points(0,i);
      side2.points(i,0)=pt;
      ttt=0.95-normal^@vector(pt);
      side2.points(i,1)=pt+ttt*normal;
      side2.normals(i,0)=loc_dirx;
      side2.normals(i,1)=loc_dirx;
   }
   
   
   side4=surfaceframe.add("Surface","Name":"Side4");
   side4.renderback=true;
   side4.canbuffer=true;
   side4.color=sidecolor;
   side4.dim(topo.dim2,2);
   for i=0 to topo.dim2-1 do {
      pt=terrain.points(topo.dim1-1,i);
      side4.points(i,0)=pt;
      ttt=0.95-normal^@vector(pt);
      side4.points(i,1)=pt+ttt*normal;
      side4.normals(i,0)=loc_dirx;
      side4.normals(i,1)=loc_dirx;
   }
      
      
   
   pt1=radial2point(1,longit+deg2rad(rs1/mapresol),lattit);
   terrainframe.add("Text3D","Content":"O","EnableLight":false,"Color":color(0.6,0.6,0.6),
      "Position":(pt1+0.01*loc_dirx-0.02*loc_diry),
      "UnitX":(0.05*loc_dirx),"UnitY":(0.05*loc_diry));
   pt1=radial2point(1,longit-deg2rad(rs1/mapresol),lattit);
   terrainframe.add("Text3D","Content":"W","EnableLight":false,"Color":color(0.6,0.6,0.6),
      "Position":(pt1-0.045*loc_dirx-0.02*loc_diry),
      "UnitX":(0.05*loc_dirx),"UnitY":(0.05*loc_diry));
   pt1=radial2point(1,longit,lattit-deg2rad(rs2/mapresol));
   terrainframe.add("Text3D","Content":"Z","EnableLight":false,"Color":color(0.6,0.6,0.6),
      "Position":(pt1-0.02*loc_dirx-0.04*loc_diry),
      "UnitX":(0.05*loc_dirx),"UnitY":(0.05*loc_diry));
   pt1=radial2point(1,longit,lattit+deg2rad(rs2/mapresol));
   terrainframe.add("Text3D","Content":"N","EnableLight":false,"Color":color(0.6,0.6,0.6),
      "Position":(pt1-0.02*loc_dirx+0.01*loc_diry),
      "UnitX":(0.05*loc_dirx),"UnitY":(0.05*loc_diry));
      
   radius=3396.2;
   udist=200.0/radius;
   pt1=radial2point(1,longit,lattit+deg2rad(rs2/mapresol))+0.01*loc_diry+0.05*loc_dirx;
   terrainframe.add("Rectangle","Position":pt1,"Axis1":(udist*loc_dirx),"Axis2":(0.01*loc_diry));
   terrainframe.add("Text3D","Position":(pt1+0.025*loc_diry),"Content":"200km","UnitX":(0.02*loc_dirx),"UnitY":(0.02*loc_diry));
   
   
   myviewport.camerapos=loc_cent+0.4*(loc_dirz-loc_diry);
   myviewport.cameradir=vecnorm(loc_cent-myviewport.camerapos);
   myviewport.cameraupdir=loc_dirz;
   
   
   return(map("Frame":surfaceframe,
      "Loc_cent":loc_cent,
      "Loc_dirx":loc_dirx,
      "Loc_diry":loc_diry,
      "Loc_dirz":loc_dirz,
      "h_central":h_central,
      "h_min":h_min,
      "h_max":h_max,
      "hconv":(hfactor/(1000*MarsRadius))
      ));
}




function SUB_RenderMarsSurface(rootframe,longit,lattit,rs1,rs2)
{
   SUB_showloading(true);

   maprs=SUB_CreateMarsSurface(rootframe,longit,lattit,rs1,rs2);
   surfaceframe=maprs.Frame;
   loc_cent=maprs.Loc_cent;
   loc_dirx=maprs.Loc_dirx;
   loc_diry=maprs.Loc_diry;
   loc_dirz=maprs.Loc_dirz;
   hconv=maprs.hconv;
   
   finished=false;
   distaxislast=-1;
   
   PanelContent=SUB_G_leftpanel.addscreenframe("MarsSurfaceInfo");
   PanelLines_init(PanelContent,0.9);
   PanelLines_add("Plaats op Mars:");
   PanelLines_add("Lengte: "+longit2string(longit));
   PanelLines_add("Breedte: "+lattit2string(lattit));
   PanelLines_skip;
   PanelLines_add("Hoogte:");
   PanelLines_add("Centrum: "+str(round(maprs.h_central,0))+"m");
   PanelLines_add("Minimum: "+str(round(maprs.h_min,0))+"m");
   PanelLines_add("Maximum: "+str(round(maprs.h_max,0))+"m");
   
   #make cursor stick
   stick=maprs.Frame.addsubframe("Stick");
   for i=-5 to 5 do {
      cl=color(0,0.75,0);
      if i/2==floor(i/2) then cl=color(0,0.25,0);
      h=i*5000;
      stick.add("cylinder","Position":point(0,0,h*hconv),"Radius":0.002,"Axis":vector(0,0,5000*hconv),"Color":cl);
   }
   stick.transf.origin=loc_cent;
   stick.transf.Xaxis=loc_dirx;
   stick.transf.Yaxis=loc_diry;
   stick.transf.Zaxis=loc_dirz;
   stick.visible=false;
   
   
   SUB_showloading(false);
   
#   message("started");

   level0=root.SC.Universe.SurfaceFrame.TerrainFrame.Level0;
  
   while not(finished) do {
      if JoystickButtonClicked(0,7) then finished=true;
     
      if JoystickButtonClicked(JID,9) then level0.visible=not(level0.visible);
      if JoystickButtonClicked(JID,11) then stick.visible=not(stick.visible);
     
      distaxis=JoystickGetAxis(JID,3);
      if abs(distaxislast-distaxis)>0.025 then distaxislast=distaxis;
      dist=0.1+0.4*sqr(1+distaxislast);
      myviewport.camerapos=loc_cent+dist*vecnorm(myviewport.camerapos-loc_cent);
     
      a=JoystickGetAxis(JID,0);if abs(a)<0.02 then a=0;
      a=a/30;
      rtdir=myviewport.cameraupdir;
      myviewport.camerapos=loc_cent+vecrotate(myviewport.camerapos-loc_cent,rtdir,a);
      myviewport.cameradir=vecrotate(myviewport.cameradir,rtdir,a);
      
      a=JoystickGetAxis(JID,1);if abs(a)<0.02 then a=0;
      a=-1*a/30;
      rtdir=vecnorm(myviewport.cameradir*myviewport.cameraupdir);
      ndir=vecrotate(myviewport.cameradir,rtdir,a);
      if (ndir^loc_dirz<-0.08) and (ndir^loc_dirz>-0.99) then {
         myviewport.camerapos=loc_cent+vecrotate(myviewport.camerapos-loc_cent,rtdir,a);
         myviewport.cameradir=ndir;
      }
     
      sss.light0pos=loc_cent+100*(
         vecnorm(myviewport.cameradir*myviewport.cameraupdir)
         -0.5*myviewport.cameradir
         +0.2*myviewport.cameraupdir);
     
      SUB_testcmd();
      render;
   }
   
   surfaceframe.dispose;
   PanelContent.dispose;
}

