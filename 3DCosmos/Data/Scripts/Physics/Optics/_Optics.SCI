
#Initialisation of some internal variables
createvar(opticframe);
ListOpticalObjects=list;
ListRays=list;

#Call this function to define the frame wherein the optics are defined and calculated
function SetOpticFrame(iframe)
{
   opticframe=iframe;
}

#Converts Hue-Saturation-Intensity to a color variable
function HSI2Color(H,S,I)
{
   r=0;g=0;b=0;
   if (H>=0) and (H<=2*Pi/3) then
   {
      b=(1-S)/3;
      r=(1.0+S*cos(H)/cos(2*Pi/6-H))/3;
      g=1.0-r-b;
   }
   if (H>=2*Pi/3) and (H<=4*Pi/3) then
   {
      r=(1-S)/3;
      g=(1.0+S*cos(H-2*Pi/3)/cos(2*Pi/6-H+2*Pi/3))/3;
      b=1.0-r-g;
   }
   if (H>=4*Pi/3) and (H<=6*Pi/3) then
   {
      g=(1-S)/3;
      b=(1.0+S*cos(H-4*Pi/3)/cos(2*Pi/6-H+4*Pi/3))/3;
      r=1.0-g-b;
   }
   r=r*I*3;
   g=g*I*3;
   b=b*I*3;
   if (r<0) then r=0; if (r>1) then r=1;
   if (g<0) then g=0; if (g>1) then g=1;
   if (b<0) then b=0; if (b>1) then b=1;
   return(color(r,g,b));
}



#Returns an object that represents properties of a reflecting object
function CreateReflectingProperties()
{
   theprop=map;
   theprop.color=color(0.6,0.6,0.6);
   return(theprop);
}

#Returns an object that represents properties of a refracting object
function CreateRefractingProperties()
{
   theprop=map;
   theprop.color=color(0.5,0.7,1,0.5);
   theprop.index=0.8;
   theprop.freqindex=0;
   return(theprop);
}

#Returns an object that represents properties of a light ray
function CreateRayProperties()
{
   theprop=map;
   theprop.color=color(1.0,0.3,0);
   theprop.type=0;#0=solid 1=dotted 2=transparent
   theprop.frequency=0;
   theprop.lastrayinvertlength=0;
   theprop.alsoreflectbouncenrs=list;#list of refracting bounce #s that should also reflect
   return(theprop);
}


#Adds a reflecting object to the scene, defined as a SolidShape
function AddReflectingObject(TheShape,TheReflectingProperties)
{
   TheObj=map;
   TheObj.shape=TheShape;
   TheObj.type=0;#reflecting
   TheObj.properties=TheReflectingProperties;
   ListOpticalObjects.add(TheObj);

   #create physical representation
   myobj=opticframe.add("SolidObject");
   myobj.color=TheReflectingProperties.color;
   myobj.SpecularColor=color(0.5,0.5,0.5);
   myobj.SpecularValue=60;
   myobj.CreateShape(TheShape);
   return(myobj);
}


#Adds a refracting object to the scene, defined as a SolidShape
function AddRefractingObject(TheShape,TheRefractingProperties)
{
   TheObj=map;
   TheObj.shape=TheShape;
   TheObj.type=1;#refracting
   TheObj.properties=TheRefractingProperties;
   ListOpticalObjects.add(TheObj);

   #create physical representation
   myobj=opticframe.add("SolidObject");
   myobj.color=TheRefractingProperties.color;
   myobj.SpecularColor=color(1,1,1);
   myobj.SpecularValue=60;
   myobj.BlendType=BlendTranslucent;
   #myobj.BlendType=BlendTransparent;
   myobj.DepthMask=DepthMaskDisable;
   #myobj.RenderBack=true;
   myobj.CreateShape(TheShape);
   return(myobj);
}

#Adds a single ray to the raytracing engine
function AddRay(TheOrigin,TheDirection,TheRayProperties)
{
   TheRay=map;
   TheRay.origin=TheOrigin;
   TheRay.direction=TheDirection;
   TheRay.properties=TheRayProperties;
   ListRays.add(TheRay);
}


#Adds a ray to the raytracing engine that consists of several frequencies
function AddMultiColorRay(TheOrigin,TheDirection,TheRayProperties)
{
   for raynr=-4 to 4 do {
      rayprop=TheRayProperties;
      rayprop.frequency=raynr/4;
      rii=sqrt(1.05-abs(rayprop.frequency));
      rayprop.color=HSI2Color(2.3*(rayprop.frequency+1),0.8,0.8*rii);
      AddRay(TheOrigin,TheDirection,rayprop);
   }
}

#Internal function
function makerayline(myray,mystartpt,myendpt)
{
   c=opticframe.add("Curve");
   c.color=myray.properties.color;
   c.EnableLight=false;
   if myray.properties.type==1 then {
      c.CurveRenderType=CurveRenderDot;
   }
   if myray.properties.type==2 then {
      c.BlendType=BlendTransparent;
      c.DepthMask=DepthMaskDisable;
   }
   c.makeline(mystartpt,myendpt);
}

