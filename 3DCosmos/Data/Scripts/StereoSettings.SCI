codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

configlist=list;
configidnr=0;

function AddConfig(parentid,name,thesett)
{
   configidnr=configidnr+1;
   theconf=map;
   theconf.parentid=parentid;
   theconf.name=name;
   theconf.id=str(configidnr);
   theconf.settings=thesett;
   configlist.add(theconf);
}


function MakeDefaultSettings(thesett)
{
   thesett.ShowStereo=true;
   thesett.SwapLeftRight=false;

   thesett.MirrorLeftH=false;
   thesett.MirrorLeftV=false;
   thesett.MirrorRightH=false;
   thesett.MirrorRightV=false;
   
   thesett.HorizontalStretch=1.0;
   thesett.EyeSepFactor=0.025;

   thesett.DisplayName="";
   thesett.FullScreen=false;

   thesett.WinOffsetX="";
   thesett.WinOffsetY="";
   thesett.WinSizeX="";
   thesett.WinSizeY="";
}


function ReStart()
{
   Execute(ExecDir+"\z-flux.exe",execdir,false);
   ExitProgram;
}

##################################################################################################"
# Predefined settings
##################################################################################################"

sett=map;
MakeDefaultSettings(ref(sett));
sett.ShowStereo=false;
addconfig("Presets",'Full screen mono',sett);

sett=map;
MakeDefaultSettings(ref(sett));
sett.HorizontalStretch=0.5;
sett.WinOffsetX="0";
sett.WinOffsetY="0";
sett.WinSizeX="1920";
sett.WinSizeY="1080";
addconfig("Presets",'3DHD Side-By-Side stereo (1920x1080)',sett);

sett=map;
MakeDefaultSettings(ref(sett));
sett.WinOffsetX="0";
sett.WinOffsetY="0";
sett.WinSizeX="2048";
sett.WinSizeY="768";
addconfig("Presets",'Dual output stereo (2x1024x768)',sett);

sett=map;
MakeDefaultSettings(ref(sett));
sett.WinOffsetX="0";
sett.WinOffsetY="0";
sett.WinSizeX="2560";
sett.WinSizeY="720";
addconfig("Presets",'Dual output stereo (2x1280x720)',sett);

sett=map;
MakeDefaultSettings(ref(sett));
sett.WinOffsetX="0";
sett.WinOffsetY="0";
sett.WinSizeX="3840";
sett.WinSizeY="720";
addconfig("Presets",'Dual output stereo (2x1920x1080)',sett);

sett=map;
MakeDefaultSettings(ref(sett));
addconfig("Presets",'Dual output stereo (auto)',sett);


sett=map;
sett.FrameRate=30;
sett.SyncFactor=1;
addconfig("Rates",'30 fps',sett);

sett=map;
sett.FrameRate=60;
sett.SyncFactor=1;
addconfig("Rates",'60 fps',sett);

sett=map;
sett.FrameRate=60;
sett.SyncFactor="";
addconfig("Rates",'60 fps (no sync)',sett);

sett=map;
sett.FrameRate="";
sett.SyncFactor="";
addconfig("Rates",'Max rate',sett);


##################################################################################################"
# START OF SCRIPT
##################################################################################################"

T_scene_create;
sc=T_getscene;
myviewport=T_getviewport;
myviewport.CameraPos=point(0,0,1);
myviewport.CameraDir=vector(0,0,-1);
myviewport.FocalDistance=1;

menu=T_createmenu("Stereo settings");
menu.sizex=0.4;
menu.Color=color(1,1,1,1);
root.SC.Universe.MenuFrame.Color=color(1,1,1,1);
root.mousedampingfactor=0.25;
root.SC.Universe.MenuFrame.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1.5,1),0.03);



menu.Add("",'Preset stereo configurations',"Presets");
menu.Add("",'Frame rate',"Rates");
menu.Add("","","-");


menu.Add("",'Custom Stereo Settings',"GeneralSettings");
menu.Add("",'Color Settings',"ColorSettings");

T_menu_appendclose;

foreach conf in configlist do {
   menu.Add(conf.parentid,conf.name,conf.id);
}


ActivateControl(menu);

menufinished=false;
while not(menufinished) do {
   if menu.WasModified then {
      menufinished=true;
      currentfolder=menu.SelectID;
   }
   render;
}
menuid=menu.SelectID;
T_delmenu;

if menuid=="GeneralSettings" then ExecuteScript(ScriptFilePath+"\_Sub\GeneralSettings.SCI");
if menuid=="ColorSettings" then ExecuteScript(ScriptFilePath+"\_Sub\ColorSettings.SCI");

foreach conf in configlist do
   if conf.id==menuid then {
      settlist=conf.settings.GetNames;
      foreach settname in settlist do
         WriteSetting(settname,conf.settings.get(settname));
      ReStart;
   }




