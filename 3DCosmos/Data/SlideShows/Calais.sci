
Picturefolder(datadir+"\StereoPictures\Calais");
Transition("Zoom",2);
SoundFolder(datadir+"\sounds");



ShowStereoPic("calaisbliksem1");
Delay(2);
ShowText("Ziehier een bliksem",point(-0.2,-0.2,0.15),"Size":0.025,"Color":color(1,0.7,0));

PlaySound("sound2.mp3",1000);
Delay(4);

ShowText("Dit staat boven",point(-0.2,0.2,0.15),"Size":0.025,"Color":color(1,0.7,0));


Delay(5);
FadeSound("sound2.mp3",0,10);

PlaySound("sound3.mp3",0);
FadeSound("sound3.mp3",1000,2);

Transition("Curtain",2);

ShowStereoPic("calaisbliksem2");
Delay(10);

Transition("Fade",2);

ShowStereoPic("zon");
Delay(6);

end;


