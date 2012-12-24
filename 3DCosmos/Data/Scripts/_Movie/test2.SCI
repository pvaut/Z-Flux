
codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");

codeblock readtextfile(ScriptDir+"\_Movie\_PlanetariumTools.sci");

###########################################################################################################
###########################################################################################################
###########################################################################################################




###########################################################################################################
###########################################################################################################
###############                                          START                              ###############
###########################################################################################################
###########################################################################################################


mydata=map;
mydata.scfac=100;
mydata.starsizefrac=0.008;
mydata.longit=(3+43/60.0)/180*Pi;
mydata.lattit=(51+3/60.0)/180*Pi;
mydata.camh=-1.99;
InitPlanetarium(ref(mydata));
mydata.clipbelowhorizon=true;
#mydata.animateatmosphere=false;
CreatePlanetariumClock;

#Hide all atmospheric effects
#root.SC.Universe.SolarSystem.Earth.SunHalo.Large.visible=false;
#root.SC.Universe.SolarSystem.Earth.SunHalo.RiseSet.visible=false;
root.SC.Universe.SolarSystem.Earth.MoonHalo.visible=false;

#backdrop
CreateBackDrop(ref(mydata));

myviewport=T_getviewport;
myviewport.Cameradir=vecnorm(vector(0.00001,0,1));
myviewport.FocalDistance=2.0*mydata.scfac;
myviewport.EyeSeparation=0.05*myviewport.FocalDistance;
myviewport.Aperture=60/180*Pi;


#hide the moon
root.SC.Universe.SolarSystem.Earth.Luna.visible=false;

#initially, hide the names of the planets
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Jupiter.Name.visible=false;
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Saturnus.Name.visible=false;


root.SC.Universe.ClockFrame.Clock2.Type=ClockTypeCalendar;

root.time=time(2000,9,1,3,0,0);

framenr=0;
root.FrameRate=30;
root.TimeSpeedFactor=3000;
jump=1;




FadeViewportsIn;

animate(100);

stop;

for i=1 to 50 do {
   root.time=root.time+364/365*24*60*60;
   animate(0.03);
}

#display the planet names
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Jupiter.Name.visible=true;
root.SC.Universe.SolarSystem.Earth.PlanetsIndicators.Saturnus.Name.visible=true;


for i=1 to 10000 do {
   root.time=root.time+364/365*24*60*60;
   animate(0.03);
}

FadeViewportsOut;

stop;
