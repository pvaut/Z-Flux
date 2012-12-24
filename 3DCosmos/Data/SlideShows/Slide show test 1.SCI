
Picturefolder(datadir+"\StereoPictures\Folder 1");
mydelay=4;
Transition("Transient",1);

SoundFolder(datadir+"\sounds");
VideoFolder(datadir+"\StereoVideos");



PlaySound("sound1.mp3",0);
FadeSound("sound1.mp3",1000,2);

ShowStereoPic("alpen1","Size":0.5,"Position":point(0,0,0));

Delay(1);

PanStereopic("alpen1","Duration":3,"Position":point(0,0,-0.5));
Delay(3);

PanStereopic("alpen1","Duration":3,"Position":point(0.05,0,-0.5));
Delay(4);


AutoHidePrevious(false);
ShowStereoPic("alpen2","Size":0.8,"Position":point(0,0,0));


ShowText("Test",point(-0.5,-0.5*2/3,0),"Size":0.1);

Delay(mydelay);

PlaySound("sound2.mp3",0);
FadeSound("sound2.mp3",1000,2);

FadeSound("sound1.mp3",0,2);

AutoHidePrevious(true);
ShowStereoPic("screendump8");
Delay(mydelay);

StopSound("sound1.mp3");

hidestereopic("screendump3");
delay(mydelay);
ShowStereoVideo("Test1");


ShowStereoPic("screendump9");
FadeSound("sound2.mp3",0,mydelay);
Delay(mydelay);

hidestereopic("screendump9");delay(mydelay);
ShowStereoVideo("Test2");

StopSound("sound2.mp3");
loop;
