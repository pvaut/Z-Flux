Picturefolder(datadir+"\StereoPictures\Folder 1");
SoundFolder(datadir+"\sounds");
VideoFolder(datadir+"\StereoVideos");

Transition("Transient",2);

PlaySound("sound1.mp3",0);FadeSound("sound1.mp3",1000,2);

ShowStereoPic("Image1");
Delay(4);
ShowStereoPic("Image2");
Delay(6);
ShowStereoPic("Image3");
Delay(4);

FadeSound("sound1.mp3",0,1);
delay(5);
ShowStereoVideo("Test1");

StopSound("sound1.mp3");
loop;
