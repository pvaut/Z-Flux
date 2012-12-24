codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

ssysframe=ssys_create;
ssys_showconstellations(false);
ssys_createearth(1000);
ssys_createmilkyway;

myviewport=T_getviewport;
myviewport.NearClipPlane=autokm(0.5);
myviewport.FarClipPlane=autokm(80);
myviewport.FocalDistance=autokm(4.5);
myviewport.enableusernavigation=true;
myviewport.EnableUserTimeControl=true;

pttx=ssysframe.createtexture("point",DataDir+"\textures\star1.bmp");
function addlightpoint(frame,sz)
{
   sz=sz/2;
   rc=frame.add("Rectangle","Position":point(-1*sz,-1*sz,0),"Axis1":vector(2*sz,0,0),"Axis2":vector(0,2*sz,0));
   rc.Texture=pttx.name;
   rc.BlendType=BlendTransparent;rc.DepthMask=DepthMaskDisable;rc.enablelight=false;
   return(rc);
}

#sun reference
ssysframe.add("LightPoint","Size":autokm(0.2),"Color":color(1,0.5,0));
ssysframe.add("LightPoint","Size":autokm(0.09),"Color":color(1,1,0.5));
ssysframe.add("LightPoint","Size":autokm(0.04),"Color":color(1,1,1));


#create Mars object
marsframe=ssys_createplanet("Mars",300);
marsframe.inclin.dispose;
textframe=marsframe.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.05)).color=color(0.3,0.3,1);
addlightpoint(textframe,autokm(0.03)).color=color(0,1,1);
addlightpoint(textframe,autokm(0.01)).color=color(1,1,1);
txt=textframe.add("TextFlat","enablelight":false,"color":color(0.6,0.6,0.6),
    "Position":point(autokm(-0.03),autokm(0.03),0),"Content":'Mars',"Size":autokm(0.06));


marsorbit=GetPlanetOrbit("Mars");
marsorbit.color=color(0,0.5,1,0.6);
marsorbit.BlendType=BlendTransparent;
marsorbit.DepthMask=DepthMaskDisable;

#create Earth object
earthframe=GetPlanetFrame("Earth");
earthframe.inclin.dispose;
textframe=earthframe.addviewdirframe(point(0,0,0));
addlightpoint(textframe,autokm(0.05)).color=color(0.3,0.3,1);
addlightpoint(textframe,autokm(0.03)).color=color(0,1,1);
addlightpoint(textframe,autokm(0.01)).color=color(1,1,1);
txt=textframe.add("TextFlat","enablelight":false,"color":color(0.6,0.6,0.6),
    "Position":point(autokm(-0.033),autokm(0.03),0),"Content":'Earth',"Size":autokm(0.06));

earthorbit=GetPlanetOrbit("Earth");
earthorbit.color=color(0,0.5,1,0.6);
earthorbit.BlendType=BlendTransparent;
earthorbit.DepthMask=DepthMaskDisable;


menu=T_createmenu;
menu.sizex=0.2;
menu.add("","1 km/s","VS010");
menu.add("","1.5 km/s","VS015");
menu.add("","2 km/s","VS020");
menu.add("","2.5 km/s","VS025");
menu.add("","4 km/s","VS040");
menu.add("","6 km/s","VS060");
menu.add("","10 km/s","VS100");
menu.add("","20 km/s","VS200");

root.timespeedfactor=1000000;
orbitframe=ssysframe.addsubframe("OrbitFrame");

#create clock
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_date=clockframe.add("Clock","Type":ClockTypeDate,"Position":point(0.02,0.02,0),"Size":0.03);


function simulate(ownspeed)
{
   root.time=time(2010,1,1,0,0,0);
   incrtime;

   orbitframe.clearobjects;
   earthmotion=root.SC.Universe.SolarSystem.Earth.Motions.EarthOrbit;
   earthspeed=(1/(24*60*60))*earthmotion.speed;
   earthpos=earthmotion.position;
   
   mu=132712440018;
   gravforce=ForceField;
   gravforce.AddCentralGravity(point(0,0,0),mu);
   gravforce.SetAccuracy(10000000000,autokm(0.01));
   
   dr1=vecnorm(earthspeed);
   dr2=vecnorm(earthpos-point(0,0,0));
   dr3=vecnorm(dr1*dr2);
   
   ct2=6;   
   for i2=0 to ct2 do {
      ang2=(i2/(ct2)-0.5)*Pi;
      ct1=round(9*((Pi/2-abs(ang2))/(Pi/2)),0);
      for i1=0 to ct1 do {
         ang1=i1/(ct1+1)*2*Pi;
         obj=orbitframe.addsubframe("Obj"+str(i1)+str(i2));
         obj.add("SolidPoint","Size":4,"Color":color(0,0.5,0));
         mt=MotionForceField.create(obj,"Motion");
         mt.ForceField=gravforce;
         obj.transf.origin=earthmotion.position;
         objspeed=cos(ang1)*cos(ang2)*dr1+sin(ang1)*cos(ang2)*dr2+sin(ang2)*dr3;
         objspeed=earthspeed+ownspeed*objspeed;
         obj.velocity=objspeed;
         obj.motion=mt;
         orb=orbitframe.add("Curve","Color":color(0,1,0,0.15),"Size":2,
            "BlendType":BlendTranslucent,"Depthmask":depthmaskdisable);
         orb.track(obj,autokm(0.01));
      }
   }
   
}


while true do {
   incrtime;
   if menu.WasModified then {
      st=menu.SelectID;
      if st.substring(0,1)=="VS" then simulate(ToScalar(st.substring(2,99))/10);
   }
   render;
}