#Call this function to execute the ray tracing
function TraceRays()
{
   foreach ray in ListRays do {
      ray_start=ray.origin;
      ray_dir=ray.direction;
      
      finished=false;
      bouncect=0;
      while not(finished) do {
         bouncect=bouncect+1;
         tmin=99999;nmmin=vector(0,0,0);createvar(prop);type=0;
         t=0;nm=vector(0,0,0);
         finished=true;
         
         foreach obj in ListOpticalObjects do {
            if obj.shape.Intersect(ray_start,ray_dir,t,nm) then {
               finished=false;
               if t<tmin then {
                  tmin=t;
                  nmmin=nm;
                  prop=Ref(obj.properties);
                  type=obj.type;
               }
            }
         }
         nm=nmmin;t=tmin;
         if finished then t=6;
         
         
         makerayline(ray,ray_start,ray_start+t*ray_dir);
         if not(finished) then {
            if type==0 then {#reflecting object
               ray_start=ray_start+(t-0.000001)*ray_dir;
               costh1=-1*nm^ray_dir;
               ray_dir=2*nm+ray_dir;
            }
            if type==1 then {#refracting object
               frcoef=prop.index+prop.freqindex*ray.properties.frequency;
               costh1=-1*nm^ray_dir;
               fr=frcoef;
               if costh1<0 then { fr=1/frcoef; costh1=-1*costh1; nm=-1*nm; }
               costh2sq=1-sqr(fr)*(1-sqr(costh1));
               
               
               
               if costh2sq>=0 then {#refraction
               
                  #check if reflection should also be rendered
                  reflectbouncelist=ray.properties.alsoreflectbouncenrs;
                  if (reflectbouncelist.size()>0) then {
                     if (reflectbouncelist(0)==bouncect) then {
                        newprop=ray.properties;
                        newprop.color=2*newprop.color+color(0,0.5,0);
                        reflectbouncelist.del(0);
                        foreach nr in reflectbouncelist do nr=nr-bouncect;
                        newprop.alsoreflectbouncenrs=reflectbouncelist;
                        AddRay(ray_start+(t-0.000001)*ray_dir,vecnorm(2*nm+ray_dir),newprop);
                     }
                  }
               
                  #perform refraction
                  ray_start=ray_start+(t+0.000001)*ray_dir;
                  costh2=sqrt(costh2sq);
                  ray_dir=fr*ray_dir+(fr*costh1-costh2)*nm;
                  ray_dir=vecnorm(ray_dir);
               }
               
               
               else
               
               
               {#internal reflection
                  ray_start=ray_start+(t-0.000001)*ray_dir;
                  ray_dir=vecnorm(2*nm+ray_dir);
               }
            }
         }
         if bouncect>15 then finished=true;
      }
      
      if ray.properties.lastrayinvertlength>0 then {
         c=opticframe.add("Curve");
         c.color=2*ray.properties.color+color(0,0,0);
         c.CurveRenderType=CurveRenderDot;
         c.makeline(ray_start,ray_start-ray.properties.lastrayinvertlength*ray_dir);
      }
   }
}

#Add a spherical mirror to the optics set
function AddSphericalMirror(mycenter,mydir,myradius,myholeradius,myfocaldistance,mydepth,TheReflectingProperties)
{
   resol1=40;
   
   cont1=FlatContourSet;
   ang1=asin(myholeradius/myfocaldistance);
   ang2=asin(myradius/myfocaldistance);
   oo=myfocaldistance*cos(ang2);
   for i=0 to resol1 do {
      a=ang1+i/resol1*(ang2-ang1);
      cont1.addpoint(
         point(-1*myfocaldistance*cos(a)+oo,myfocaldistance*sin(a))
           ,vector(cos(a),-1*sin(a)));
   }
   cont1.addpoint(point(0,myradius));
   cont1.addpoint(point(mydepth,myradius));
   cont1.addpoint(point(mydepth,myholeradius));
   cont1.addpoint(point(-1*myfocaldistance*cos(ang1)+oo,myfocaldistance*sin(ang1)));
   cont1.close();
   cont1.calcflatnormals();
   sh1=RevolvedShape(cont1,2*resol1);

   trf=Transformation;
   trf.origin=mycenter;
   trf.Xaxis=mydir;
   trf.Yaxis=vecnorm(mydir*vector(1.1,2.2,3.3));
   trf.Zaxis=vecnorm(trf.Xaxis*trf.Yaxis);
   sh1.Transform(trf);
   return(AddReflectingObject(sh1,TheReflectingProperties));
}


#Add a spherical lens to the optics set
function AddSphericalLens(mycenter,mydir,myradius,myfocaldistance1,myfocaldistance2,mydepth,TheRefractingProperties)
{
   resol1=40;
   cont1=FlatContourSet;
   ang=asin(myradius/myfocaldistance1);
   oo=myfocaldistance1*cos(ang)-mydepth/2;
   for i=0 to resol1 do {
      a=i/resol1*ang;
      cont1.addpoint(
         point(-1*myfocaldistance1*cos(a)+oo,myfocaldistance1*sin(a))
         ,vector(cos(a),-1*sin(a)));
   }
   cont1.addpoint(point(-0.5*mydepth,myradius));
   cont1.addpoint(point( 0.5*mydepth,myradius));
   ang=asin(myradius/myfocaldistance2);
   oo=myfocaldistance2*cos(ang)-mydepth/2;
   for i=0 to resol1 do {
      a=(1-i/resol1)*ang;
      cont1.addpoint(
         point(myfocaldistance2*cos(a)-oo,myfocaldistance2*sin(a))
         ,vector(-1*cos(a),-1*sin(a)));
   }
   cont1.calcflatnormals();
   
   sh1=RevolvedShape(cont1,2*resol1);
   trf=Transformation;
   trf.origin=mycenter;
   trf.Xaxis=mydir;
   trf.Yaxis=vecnorm(mydir*vector(1.1,2.2,3.3));
   trf.Zaxis=vecnorm(trf.Xaxis*trf.Yaxis);
   sh1.Transform(trf);
   return(AddRefractingObject(sh1,TheRefractingProperties));
}
