codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


facetypes=list;
function getfacetypenr(ptlist)
{
   for i=0 to facetypes.size-1 do
   {
      if facetypes(i).ptcount==ptlist.size then return(i);
   }
   newfacetype=map("ptcount":(ptlist.size));
   facetypes.add(newfacetype);
   return(facetypes.size-1);
}

function createpolyhedron(frame,objname,sizefactor,createvertices,transparent)
{
   shape=SolidShape;
   content=readtextfile(datadir+"\Scripts\Mathematics\_Polyhedra\"+objname+".raw");
   facetypes.clear;
   facelist=list;
   maxdist=0.001;
   while content!="" do {
      line=content.split("~n");
      ptcount=ToScalar(line.split(" "));
      if ptcount>0 then {
         ptlist=list;
         for i=0 to ptcount-1 do {
            pt=point(ToScalar(line.split(" ")),ToScalar(line.split(" ")),ToScalar(line.split(" ")));
            ptlist.add(pt);
            maxdist=max(maxdist,distance(point(0,0,0),pt));
         }
         if ptlist.size>2 then facelist.add(ptlist);
      }
   }
   foreach ptlist in facelist do {
      foreach pt in ptlist do pt=@point((sizefactor/maxdist)*@vector(pt));
      facenr=getfacetypenr(ptlist);
      shape.AddFace(ptlist,facenr);
   }
   
   if createvertices then {
      vertices=list;
      verticesmap=map;
      linesmap=map;
      foreach ptlist in facelist do {
         prevptidx=-1;ptidx=0;
         ptlist.add(ptlist(0));
         foreach pt in ptlist do {
            st=Str(round(pt.x,5))+";"+Str(round(pt.y,5))+";"+Str(round(pt.z,5));
            if not(verticesmap.IsPresent(st)) then {
               ptidx=vertices.size;
               verticesmap.AddItem(st:ptidx);
               vertices.add(pt);
               frame.add("Sphere","radius":0.15,"resolution":15,"position":pt,
                  "color":color(0.5,0.5,0.5),"SpecularColor":color(0.5,0.5,0.5),"SpecularValue":25);
            } else ptidx=verticesmap.Get(st);
            if prevptidx>=0 then {
               st=str(prevptidx)+"-"+str(ptidx);
               if not(linesmap.IsPresent(st)) then {
                  linesmap.addItem(st:1);
                  pt1=vertices(prevptidx);pt2=vertices(ptidx);
                  frame.add("Cylinder","radius":0.07,"Resolution":10,"Position":pt1,"Axis":(pt2-pt1),
                  "color":color(0.5,0.5,0.5),"SpecularColor":color(0.5,0.5,0.5),"SpecularValue":25);
               }
            }
            prevptidx=ptidx;
         }
      }
   }
   
   obj=frame.add("SolidObject");
   obj.RenderBack=true;
   obj.CreateShape(shape);
   obj.SpecularColor=color(0.25,0.25,0.25);
   obj.SpecularValue=15;
   if transparent then {
      obj.BlendType=BlendTransparent;
      obj.DepthMask=DepthMaskDisable;
   }
   
   if not(transparent) then {
      for i=0 to colorlist.size-1 do
         obj.SetColor(i,colorlist(i));
   } else {
      for i=0 to colorlist.size-1 do
         obj.SetColor(i,colorlist(i)+2*color(0,0,0,0));
   }
}

function showobject(objname)
{
   zoomframe.clearobjects;
   finished=false;first=true;
   menu.wasmodified=true;
   while not(finished) do {
      if menu.WasModified then {
         output("create");
         zoomframe.clearobjects;
         createpolyhedron(zoomframe,objname,5,menu.Checked("ShowFrameWork"),menu.Checked("Transparent"));
         if first then {
            fadein(0.25);
            root.showcontrols=true;
            first=false;
         }
      }
      if UIIsKeyDown("escape") then finished=true;
      if JoystickButtonDown(0,6) then finished=true;
      dr1=-1*vp.cameradir;
      dr2=vecnorm(dr1*vector(0,1,0));
      dr3=vecnorm(dr1*dr2);
      sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
      render;
   }
   fadeout(0.25);
   root.showcontrols=false;
   zoomframe.clearobjects;
}



sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.1,0.1,0.1);
overviewframe=sss.addsubframe("overviewframe");
zoomframe=sss.addsubframe("zoomframe");
sss.light0pos=point(5000,5000,10000);

