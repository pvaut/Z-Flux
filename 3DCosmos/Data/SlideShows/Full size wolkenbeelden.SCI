
Picturefolder(datadir+"\StereoPictures\Full size wolken beelden");
mydelay=3;
Transition("Pan",2);

SoundFolder(datadir+"\sounds");

PlaySound("sound3.mp3",1000);
Delay(1);

ShowStereoPic("0001");
Delay(2);
ShowText("Dit is de eerste foto",point(-0.25,-0.2,0.1),"Size":0.05,"Color":color(1,0.7,0));
Delay(mydelay);

ShowStereoPic("0002");
Delay(2);
ShowText("En dit de tweede foto",point(-0.25,-0.2,0.1),"Size":0.05,"Color":color(1,0.7,0));
Delay(mydelay);

ShowStereoPic("0003");
Delay(mydelay);

ShowStereoPic("0004");
Delay(mydelay);

ShowStereoPic("0005");
FadeSound("sound3.mp3",0,mydelay);
Delay(mydelay);

end;

