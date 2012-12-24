codeblock readtextfile(ScriptDir+"\presentations\_presentation.sci");

showall=true;


SetAnimation("None");
Slide(" ");
skipy(1);
indent(0.1);
text("\textbf{Z-Flux:}","size":3);
text("\textbf{an introduction}","size":3);
Click;



##################################################################################################
SetAnimation("MoveForward");
if showall then {
Slide("What is Z-Flux?");
Click;


text("In a nutshell: a freeware software program for real-time, interactive rendering of script-based digital animations in stereo");
Click;

text("\textbf{Free.} You can download and use Z-Flux free of charge","Bullet":true);
Click;

text("\textbf{Real-time.} All renderings are performed real-time, typically at 60 frames per second","Bullet":true);
Click;

text("\textbf{Interactive.} The user can control key aspects of an animation while it is running.","Bullet":true);
Click;

text("\textbf{Stereo.} Z-Flux renders side-by-side stereo images","Bullet":true);
Click;

text("\textbf{Flexible & extensible.} The engine of Z-Flux is a powerful script language, FluxScript, driving all animations.","Bullet":true);
Click;
}

##################################################################################################
if showall then {
SetAnimation("MoveBackward");
Slide("Origin of Z-Flux");
Click;

text("Originated from the public observatory \textit{UGent Volkssterrenwacht Armand Pien}.");

text("The purpose was to build an \textbf{educational} stereo projection facility used to show 3D animations about \textbf{astronomy} and \textbf{space exploration}.");

Click;

SetAnimationSpeed(0.5);
pd=3.0;
Picture(datadir+"\\Scripts\Presentations\_Pictures\vs1.jpg",point(0.5,GetCurPosY,-1),3.0);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\telescoop.jpg",point(6,GetCurPosY+0.5,-1),3.0);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\sterrenwacht_luchtfoto.jpg",point(0,GetCurPosY+0.5,0),3.0);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\bezoek1.jpg",point(5,GetCurPosY-0.25,0),2.0);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\leden.jpg",point(8,GetCurPosY+1.5,0),2.0);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\vs2.jpg",point(2,GetCurPosY+1.5,1),1.5);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\telescoop3.jpg",point(5.5,GetCurPosY+1,1),2);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\weerstation.jpg",point(2.5,GetCurPosY-1,1),1.7);
Sleep(pd);
Picture(datadir+"\\Scripts\Presentations\_Pictures\school2.jpg",point(8,GetCurPosY-0.5,1),1.5);
SetAnimationSpeed(1);

Click;
PresentScript(scriptdir+"\Stereo pictures\02_Slide shows.SCI",datadir+"\SlideShows\VSRUG.SCI");
Click;

PresentScript(scriptdir+"\Astronomy\01_Earth\seasons.SCI","");
PresentScript(scriptdir+"\Astronomy\04_Saturnus\Saturnus + Moons.SCI","");
PresentScript(scriptdir+"\Space\Models\ISS.SCI","");
Click;
}

##################################################################################################
if showall then {
Slide("Advantages of Z-Flux");
Click;

text("High level scripting language allows quick development of powerful animations","Bullet":true);
indent(0.05);
text("Powerful mathematice engine","Bullet":true,"Size":0.8);
text("Knowlegde of laws of physics","Bullet":true,"Size":0.8);
text("Advanced geometry tools","Bullet":true,"Size":0.8);
outdent(0.05);
Click;
PresentScript(scriptdir+"\Physics\spherical pendulum.SCI","");

Picture(datadir+"\\Scripts\Presentations\_Pictures\script_pendulum.png",point(2,1,0.1),7.0);
Click;
RemoveLastItem;

skipy(0.2);
text("Reads & displays 3DS files (standard 3d model format)","Bullet":true);
Click;

text("Resolution and projection mode independent","Bullet":true);
Click;
text("Built-in color correction for Infitec projection","Bullet":true);
Click;
text("Warped projection possible (dome projection)","Bullet":true);
Click;

text("A variety of control devices can be used","Bullet":true);
indent(0.05);
text("Keyboard & mouse","Bullet":true);
text("(Wireless) Gamepad","Bullet":true);
text("SpaceConnector (3D pointing device)","Bullet":true);
outdent(0.05);
}

