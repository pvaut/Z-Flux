
Picturefolder(datadir+"\StereoPictures\VSRUG");
mydelay=15;


#intro
Transition("Transient",2);

ShowStereoPic("2");
Delay(mydelay);

ShowStereoPic("1");
Delay(mydelay);



ShowStereoPic("3");
Delay(mydelay);

#grasveld
Transition("Curtain",2);
ShowStereoPic("5");
Delay(mydelay);

ShowStereoPic("6");
Delay(mydelay);

ShowStereoPic("7");
Delay(mydelay);


#binoc
Transition("Pan",2);
ShowStereoPic("9");
Delay(mydelay);

ShowStereoPic("10");
Delay(mydelay);

ShowStereoPic("11");
Delay(mydelay);

ShowStereoPic("12");
Delay(mydelay);

#old telescope
Transition("Zoom",3);
ShowStereoPic("14");
Delay(mydelay);

ShowStereoPic("15bis","Position":point(0,0,0),"Size":1);
Delay(mydelay);
PanStereopic("15bis","Duration":6,"Position":point(-0.14,-0.1,-0.5));
Delay(mydelay);
PanStereopic("15bis","Duration":3,"Position":point(0,0.1,-0.5));
Delay(mydelay);


#stereo equipment
Transition("Transient",2);
ShowStereoPic("16");
Delay(mydelay);

Transition("Zoom",3);

ShowStereoPic("17");
Delay(mydelay);

#bar
ShowStereoPic("18");
Delay(1);
ShowText("The bar",point(-0.4,-0.35,-0.15),"size":0.1,"color":color(1,0.25,0));
Delay(mydelay);

end;


