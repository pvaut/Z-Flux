function CreateFigure(figframe,figsize)
{
   bl=Blob;
   r2=0.3;
   bl.AddSphere(point(0,0,0.95),0.2,0.3);
   bl.AddSegment(point(0,0,0),point(0,0,1),0.23,r2+0.03);
   bl.AddSegment(point(0.22,0,-1),point(0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(-0.22,0,-1),point(-0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(0.26,0,0.55),point(0.66,-0.4,0.94),0.185,r2);
   bl.AddSegment(point(-0.26,0,0.55),point(-0.66,-0.4,0.94),0.185,r2);
   sh=bl.CreateSolidShape(0.06);
   ttf=transformation;ttf.translate(vector(0,0,0.65*figsize));ttf.scale(0.6*figsize);
   ttf.rotate(vector(0,0,1),Pi/2);
   sh.transform(ttf);
   ob=figframe.add("SolidObject");
   ob.CreateShape(sh);
   ob.color=color(0.7,0.7,0.7);
   ob.SpecularColor=color(1.0,0.6,0.0,0.5);
   ob.SpecularValue=30;
   return(ob);
}

function CreateEarthFrame() {
   radius=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering.Earth.Radius*1.005;
   eglobe=root.SC.Universe.SolarSystem.Earth.Inclin.Globe.GlobeRendering;
#   eglobe.add("cylinder","Position":point(0,0,-1.3*radius),"Axis":vector(0,0,2.6*radius),"Radius":(0.05*radius),"IsClosed":true,
#       "color":color(0.5,0.5,0.5));
   gridframe=eglobe.addsubframe("grid");
   for i=-8 to 8 do {
      a=deg2rad(10*i);
      crv=gridframe.add("Curve");
      crv.makecircle(point(0,0,radius*sin(a)),vector(0,0,1),radius*cos(a),200);
      crv.CurveRenderType=CurveRenderSmooth;
      crv.Size=0.005*radius;
      crv.color=color(1,0,0,0.5);
      if i==0 then { crv.Size=0.01*radius; crv.color=color(1,0,0,1); }
   }
   for i=0 to 17 do {
      a=deg2rad(10*i);
      crv=gridframe.add("Curve");
      crv.makecircle(point(0,0,0),vector(cos(a),sin(a),0),radius,200);
      crv.CurveRenderType=CurveRenderSmooth;
      crv.Size=0.005*radius;
      crv.color=color(1,0,0,0.5);
      if (i==0) or (i==9) then { crv.Size=0.01*radius; crv.color=color(1,0,0,1); }
   }
}


function NextTimePoint() {
   tm=root.time;tm1=time;
   mindiff=10000000;
   tm0=time(tm.year,3,21,0,0,0);
   if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
   tm0=time(tm.year,6,21,0,0,0);
   if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
   tm0=time(tm.year,9,21,0,0,0);
   if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
   tm0=time(tm.year,12,21,0,0,0);
   if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
   tm0=time(tm.year+1,3,21,0,0,0);
   if (tm0-tm>0) and (tm0-tm<mindiff) then { mindiff=tm0-tm; tm1=tm0; }
   root.time=tm1;
}