##################################################################################################
if showall then {
SetAnimation("ZoomX");
Slide("Z-Flux for stereo photography");
text("Interest by the stereo photography community triggered some extra developments:");
Click;
text("A fancy stereo foto browser","Bullet":true);
Click;
text("A powerful tool to create stereo slide shows","Bullet":true);
Click;
text("An interactive, menu based environment to browse stereo content","Bullet":true);
Click;
text("Stereo video playing (with sound)","Bullet":true);
Click;
text("A 3D ~"PowerPoint~" presentation builder (=this show!)","Bullet":true,"Color":color(0.9,0.5,0));
Click;
}
##################################################################################################
if showall then {
SetAnimation("MoveForward");
Slide("Stereo foto browser");
Click;
text("Automatic generation of stereo thumbnails","Bullet":true);
Click;
text("~"CoverFlow~"-style 3D animation of stereo thumbnails","Bullet":true);
Click;
PresentScript(scriptdir+"\Stereo pictures\01_Browse Stereo pictures.SCI",datadir+"\StereoPictures\Winter2008_2009");
Click;
}


##################################################################################################
if showall then {
SetAnimation("FadeIn");
Slide("Stereo slide show creator");
Click;
text("Script based: easy to adapt and modify","Bullet":true);
Click;
text("Variety of true 3D transitions","Bullet":true);
Click;
text("Possible to show several images simultaneously in 3D overlay","Bullet":true);
Click;
text("Zooming an panning on stereo images","Bullet":true);
Click;
text("Play stereo videos in the show","Bullet":true);
Click;
text("Powerful sound tools (fade in & out, mix, synchronise)","Bullet":true);
Click;

Picture(datadir+"\\Scripts\Presentations\_Pictures\script_slideshow.png",point(5,2,1),5.0);
Click;

for i=1 to 7 do RemoveLastItem;
skipy(-4.3);
text("\textbf{picturefolder, videofolder, soundfolder}: set source data folders");
text("\textbf{transition}: chose the animation between two pictures");
text("\textbf{showstereopic}: shows a stereo image");
text("\textbf{panstereopic}: pans/zooms on a stereo image");
text("\textbf{showstereovideo}: shows a stereo video");
text("\textbf{showtext}: displays text in overlay");
text("\textbf{playsound,fadesound,stopsound}: sound manipulation");
text("\textbf{delay}: waits for a number of seconds");
text("\textbf{waitclick}: waits for a user action");
text("\textbf{loop}: automatically restarts");
Click;
}

##################################################################################################
if showall then {
SetAnimation("MoveBackward");
Slide("Interactive menu based environment");
Click;
text("Can offer any Z-Flux content in a easy-to-use browsable environment");
text("=Ideal for self-exploration by visitors (museums, ...)");
Click;

PresentScript(scriptdir+"\02_user menu.sci","");
Click;
skipy(0.5);
text("Intuitive: small learning curve","Bullet":true);
Click;
text("Robust (only input device is mouse or Bamboo touchpad)","Bullet":true);
Click;
text("Easy to modify content","Bullet":true);
Click;

for i=1 to 5 do RemoveLastItem;
Picture(datadir+"\\Scripts\Presentations\_Pictures\usermenu1.png",point(0.5,1,-1),6.0);
Click;
Picture(datadir+"\\Scripts\Presentations\_Pictures\usermenu2.png",point(4.5,3,1),3.0);
Click;

}

##################################################################################################
if showall then {
Slide("3D presentation builder");
text("= a very simple PowerPoint, but with 3D capacities");
Click;
#skipy(1);
text("Easy and simple make-up","Bullet":true);
Click;
text("Display of text, mono & stereo images/videos","Bullet":true);
Click;
text("Choice between a variety of 3D animation effects","Bullet":true);
Click;
st="";
st=st+"Math formulas: $ ";
st=st+"\sum_{i=1}^n i^2 + \left| \, \vec{v} \times \vec{w} \right| + x_1^2 +  \sin \beta - (1+x)^2 + \sqrt{ \frac{1}{1+x} }";
st=st+" $";
text(st,"Bullet":true);
Click;

text("Inclusion of any Z-Flux animation in the presentation","Bullet":true);
Click;
text("Script based","Bullet":true);
Click;
}

##################################################################################################
Slide(" ");
Picture(datadir+"\\Scripts\Presentations\_Pictures\script_present.png",point(0,0.5,-0.1),7.0);
Click;

Slide(" ");
delay(1);

stop;
