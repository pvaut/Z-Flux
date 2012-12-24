

codeblock readtextfile(ScriptDir+"\TOOLS.sci");
codeblock readtextfile(ScriptDir+"\SSYS.sci");

codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_general.sci");
codeblock readtextfile(ScriptDir+"\MarsProject\Sub\SUB_CreateMarsSolarMovement.sci");

JID=0;

myviewport=T_getviewport;

ssysframe=ssys_create;
ssys_showconstellations(false);
#ssys_createearth(1000);
ssys_createmilkyway;

SUB_init;
PanelLeft=SUB_createleftpanel;


SUB_MarsSolarMovementAnimate(deg2rad(0),deg2rad(40));
