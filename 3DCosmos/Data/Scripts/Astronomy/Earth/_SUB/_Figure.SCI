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

