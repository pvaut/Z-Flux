codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

au2km=149598000;

#Create the scene
ssys_create;
#Show dimmed constellation lines
ssys_showconstellations(true);
root.SC.Universe.StarBackFrame.StarBack.LineColor=color(0,0.5,1,0.12);
#Show milky way
ssys_createmilkyway;
#Create a sun object
ssys_createsun;
#This frame will hold the solar system
frame=root.SC.Universe.SolarSystem;

#Initialise light
scene=T_getscene;
scene.ambientlightcolor=color(0.07,0.07,0.07);
scene.Light0Pos=point(0,0,0);

#Initialise camera position
vp=T_getviewport;
vp.cameradir=vecnorm(vector(0,-1,-0.5));
vp.camerapos=point(0,0,0)-8*au2km*vp.cameradir;
vp.cameraupdir=vector(0,0,1);
vp.enableusernavigation=true;
vp.EnableUserTimeControl=true;
#Initialise clipping planes & perspectivee
vp.NearClipPlane=0.5*au2km;
vp.FarClipPlane=80*au2km;
vp.FocalDistance=8*au2km;

#create clock display object
clockframe=root.SC.Universe.addscreenframe("ClockFrame");
clockframe.EnableLight=false;clockframe.color=color(0.5,0.5,0.5);
info_clock=clockframe.add("Clock","Type":ClockTypeDateTime,"Position":point(0.1,0.05,0),"Size":0.03);


#Define Sun gravitiy field
gravforce=ForceField;
mu=132712440018;
gravforce.AddCentralGravity(point(0,0,0),mu);
gravforce.SetAccuracy(10000000000,0.01*au2km);


#This list will hold all the planets
planetlist=list;

#Define a planet
planet=map;
planet.name="Aarde";
planet.posit=point(1.0*au2km,0,0);# in km
planet.veloc=vector(0,29.77,0);# in km/s
planet.col=color(0.6,0.8,1);
planet.dispradius=0.04*au2km;#displayed radius of the sphere, in km
planetlist.add(planet);

#Example: define a family of planets
foreach dist in list(1.5,2,2.5,3,3.5,4,4.5) do {
   planet=map;
   planet.name="Another";
   planet.posit=point(dist*au2km,0,0);
   planet.veloc=vector(0,29.77,0);
   planet.col=color(1.0,0.8,0.4);
   planet.dispradius=0.03*au2km;
   planetlist.add(planet);
}



#Loop over all planet, and do the stuff
foreach planet in planetlist do {
   #create a subframe to hold the planet
   sf1=scene.addsubframe("sf1","Name":planet.name);
   sf1.Transf.origin=planet.posit;
   #define a force field motion that hooks to the Sun gravity force
   mt=MotionForceField.create(sf1,"Motion");
   mt.ForceField=gravforce;
   sf1.velocity=planet.veloc;
   sf1.motion=mt;
   #create the planet display object as a sphere
   obj=sf1.add("Sphere","Radius":planet.dispradius,"Color":planet.col);
   #track the orbit
   orb=frame.add("Curve","Color":color(0,1,0,0.15),"Size":2,
      "BlendType":BlendTranslucent,"Depthmask":depthmaskdisable);
   orb.track(sf1,0.01*au2km);
}





#Set start time and initial speed factor
root.time=time(2000,1,1,0,0,0);
root.TimeSpeedFactor=2000000;

#Animation render loop
while true do {
   incrtime;
   render;
}