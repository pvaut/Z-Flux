#codeblock readtextfile(ScriptDir+"\TOOLS.sci");

function SUB_CreateMarsGeoGlobe(globeframe)
{
   globegeoframe=globeframe.addsubframe("MarsGlobeGeological");
   
   loadstored=true;
   
   resol=60;
   ffc=0;
   
   #MAIN PART OF THE CHART
   surfGeo=globegeoframe.add("Surface","Name":"Geological");
   if not(loadstored) then {
      fnc=functor("radial2point(1,a1,a2)","a1","a2");
      surfGeo.Generate(fnc,0,2*Pi*0.9999,resol,deg2rad(65),deg2rad(-65),resol/2);
      surfGeo.canbuffer=true;
      #correct for mercator projection to 65 degs
      ffc=2*log(tan(Pi/4+deg2rad(65)/2));
      fnc=functor("0.5+log(tan(Pi/4+asin(v.z)/2))/ffc","v");
      surfGeo.GenerateVertexProperty(fnc,VertexPropertyTC2);
      fnc=functor("angle(v.x,v.y)/(2*Pi)","v");
      surfGeo.GenerateVertexProperty(fnc,VertexPropertyTC1);
      saveobject(surfGeo,DataDir+"\MarsProjectData\StoredShapes\SurfGeo.mdl");
   } else {
      ReadObject(surfGeo,DataDir+"\MarsProjectData\StoredShapes\SurfGeo.mdl");
   }
   
   
   #NORTH POLE CAP
   surfGeoNP=globegeoframe.add("Surface","Name":"GeologicalNP");
   if not(loadstored) then {
      fnc=functor("radial2point(0.997,a1,a2)","a1","a2");
      surfGeoNP.Generate(fnc,0,2*Pi*0.9999,resol,deg2rad(55),deg2rad(90),resol/2);
      surfGeoNP.canbuffer=true;
      surfGeoNP.RenderBack=true;surfGeoNP.RenderFront=false;
      #correct for stereographic projection
      x=cos(deg2rad(55));z=sin(deg2rad(55));
      ffc=0.5 / ( x/(1+z) );
      fnc=functor("0.5-ffc*v.y/(1+v.z)","v");
      surfGeoNP.GenerateVertexProperty(fnc,VertexPropertyTC1);
      fnc=functor("0.5+ffc*v.x/(1+v.z)","v");
      surfGeoNP.GenerateVertexProperty(fnc,VertexPropertyTC2);
      saveobject(surfGeoNP,DataDir+"\MarsProjectData\StoredShapes\SurfGeoNP.mdl");
   } else {
      ReadObject(surfGeoNP,DataDir+"\MarsProjectData\StoredShapes\SurfGeoNP.mdl");
   }
   
   
   #SOUTH POLE CAP
   surfGeoSP=globegeoframe.add("Surface","Name":"GeologicalSP");
   if not(loadstored) then {
      fnc=functor("radial2point(0.997,a1,a2)","a1","a2");
      surfGeoSP.Generate(fnc,0,2*Pi*0.9999,resol,deg2rad(-55),deg2rad(-90),resol/2);
      surfGeoSP.canbuffer=true;
      #correct for stereographic projection
      x=cos(deg2rad(-55));z=sin(deg2rad(-55));
      ffc=0.5 / ( x/(1-z) );
      fnc=functor("0.5-ffc*v.y/(1-v.z)","v");
      surfGeoSP.GenerateVertexProperty(fnc,VertexPropertyTC1);
      fnc=functor("0.5-ffc*v.x/(1-v.z)","v");
      surfGeoSP.GenerateVertexProperty(fnc,VertexPropertyTC2);
      saveobject(surfGeoSP,DataDir+"\MarsProjectData\StoredShapes\SurfGeoSP.mdl");
   } else {
      ReadObject(surfGeoSP,DataDir+"\MarsProjectData\StoredShapes\SurfGeoSP.mdl");
   }
   
   
   #load textures
   
   txGeo=globegeoframe.createtexture("MarsGeologic",DataDir+"\textures\MarsGeologic.jpg");
   surfGeo.texture=txGeo.name;
   
   txGeoNP=globegeoframe.createtexture("MarsGeologicNP",DataDir+"\textures\MarsGeologicNP.jpg");
   surfGeoNP.texture=txGeoNP.name;
   
   txGeoSP=globegeoframe.createtexture("MarsGeologicSP",DataDir+"\textures\MarsGeologicSP.jpg");
   surfGeoSP.texture=txGeoSP.name;
}

