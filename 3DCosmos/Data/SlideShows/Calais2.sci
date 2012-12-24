
Picturefolder(datadir+"\StereoPictures\Calais");
Transition("Zoom",2);
SoundFolder(datadir+"\sounds");



ShowStereoPic("calaisbliksem1","Size":0.5,"Position":point(-0.05,0,-0.3));

#Delay(3);
AutoHidePrevious(false);


ShowStereoPic("calaisbliksem2","Size":0.5,"Position":point(0.15,0,0));
Delay(5);

AutoHidePrevious(true);

Transition("Fade",2);

ShowStereoPic("zon");
Delay(6);

loop;



