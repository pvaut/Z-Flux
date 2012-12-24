function createlogo(frame)
{
   col1=color(0.2,0.7,0.2);#text color
   col2=color(0.6,1,0.1);#arrow color

   dptt=0.1;#depth of text
   dpta=0.12;#depth of arrow
   mcf=0.25;#mirror color darkening factor
   
   f1=frame.addsubframe("f2");
   f1.transf.Xaxis=vector(1,0,0);
   f1.transf.Yaxis=vector(0,1,0);
   f1.transf.Zaxis=vector(0,0,1);
   t1=f1.add("Text3D","Content":"Z  Flux","Color":col1,"Depth":dptt,
      "Position":point(0,0.25,0.5*dptt),
      "UnitX":vector(1,0,0),"UnitY":vector(0,1,0));
      
   t1.SpecularColor=color(0.5,0.5,0.5);
   t1.SpecularValue=30;
   
   
   a1=f1.add("Arrow","Color":col2,"Thickness":dpta,"Width":0.1,
      "position":point(0,0.06,0),
      "Axis":vector(3,0,0),"NormAxis":vector(0,0,1));
   
   b1=f1.add("Arrow","Color":col2,"Thickness":dpta,"Width":0.1,
      "position":point(0,2*0.27,0),
      "Axis":vector(1.1,0,0),"NormAxis":vector(0,0,1));

   
   f2=frame.addsubframe("f2");
   f2.transf.Xaxis=vector(1,0,0);
   f2.transf.Yaxis=vector(0,-1,0);
   f2.transf.Zaxis=vector(0,0,-1);
   t2=f2.add("Text3D","Content":"Z  Flux","Color":(mcf*col1+color(0,0,0)),"Depth":dptt,"renderback":true,"renderfront":true,
      "Position":point(0,0.25,0.5*dptt),
      "UnitX":vector(1,0,0),"UnitY":vector(0,1,0));
   
   a1=f2.add("Arrow","Color":(mcf*col2+color(0,0,0)),"Thickness":dpta,"Width":0.1,
      "position":point(0,0.06,0),
      "Axis":vector(3,0,0),"NormAxis":vector(0,0,1));
   
   b1=f2.add("Arrow","Color":(mcf*col2+color(0,0,0)),"Thickness":dpta,"Width":0.1,
      "position":point(0,0.27,0),
      "Axis":vector(1.1,0,0),"NormAxis":vector(0,0,1));
      
   s2=f2.add("Surface","enablelight":false);
   s2.BlendType=BlendTranslucent;s2.DepthMask=DepthMaskDisable;
   s2.dim(2,2);
   dz=-0.07;
   s2.points(0,0)=point(0,0,dz);s2.colors(0,0)=color(0,0,0,0);
   s2.points(1,0)=point(4,0,dz);s2.colors(1,0)=color(0,0,0,0);
   s2.points(1,1)=point(4,1,dz);s2.colors(1,1)=color(0,0,0,1);
   s2.points(0,1)=point(0,1,dz);s2.colors(0,1)=color(0,0,0,1);
}