vp=root.Viewports.main;


colorlist=list;
colorlist.add(color(0,0.75,0));
colorlist.add(color(1,0,0));
colorlist.add(color(0,0,1));
colorlist.add(color(0.6,0.6,0));
colorlist.add(color(0,0.6,0.6));
colorlist.add(color(0.7,0,0.7));
foreach cl in colorlist do cl=cl+color(0.7,0.7,0.7);



objectfiles=GetFileList(datadir+"\Scripts\Mathematics\_Polyhedra\*.raw",false);

ctx=6;
cty=4;

cursorx=0;cursory=0;

objframes=list;
ctr=0;
foreach objectfile in objectfiles do {
   ix=floor(ctr/cty);
   iy=ctr-cty*ix;
   objectfile.invert;objectfile.split(".");objectfile.invert();
   objframe=overviewframe.addsubframe(objectfile);
   objframe.transf.origin=point((ix-(ctx-1)/2)*2.8,(iy-(cty-1)/2)*3,0);
 
   txt=objframe.add("Text3D","Content":objectfile,"UnitX":vector(0.25,0,0),"UnitY":vector(0,0.25,0),
        "EnableLight":false,"color":color(0.7,0.7,0.7));
   txt.position=point(-0.5*0.25*txt.GetSizeX,-1.3,0);
   
   subframe1=objframe.addsubframe("SubFrame");
   subframe1.motion=MotionRotate.create(subframe1);
   subframe1.motion.NormDir=vector(0,1,0);subframe1.motion.RotSpeed=0.2;
   subframe2=subframe1.addsubframe("SubFrame");
   subframe2.motion=MotionRotate.create(subframe2);
   subframe2.motion.NormDir=vector(0,0,1);subframe2.motion.RotSpeed=0.06;
   createpolyhedron(subframe2,objectfile,1,false,false);
   objframes.add(objframe);
   ctr=ctr+1;
}

cursorframe=overviewframe.addsubframe("cursorframe");
crv=cursorframe.add("Curve","color":color(1,0.5,0),"size":2,"enablelight":false);
fc=1.5;
crv.addpoint(point(-1*fc,-1*fc,0));
crv.addpoint(point(-1*fc,1*fc,0));
crv.addpoint(point(1*fc,1*fc,0));
crv.addpoint(point(1*fc,-1*fc,0));
crv.addpoint(point(-1*fc,-1*fc,0));

menu=T_createmenu;
menu.Add("",'Show framework',"ShowFrameWork",true);
menu.Add("",'Transparent',"Transparent",true);
root.showcontrols=false;


function initcamera()
{
   vp.camerapos=point(0,0,20);
   vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
   vp.cameraupdir=vector(0,1,0);
   vp.enableusernavigation=true;
   vp.EnableUserTimeControl=true;
   vp.EnableUserStop=true;
   vp.NearClipPlane=0.1;
   vp.FarClipPlane=200;
   vp.FocalDistance=20;
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
}

root.timespeedfactor=1;
initcamera;
while true do {
   incrtime;
   st=UIGetKeyPressed;
   if st=="right" then cursorx=cursorx+1;
   if st=="left" then cursorx=cursorx-1;
   if cursorx<0 then cursorx=cursorx+ctx;
   if cursorx>=ctx then cursorx=cursorx-ctx;
   if st=="up" then cursory=cursory+1;
   if st=="down" then cursory=cursory-1;
   if cursory<0 then cursory=cursory+cty;
   if cursory>=cty then cursory=cursory-cty;
   cursorframe.transf.origin=point((cursorx-(ctx-1)/2)*2.8,(cursory-(cty-1)/2)*3,0);
   if st=="RETURN" then {
      objectnr=cursory+cursorx*cty;
      if (objectnr>=0) and (objectnr<objectfiles.size) then {
         fadeout(0.25);
         overviewframe.visible=false;
         vp.EnableUserStop=false;
         showobject(objectfiles(objectnr));
         overviewframe.visible=true;
         initcamera;
         fadein(0.25);
         vp.EnableUserStop=true;
      }
   }
   render;
}
